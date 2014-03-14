require 'ffi'

module QML

  class MetaType
    attr_reader :id

    def initialize(id)
      @id = id
    end

    def name
      @name ||= CLib.qmetatype_name(@id)
    end

    def ruby_class
      case id
      when TYPE_BOOL
        [TrueClass, FalseClass]
      when TYPE_INT
        Integer
      when TYPE_DOUBLE
        Float
      when TYPE_Q_STRING
        String
      when TYPE_Q_VARIANT_LIST
        Array
      when TYPE_Q_VARIANT_HASH, TYPE_Q_VARIANT_MAP
        Hash
      when TYPE_Q_DATE_TIME
        Time
      else
        nil
      end
    end
  end

  class Variant

    extend FFI::DataConverter

    TYPE_VOID = 43
    TYPE_BOOL = 1
    TYPE_INT = 2
    TYPE_DOUBLE = 6
    TYPE_Q_STRING = 10
    TYPE_Q_VARIANT = 41
    TYPE_Q_VARIANT_LIST = 9
    TYPE_Q_VARIANT_MAP = 8
    TYPE_Q_VARIANT_HASH = 28
    TYPE_Q_DATE_TIME = 16

    attr_reader :pointer

    def self.new(val)
      case val
      when Variant
        val.dup
      when FFI::Pointer
        super(val)
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
      case meta_type.id
      when TYPE_BOOL
        CLib.qvariant_to_int(self) != 0
      when TYPE_INT
        CLib.qvariant_to_int(self)
      when TYPE_DOUBLE
        CLib.qvariant_to_float(self)
      when TYPE_Q_STRING
        result = nil
        CLib.qvariant_get_string(self, ->(str) { result = str.force_encoding("utf-8") })
        result
      when TYPE_Q_VARIANT
        CLib.qvariant_unnest(self).value
      when TYPE_Q_VARIANT_LIST
        result = []
        CLib.qvariant_get_array(self, ->(variant) { result << variant.value })
        result
      when TYPE_Q_VARIANT_HASH, TYPE_Q_VARIANT_MAP
        result = {}
        CLib.qvariant_get_hash(self, ->(key, variant) { result[key.force_encoding("utf-8").to_sym] = variant.value })
        result
      when TYPE_Q_DATE_TIME
        ffi_buf = FFI::MemoryPointer.new(:int, 8)
        CLib.qvariant_get_time(self, ffi_buf)
        nums = ffi_buf.read_array_of_int(8)
        Time.new(nums[0], nums[1], nums[2], nums[3], nums[4], nums[5] + nums[6] * Rational(1, 1000), nums[7])
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
      variant = Variant.new(variant) unless variant.is_a?(Variant)
      ptr = variant.pointer
      fail TypeError, "Null pointer" if ptr.null?
      ptr
    end

    def self.from_native(ptr, ctx)
      self.new(ptr)
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

