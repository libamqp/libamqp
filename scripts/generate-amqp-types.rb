#!/usr/bin/ruby
#
require 'rexml/document'
require 'generate-common'

$xpaths = {}
$xpaths['transport'] = '//section[@name=\'performatives\']/type[descriptor]'
$xpaths['security'] = '//section[@name=\'sasl\']/type[descriptor]'
# $xpaths['definition'] = '//section[@name=\'definitions\']/type[descriptor]'
$xpaths['messaging'] = '//section[@name=\'message-format\']/type[descriptor]'
$xpaths['delivery-state'] = '//section[@name=\'delivery-state\']/type'
$xpaths['addressing'] = '//section[@name=\'addressing\']/type[descriptor]'
$xpaths['txn'] = '//section[@name=\'coordination\']/type[descriptor]'

class Parser
  include AttributeTests
  include Common

  def initialize(xml_file)
    @xml_file = xml_file
  end      
  def emit_field(f, name, options = {})
    type = f.attributes['type']
    mapper = $mappings[type]
    mapper.output(name, type, options) if mapper
    puts("// TODO - define #{type}") unless mapper
  end  
  def emit_descriptor(name, type, options = {})
    mapper = $mappings[type]
    mapper.output(name, type, options) if mapper
    puts("// TODO - define #{type}") unless mapper
  end
  def field(f)
    name = f.attributes['name'].gsub(/-/, '_')
    # type = f.attributes['type'].gsub(/-/, '_')
    options = {}
    options[:mandatory] = f.attributes['mandatory'] == 'true'
    options[:multiple] = f.attributes['multiple'] == 'true'
    options[:default] = f.attributes['default']
    options[:requires] = f.attributes['requires']
    emit_field(f, name, options) 
  end
  def typedefs(n, prefix)
    name = n.attributes['name'].gsub(/-/, '_')
    puts "typedef struct amqp_#{prefix}_#{name}_t amqp_#{prefix}_#{name}_t;"
  end 
  def described e, d
    puts "    // described #{d.attributes['name']}" 
    emit_descriptor(e.attributes['name'].gsub(/-/,'_'), d.attributes['name'].split(':').last)
  end
  def struct(n, prefix)
    puts(<<eos)
    
/*
    #{n}
*/
eos
    name = n.attributes['name'].gsub(/-/, '_')
    puts "struct amqp_#{prefix}_#{name}_t {"
    if n.elements.count == 1
      described n, n.elements.first
    else
      puts "    // #{n.elements.count - 1} fields"
      n.elements.each('field') {|f| field(f) }
    end
    puts "};"
  end

  def parse
    guard = File.basename(@xml_file, ".bare.xml").downcase
    puts($header)
    puts(<<-eos)
\#ifndef LIBAMQP_AMQP_AMQP_#{guard.upcase}_H
\#define LIBAMQP_AMQP_AMQP_#{guard.upcase}_H
\#ifdef __cplusplus
extern "C" {
\#endif

\#include "AmqpTypes/AmqpTypes.h"
    
eos
    xml_file = File.new(@xml_file)
    document = REXML::Document.new(xml_file)
    $xpaths.each do |key, value| 
      prefix = key.gsub(/-/,'_')
      document.root.each_element(value) { |e| typedefs e, prefix }
      document.root.each_element(value) { |e| struct e, prefix }
    end
    puts(<<-eos)
\#ifdef __cplusplus
}
\#endif
\#endif
eos
  end
end

if ARGV[0] 
  Parser.new(ARGV[0]).parse
else
  puts('No XML file provided')
end
