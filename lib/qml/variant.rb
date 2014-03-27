require 'qml/c_lib'
require 'qml/meta_type'
require 'qml/qt_object_base'

module QML
  class Variant

    class << self

      def new(val, raw: nil)
        return super(raw) if raw
        case val
        when Variant
          val.dup
        when true, false
          CLib.rbqml_variant_from_boolean(val)
        when Integer
          CLib.rbqml_variant_from_int(val)
        when Float
          CLib.rbqml_variant_from_float(val)
        when String
          CLib.rbqml_variant_from_string(val)
        when Symbol
          CLib.rbqml_variant_from_string(val.to_s)
        when Array
          from_array(val)
        when Hash
          from_hash(val)
        when Time
          CLib.rbqml_variant_from_time(val.year, val.month, val.day, val.hour, val.min, val.sec, val.nsec / 1_000_000, val.gmt_offset)
        when QtObjectBase
          CLib.rbqml_variant_from_qobject(val)
        when FFI::Pointer
          CLib.rbqml_variant_from_voidp(val)
        when MetaObject
          CLib::rbqml_variant_from_metaobject(val)
        else
          fail TypeError, "Cannot initialize QML::Variant with #{val.class.name}"
        end
      end

      private

      def from_array(array)
        struct = CLib::VariantFromArrayStruct.new
        struct[:count] = array.length
        values = array.map { |value| Variant.new(value).pointer }
        struct[:variants] = FFI::MemoryPointer.new(:pointer, array.length).write_array_of_pointer(values)
        CLib.rbqml_variant_from_array(struct)
      end

      def from_hash(hash)
        struct = CLib::VariantFromHashStruct.new
        struct[:count] = hash.length
        keys = hash.each_key.map { |k| FFI::MemoryPointer::from_string(k.to_s) }
        struct[:keys] = FFI::MemoryPointer.new(:pointer, hash.length).write_array_of_pointer(keys)
        values = hash.each_value.map { |v| QML::Variant.new(v).pointer }
        struct[:variants] = FFI::MemoryPointer.new(:pointer, hash.length).write_array_of_pointer(values)
        CLib.rbqml_variant_from_hash(struct)
      end
    end

    attr_reader :pointer

    def initialize(ptr)
      fail TypeError, "Null pointer" if ptr.null?
      @pointer = FFI::AutoPointer.new(ptr, CLib.method(:rbqml_variant_destroy))
    end

    # TODO: support more types including uint, float
    def value
      case meta_type
      when MetaType::BOOL
        CLib.rbqml_variant_to_int(self) != 0
      when MetaType::INT
        CLib.rbqml_variant_to_int(self)
      when MetaType::DOUBLE
        CLib.rbqml_variant_to_float(self)
      when MetaType::Q_STRING
        result = nil
        CLib.rbqml_variant_get_string(self, ->(str) { result = str.force_encoding("utf-8") })
        result
      when MetaType::Q_VARIANT
        CLib.rbqml_variant_unnest(self).value
      when MetaType::Q_VARIANT_LIST
        result = []
        CLib.rbqml_variant_get_array(self, ->(variant) { result << variant.value })
        result
      when MetaType::Q_VARIANT_HASH, MetaType::Q_VARIANT_MAP
        result = {}
        CLib.rbqml_variant_get_hash(self, ->(key, variant) { result[key.force_encoding("utf-8").to_sym] = variant.value })
        result
      when MetaType::Q_DATE_TIME
        ffi_buf = FFI::MemoryPointer.new(:int, 8)
        CLib.rbqml_variant_get_time(self, ffi_buf)
        nums = ffi_buf.read_array_of_int(8)
        Time.new(nums[0], nums[1], nums[2], nums[3], nums[4], nums[5] + nums[6] * Rational(1, 1000), nums[7])
      when MetaType::Q_OBJECT_STAR
        obj = CLib.rbqml_variant_to_qobject(self)
        obj.meta_object.ruby_class.new(obj.pointer)
      when MetaType::VOID_STAR
        CLib.rbqml_variant_to_voidp(self)
      when MetaType::CONST_Q_META_OBJECT_STAR
        CLib.rbqml_variant_to_metaobject(self)
      else
        nil
      end
    end

    def meta_type
      MetaType.new(CLib.rbqml_variant_type(self))
    end

    def convert(metatype)
      CLib.rbqml_variant_convert(self, metatype.id)
    end

    def valid?
      CLib.rbqml_variant_is_valid(self)
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

  end
end
