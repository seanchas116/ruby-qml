require 'ffi'
require 'qml/meta_type'
require 'qml/qt_object_base'

module QML

  class Variant

    extend FFI::DataConverter

    attr_reader :pointer

    def self.new(val, raw: nil)
      return super(raw) if raw
      case val
      when Variant
        val.dup
      when true, false
        CLib.qvariant_from_boolean(val)
      when Integer
        CLib.qvariant_from_int(val)
      when Float
        CLib.qvariant_from_float(val)
      when String
        CLib.qvariant_from_string(val)
      when Symbol
        CLib.qvariant_from_string(val.to_s)
      when Array
        CLib.qvariant_from_array(FromArrayStruct.from_array(val))
      when Hash
        CLib.qvariant_from_hash(FromHashStruct.from_hash(val))
      when Time
        CLib.qvariant_from_time(val.year, val.month, val.day, val.hour, val.min, val.sec, val.nsec / 1_000_000, val.gmt_offset)
      when QtObjectBase
        CLib.qvariant_from_qobject(val)
      when FFI::Pointer
        CLib.qvariant_from_voidp(val)
      else
        fail TypeError, "Cannot initialize QML::Variant with #{val.class.name}"
      end
    end

    def initialize(ptr)
      fail TypeError, "Null pointer" if ptr.null?
      @pointer = FFI::AutoPointer.new(ptr, CLib.method(:qvariant_destroy))
    end

    # TODO: support more types including uint, float
    def value
      case meta_type
      when MetaType::BOOL
        CLib.qvariant_to_int(self) != 0
      when MetaType::INT
        CLib.qvariant_to_int(self)
      when MetaType::DOUBLE
        CLib.qvariant_to_float(self)
      when MetaType::Q_STRING
        result = nil
        CLib.qvariant_get_string(self, ->(str) { result = str.force_encoding("utf-8") })
        result
      when MetaType::Q_VARIANT
        CLib.qvariant_unnest(self).value
      when MetaType::Q_VARIANT_LIST
        result = []
        CLib.qvariant_get_array(self, ->(variant) { result << variant.value })
        result
      when MetaType::Q_VARIANT_HASH, MetaType::Q_VARIANT_MAP
        result = {}
        CLib.qvariant_get_hash(self, ->(key, variant) { result[key.force_encoding("utf-8").to_sym] = variant.value })
        result
      when MetaType::Q_DATE_TIME
        ffi_buf = FFI::MemoryPointer.new(:int, 8)
        CLib.qvariant_get_time(self, ffi_buf)
        nums = ffi_buf.read_array_of_int(8)
        Time.new(nums[0], nums[1], nums[2], nums[3], nums[4], nums[5] + nums[6] * Rational(1, 1000), nums[7])
      when MetaType::Q_OBJECT_STAR
        obj = CLib.qvariant_to_qobject(self)
        obj.meta_object.ruby_class.new(obj.pointer)
      when MetaType::VOID_STAR
        CLib.qvariant_to_voidp(self)
      else
        nil
      end
    end

    def meta_type
      MetaType.new(CLib.qvariant_type(self))
    end

    def convert(metatype)
      CLib.qvariant_convert(self, metatype.id)
    end

    def valid?
      CLib.qvariant_is_valid(self)
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

    def to_sym
      value.to_sym
    end

    native_type FFI::Type::POINTER

    def self.to_native(variant, ctx)
      variant = self.new(variant) unless variant.is_a?(Variant)
      ptr = variant.pointer
      fail TypeError, "Null pointer" if ptr.null?
      ptr
    end

    def self.from_native(ptr, ctx)
      self.new(nil, raw: ptr)
    end

    class FromArrayStruct < FFI::Struct
      layout :count, :int, :variants, :pointer
      def self.from_array(array)
        struct = self.new
        struct[:count] = array.length
        values = array.map { |value| Variant.new(value).pointer }
        struct[:variants] = FFI::MemoryPointer.new(:pointer, array.length).write_array_of_pointer(values)
        struct
      end
    end

    class FromHashStruct < FFI::Struct
      layout :count, :int, :keys, :pointer, :variants, :pointer
      def self.from_hash(hash)
        struct = self.new
        struct[:count] = hash.length
        keys = hash.each_key.map { |k| FFI::MemoryPointer::from_string(k.to_s) }
        struct[:keys] = FFI::MemoryPointer.new(:pointer, hash.length).write_array_of_pointer(keys)
        values = hash.each_value.map { |v| Variant.new(v).pointer }
        struct[:variants] = FFI::MemoryPointer.new(:pointer, hash.length).write_array_of_pointer(values)
        struct
      end
    end
  end
end

