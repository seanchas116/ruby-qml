require 'ffi'

module QML
  module CLib

    module IntAsBoolean
      extend FFI::DataConverter
      native_type FFI::Type::INT
      def self.to_native(bool, ctx)
        bool ? 1 : 0
      end
      def self.from_native(int, ctx)
        int != 0
      end
    end

    module MetaObject
      extend FFI::DataConverter
      native_type FFI::Type::POINTER

      def self.to_native(metaobj, ctx)
        ptr = metaobj.pointer
        fail TypeError, "Null pointer" if ptr.null?
        ptr
      end

      def self.from_native(ptr, ctx)
        return nil if ptr.null?
        @metaobjects ||= {}
        @metaobjects[ptr.to_i] ||= QML::MetaObject.new(ptr)
      end
    end

    module QtObjectBase
      extend FFI::DataConverter
      native_type FFI::Type::POINTER

      def self.to_native(obj, ctx)
        obj.pointer
      end

      def self.from_native(ptr, ctx)
        ptr.null? ? nil : QML::QtObjectBase.new(ptr)
      end
    end

    module Variant
      extend FFI::DataConverter
      native_type FFI::Type::POINTER

      def self.to_native(variant, ctx)
        variant = QML::Variant.new(variant) unless variant.is_a?(Variant)
        ptr = variant.pointer
        fail TypeError, "Null pointer" if ptr.null?
        ptr
      end

      def self.from_native(ptr, ctx)
        QML::Variant.new(nil, raw: ptr)
      end
    end

    extend FFI::Library

    LIBPATH = %w{so dylib dll}
        .map { |suffix| Pathname(__FILE__) + "../../../ext/ruby-qml/libruby-qml.1.0.0.#{suffix}"  }
        .find { |path| path.exist? }
    ffi_lib LIBPATH.to_s

    attach_function :rbqml_init, [], :void

    attach_function :rbqml_testobject_new, [], QtObjectBase
    attach_function :rbqml_testobject_static_metaobject, [], MetaObject

    attach_function :rbqml_backendmain_instance, [], QtObjectBase
    attach_function :rbqml_backendmain_static_metaobject, [], MetaObject

    attach_function :rbqml_qt_metaobject, [], MetaObject
    attach_function :rbqml_object_static_metaobject, [], MetaObject
    attach_function :rbqml_object_metaobject, [QtObjectBase], MetaObject
    attach_function :rbqml_object_destroy, [:pointer], :void

    callback :metaobject_signal_callback, [Variant], :void

    attach_function :rbqml_metaobject_class_name, [MetaObject], Variant
    attach_function :rbqml_metaobject_method_offset, [MetaObject], :int
    attach_function :rbqml_metaobject_method_count, [MetaObject], :int
    attach_function :rbqml_metaobject_method_name, [MetaObject, :int], Variant
    attach_function :rbqml_metaobject_method_parameter_names, [MetaObject, :int], Variant
    attach_function :rbqml_metaobject_method_parameter_types, [MetaObject, :int], Variant
    attach_function :rbqml_metaobject_method_is_signal, [MetaObject, :int], IntAsBoolean
    attach_function :rbqml_metaobject_method_invoke, [MetaObject, QtObjectBase, :int, Variant], Variant
    attach_function :rbqml_metaobject_signal_connect, [MetaObject, QtObjectBase, :int, :metaobject_signal_callback], :void
    attach_function :rbqml_metaobject_property_offset, [MetaObject], :int
    attach_function :rbqml_metaobject_property_count, [MetaObject], :int
    attach_function :rbqml_metaobject_property_name, [MetaObject, :int], Variant
    attach_function :rbqml_metaobject_property_get, [MetaObject, QtObjectBase, :int], Variant
    attach_function :rbqml_metaobject_property_set, [MetaObject, QtObjectBase, :int, Variant], :void
    attach_function :rbqml_metaobject_property_notify_signal, [MetaObject, :int], :int
    attach_function :rbqml_metaobject_enum_count, [MetaObject], :int
    attach_function :rbqml_metaobject_enum_offset, [MetaObject], :int
    attach_function :rbqml_metaobject_enum_get, [MetaObject, :int], Variant
    attach_function :rbqml_metaobject_super, [MetaObject], MetaObject

    attach_function :rbqml_metatype_name, [:int], :string
    attach_function :rbqml_metatype_from_name, [:string], :int

    class VariantFromArrayStruct < FFI::Struct
      layout :count, :int, :variants, :pointer
    end
    class VariantFromHashStruct < FFI::Struct
      layout :count, :int, :keys, :pointer, :variants, :pointer
    end

    attach_function :rbqml_variant_new, [], Variant
    attach_function :rbqml_variant_from_int, [:int], Variant
    attach_function :rbqml_variant_from_float, [:double], Variant
    attach_function :rbqml_variant_from_boolean, [IntAsBoolean], Variant
    attach_function :rbqml_variant_from_string, [:string], Variant
    attach_function :rbqml_variant_from_array, [VariantFromArrayStruct.by_value], Variant
    attach_function :rbqml_variant_from_hash, [VariantFromHashStruct.by_value], Variant
    attach_function :rbqml_variant_from_time, [:int, :int, :int, :int, :int, :int, :int, :int], Variant
    attach_function :rbqml_variant_from_qobject, [QtObjectBase], Variant
    attach_function :rbqml_variant_from_voidp, [:pointer], Variant
    attach_function :rbqml_variant_from_metaobject, [MetaObject], Variant

    attach_function :rbqml_variant_to_int, [Variant], :int
    attach_function :rbqml_variant_to_float, [Variant], :double
    attach_function :rbqml_variant_to_qobject, [Variant], QtObjectBase
    attach_function :rbqml_variant_to_voidp, [Variant], :pointer
    attach_function :rbqml_variant_to_metaobject, [Variant], MetaObject
    attach_function :rbqml_variant_unnest, [Variant], Variant

    callback :variant_get_string_callback, [:string], :void
    callback :variant_get_array_callback, [Variant], :void
    callback :variant_get_hash_callback, [:string, Variant], :void

    attach_function :rbqml_variant_get_string, [Variant, :variant_get_string_callback], :void
    attach_function :rbqml_variant_get_array, [Variant, :variant_get_array_callback], :void
    attach_function :rbqml_variant_get_hash, [Variant, :variant_get_hash_callback], :void
    attach_function :rbqml_variant_get_time, [Variant, :pointer], :void

    attach_function :rbqml_variant_type, [Variant], :int
    attach_function :rbqml_variant_convert, [Variant, :int], Variant
    attach_function :rbqml_variant_is_valid, [Variant], IntAsBoolean

    attach_function :rbqml_variant_dup, [Variant], Variant
    attach_function :rbqml_variant_destroy, [:pointer], :void

    rbqml_init
  end
end
