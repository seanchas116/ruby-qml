require 'qml/c_library'

module QML

  class Variant

    TYPE_VOID = 43
    TYPE_BOOL = 1
    TYPE_INT = 2
    TYPE_DOUBLE = 6
    TYPE_Q_STRING = 10
    TYPE_Q_VARIANT_LIST = 9
    TYPE_Q_VARIANT_MAP = 8
    TYPE_Q_VARIANT_HASH = 28

    def self.from_raw_pointer(ptr)
      Variant.new.tap do |variant|
        variant.pointer = ptr
      end
    end

    def self.call_c(function_name, *arg)
      CLibrary.send("qvariant_#{function_name}", *arg)
    end

    def initialize(val = nil)
      self.value = val unless val.nil?
    end

    def value
      case type_number
      when TYPE_BOOL
        call_c(:to_int) != 0
      when TYPE_INT
        call_c(:to_int)
      when TYPE_DOUBLE
        call_c(:to_float)
      when TYPE_Q_STRING
        result = nil
        call_c(:get_string, lambda { |str| result = str.force_encoding("utf-8") })
        result
      when TYPE_Q_VARIANT_LIST
        result = []
        call_c(:get_array, lambda { |ptr| result << Variant.from_raw_pointer(ptr).value })
        result
      when TYPE_Q_VARIANT_HASH, TYPE_Q_VARIANT_MAP
        result = {}
        call_c(:get_hash, lambda { |str, ptr| result[str.force_encoding("utf-8").to_sym] = Variant.from_raw_pointer(ptr).value })
        result
      else
        nil
      end
    end

    def value=(val)
      self.pointer =
        case val
        when true
          Variant.call_c(:from_boolean, 1)
        when false
          Variant.call_c(:from_boolean, 0)
        when Integer
          Variant.call_c(:from_int, val)
        when Float
          Variant.call_c(:from_float, val)
        when String
          Variant.call_c(:from_string, val)
        when Array
          ffi_values = val.map { |value| Variant.new(value).pointer }
          ffi_values_array = FFI::MemoryPointer.new(:pointer, val.length).write_array_of_pointer(ffi_values)
          Variant.call_c(:from_array, val.length, ffi_values_array)
        when Hash
          ffi_keys = val.each_key.map { |k| FFI::MemoryPointer::from_string(k.to_s) }
          ffi_keys_array = FFI::MemoryPointer.new(:pointer, val.length).write_array_of_pointer(ffi_keys)
          ffi_values = val.each_value.map { |v| Variant.new(v).pointer }
          ffi_values_array = FFI::MemoryPointer.new(:pointer, val.length).write_array_of_pointer(ffi_values)
          Variant.call_c(:from_hash, val.length, ffi_keys_array, ffi_values_array)
        else
          Variant.call_c(:new)
        end
      val
    end

    def type_number
      return TYPE_VOID if @pointer.nil?
      call_c(:type)
    end

    def convert(type_num)
      Variant.from_raw_pointer(call_c(:convert, type_num))
    end

    def to_i
      value.to_i
    end

    def to_f
      value.to_f
    end

    def to_s
      value.to_s
    end

    def to_a
      value.to_a
    end

    def to_hash
      value.to_hash
    end

    def pointer
      @pointer
    end

    def pointer=(ptr)
      raise "Pointer is not a FFI::Pointer" unless ptr.is_a?(FFI::Pointer)
      raise "Null pointer" if ptr.null?
      @pointer = FFI::AutoPointer.new(ptr, CLibrary.method(:qvariant_destroy))
    end

    def call_c(function_name, *arg)
      raise "Uninitialized" if @pointer.nil?
      raise "Null pointer" if @pointer.null?
      CLibrary.send("qvariant_#{function_name}", @pointer, *arg)
    end

  end
end

