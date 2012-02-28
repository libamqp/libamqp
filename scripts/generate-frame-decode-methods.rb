#!/usr/bin/ruby
#
require 'rexml/document'
require 'generate-common'

$xpaths = {}
$xpaths['transport'] = '//section[@name=\'performatives\']/type[descriptor]'
$xpaths['security'] = '//section[@name=\'sasl\']/type[descriptor]'

$enum_value_mapping = {}
$enum_value_mapping['mixed'] = 'amqp_sender_settle_mode_mixed'
$enum_value_mapping['first'] = 'amqp_receiver_settle_mode_first'

class Parser
  include Common
  
  def initialize(xml_file)
    @xml_file = xml_file
  end      

  def xml_name(f)
    m = field_mapping f
    m.xml_name 
  end

  def base_xml_name(f)
    m = field_mapping f
    m.base_xml_name 
  end

  def c_name(f)
    m = field_mapping f
    m.xml_name(f) 
  end

  def multiple_prefix(f)
    multiple?(f) ? "multiple_" : ""
  end
  def mandatory_prefix(f)
    mandatory?(f) ? "mandatory_" : ""
  end

  def mandatory_arg(f)
    # mandatory?(f) ? "mandatory" : "optional"
    ""
  end
  
  def type_or_primitive(f)
    primitive?(f) ? "primitive_" : "type_"
  end
  
  def requires_suffix(f)
    "_expecting_#{f.attributes['requires'].gsub(/-/,'_')}" if f.attributes['requires']
  end
  
  def default_arg_value(f)
    v=f.attributes['default']
    $enum_value_mapping[v] || v || "AMQP_NO_DEFAULT_VALUE"
  end
  
  def default_arg(f)
    return "" if mandatory?(f)
    return "" unless primitive?(f)
    ", #{default_arg_value(f)}"
  end
  
  def group(e)
    e.attributes['name'].split('-').last
  end
  def field_name(f)
    f.attributes['name'].gsub(/-/,'_')
  end
  def frame_field_name(e, f, t)
    "#{t}.#{group e}.#{field_name f}"
  end
  
  def todo(f)
    "// TODO - Missing or confusing mapping for #{f}\nabort();"
  end  
  
  def cleanup_call(e, f, t)
    return "// #{field_name(f)}" if primitive?(f)
    "amqp_#{multiple_prefix f}#{base_xml_name f}_cleanup(context, &frame->frames.#{frame_field_name e, f, t});" 
  end
  
  def cleanup_calls(e, t)
    result = ""
    e.elements.each('field') do |f| 
      result += "    " + cleanup_call(e, f, t)
      result += "\n"
    end
    result
  end

  def field_value_arg(e, f, t)
    "&frame->frames.#{frame_field_name(e, f, t)}"
  end

  def is_null_arg(e, f, t)
    return "#{field_value_arg(e, f, t)}_is_null, " if optional_primitive?(f)
    ""
  end

  def decode_field_call(element, field, tunnel, padding)
    return todo(field) if !has_mapping?(field)
    "    // #{field_name field}\n" +
    "    #{padding}amqp_decode_#{mandatory_prefix field}#{multiple_prefix field}#{type_or_primitive field}#{base_xml_name field}#{requires_suffix field}(" +
            "context, amqp_field_from_list(field_list, field_number), field_number, total_fields, " +
            "#{is_null_arg(element, field, tunnel)}" +
            "#{field_value_arg(element, field, tunnel)}" +
            "#{default_arg field}); field_number++;"         
  end
  
  def decode_calls(e, t)
    result = ""
    assign = "rc = "
    e.elements.each('field') do |f| 
      # result += "    // #{f}\n"
      result += decode_field_call(e, f, t, assign)
      result += "\n"
      assign = "rc = rc && "
    end
    result
  end    
  
  def method(e)
    raw_name = e.attributes['name']
    lname = raw_name.split('-').last
    name = raw_name.gsub(/-/,'_')
    title = raw_name.split('-').collect{|x| x.capitalize}
    total_fields = e.elements.count - 1
    if (title[0] == 'Sasl')
      title[0].upcase!
      title = title.join(' ')
      tunnel = 'sasl'
      assert_stmt = ""
    else
      title = "AMQP " + title.join(' ')
      tunnel = 'amqp'
      assert_stmt = "assert(total_fields < 32);               // protect against null_flags overflow"
    end
    qname = "#{tunnel}_#{lname}"

    puts(<<-eos)
/*
    #{e}
*/ 
\#ifdef AMQP_DECODE_GENERATED_HEADER
extern int amqp_decode_#{qname}_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list);
\#endif
\#ifdef AMQP_DECODE_GENERATED_BODY
static void cleanup_#{qname}_frame(amqp_context_t *context, amqp_frame_t *frame)
{
#{cleanup_calls(e, tunnel)}}
int amqp_decode_#{qname}_frame(amqp_context_t *context, amqp_buffer_t *buffer, amqp_frame_t *frame, amqp_type_t *field_list)
{
    const int total_fields = #{total_fields};
    int field_number = 0;
    int rc;
    
    #{assert_stmt}
    frame->dispatch = amqp_dispatch_#{qname};
    frame->cleanup = cleanup_#{qname}_frame;

#{decode_calls(e, tunnel)}
    assert(field_number = total_fields);
    if (rc == 0)
    {
        amqp_decode_frame_error(context, "#{title}");
    }

    return rc;  
}   
\#endif
eos
  end

  def parse
    guard = File.basename(@xml_file, ".bare.xml").downcase
    puts($header)
    xml_file = File.new(@xml_file)
    document = REXML::Document.new(xml_file)
    $xpaths.each do |key, value| 
      document.root.each_element(value) { |e| method e }
    end
  end
end

if ARGV[0] 
  Parser.new(ARGV[0]).parse
else
  puts('No XML file provided')
end
