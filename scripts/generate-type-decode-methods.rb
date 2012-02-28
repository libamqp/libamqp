#!/usr/bin/ruby
#
require 'rexml/document'
require 'generate-common'

$xpaths = {}
$xpaths['messaging'] = '//section[@name=\'message-format\']/type[descriptor]'
$xpaths['delivery-state'] = '//section[@name=\'delivery-state\']/type'
$xpaths['addressing'] = '//section[@name=\'addressing\']/type[descriptor]'

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
    "amqp_#{multiple_prefix f}#{base_xml_name f}_cleanup(context, &messaging_type->#{field_name f});" 
  end
  
  def cleanup_calls(e, t)
    result = ""
    e.elements.each('field') do |f| 
      result += "    " + cleanup_call(e, f, t)
      result += "\n"
    end
    result
  end
  
  def type_type(name)
    "amqp_#{name}_t"
  end

  def is_null_arg(f)
    return "&messaging_type->#{field_name f}_is_null, " if optional_primitive?(f)
    ""
  end

  def decode_field_call(element, field, tunnel, assignment, name)
    return todo(field) if !has_mapping?(field)    
    "#{assignment}amqp_decode_#{mandatory_prefix field}#{multiple_prefix field}#{type_or_primitive field}#{base_xml_name field}#{requires_suffix field}(" +
            "context, amqp_field_from_list(field_list, field_number), field_number, total_fields, " +
            "#{is_null_arg field}" +
            "&messaging_type->#{field_name field}" +
            "#{default_arg field}); field_number++;"
  end
  
  def decode_calls(e, t, name)
    result = ""
    assign = "rc = "
    e.elements.each('field') do |f| 
      result += "    " + decode_field_call(e, f, t, assign, name)
      result += "\n"
      assign = "rc = rc && "
    end
    result
  end    
  
  def method(e, section)
    raw_name = e.attributes['name']
    name = raw_name.gsub(/-/,'_')
    section = section.gsub(/-/,'_')
    title = raw_name.split('-').collect{|x| x.capitalize}
    if (title[0] == 'Sasl')
      title[0].upcase!
      title = title.join(' ')
      tunnel = 'sasl'
    else
      title = "AMQP " + title.join(' ')
      tunnel = 'amqp'
    end
    qname = "#{section}_#{name}"
    total_fields = e.elements.count - 1

    puts(<<-eos)
/*
    #{e}
*/ 
\#ifdef AMQP_DECODE_GENERATED_HEADER
extern int amqp_decode_#{qname}_type(amqp_context_t *context, amqp_buffer_t *buffer, #{type_type(qname)} *messaging_type, amqp_type_t *field_list);
\#endif
\#ifdef AMQP_DECODE_GENERATED_BODY
static void cleanup_#{qname}_type(amqp_context_t *context, void *type_for_cleanup)
{
    #{type_type(qname)} *messaging_type = (#{type_type(qname)} *) type_for_cleanup;

#{cleanup_calls(e, tunnel)}}
int amqp_decode_#{qname}_type(amqp_context_t *context, amqp_buffer_t *buffer, #{type_type(qname)} *messaging_type, amqp_type_t *field_list)
{
    const int total_fields = #{total_fields};
    int field_number = 0;
    int rc;
    
    messaging_type->cleanup = cleanup_#{qname}_type;

#{decode_calls(e, tunnel, qname)}
    assert(field_number = total_fields);
    if (rc == 0)
    {
        amqp_decode_messaging_type_error(context, "#{title}");
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
      puts "// XPath key: #{key}"
      document.root.each_element(value) { |e| method(e, key) }
    end
  end
end

if ARGV[0] 
  Parser.new(ARGV[0]).parse
else
  puts('No XML file provided')
end
