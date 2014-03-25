require 'ffi'
require 'qml/variant'
require 'qml/meta_object'
require 'qml/qt_object_base'

module QML
  module CLib

    extend FFI::Library

    LIBPATH = %w{so dylib dll}
        .map { |suffix| Pathname(__FILE__) + "../../../ext/ruby-qml/libruby-qml.1.0.0.#{suffix}"  }
        .find { |path| path.exist? }
    ffi_lib LIBPATH.to_s

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

    attach_function :rbqml_testobject_new, [], :pointer
    attach_function :rbqml_testobject_static_metaobject, [], :pointer

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

    attach_function :rbqml_variant_new, [], Variant
    attach_function :rbqml_variant_from_int, [:int], Variant
    attach_function :rbqml_variant_from_float, [:double], Variant
    attach_function :rbqml_variant_from_boolean, [IntAsBoolean], Variant
    attach_function :rbqml_variant_from_string, [:string], Variant
    attach_function :rbqml_variant_from_array, [Variant::FromArrayStruct.by_value], Variant
    attach_function :rbqml_variant_from_hash, [Variant::FromHashStruct.by_value], Variant
    attach_function :rbqml_variant_from_time, [:int, :int, :int, :int, :int, :int, :int, :int], Variant
    attach_function :rbqml_variant_from_qobject, [QtObjectBase], Variant
    attach_function :rbqml_variant_from_voidp, [:pointer], Variant

    attach_function :rbqml_variant_to_int, [Variant], :int
    attach_function :rbqml_variant_to_float, [Variant], :double
    attach_function :rbqml_variant_to_qobject, [Variant], QtObjectBase
    attach_function :rbqml_variant_to_voidp, [Variant], :pointer
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
  end
end