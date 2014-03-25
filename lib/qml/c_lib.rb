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

    attach_function :testobject_new, [], :pointer
    attach_function :testobject_static_metaobject, [], :pointer

    attach_function :qt_static_metaobject, [], MetaObject
    attach_function :qobject_static_metaobject, [], MetaObject
    attach_function :qobject_metaobject, [QtObjectBase], MetaObject
    attach_function :qobject_destroy, [:pointer], :void

    callback :qmetaobject_signal_callback, [Variant], :void

    attach_function :qmetaobject_class_name, [MetaObject], Variant
    attach_function :qmetaobject_method_offset, [MetaObject], :int
    attach_function :qmetaobject_method_count, [MetaObject], :int
    attach_function :qmetaobject_method_name, [MetaObject, :int], Variant
    attach_function :qmetaobject_method_parameter_names, [MetaObject, :int], Variant
    attach_function :qmetaobject_method_parameter_types, [MetaObject, :int], Variant
    attach_function :qmetaobject_method_is_signal, [MetaObject, :int], IntAsBoolean
    attach_function :qmetaobject_method_invoke, [MetaObject, QtObjectBase, :int, Variant], Variant
    attach_function :qmetaobject_signal_connect, [MetaObject, QtObjectBase, :int, :qmetaobject_signal_callback], :void
    attach_function :qmetaobject_property_offset, [MetaObject], :int
    attach_function :qmetaobject_property_count, [MetaObject], :int
    attach_function :qmetaobject_property_name, [MetaObject, :int], Variant
    attach_function :qmetaobject_property_get, [MetaObject, QtObjectBase, :int], Variant
    attach_function :qmetaobject_property_set, [MetaObject, QtObjectBase, :int, Variant], :void
    attach_function :qmetaobject_property_notify_signal, [MetaObject, :int], :int
    attach_function :qmetaobject_enum_count, [MetaObject], :int
    attach_function :qmetaobject_enum_offset, [MetaObject], :int
    attach_function :qmetaobject_enum_get, [MetaObject, :int], Variant
    attach_function :qmetaobject_super, [MetaObject], MetaObject

    attach_function :qmetatype_name, [:int], :string

    attach_function :qvariant_new, [], Variant
    attach_function :qvariant_from_int, [:int], Variant
    attach_function :qvariant_from_float, [:double], Variant
    attach_function :qvariant_from_boolean, [IntAsBoolean], Variant
    attach_function :qvariant_from_string, [:string], Variant
    attach_function :qvariant_from_array, [Variant::FromArrayStruct.by_value], Variant
    attach_function :qvariant_from_hash, [Variant::FromHashStruct.by_value], Variant
    attach_function :qvariant_from_time, [:int, :int, :int, :int, :int, :int, :int, :int], Variant
    attach_function :qvariant_from_qobject, [QtObjectBase], Variant
    attach_function :qvariant_from_voidp, [:pointer], Variant

    attach_function :qvariant_to_int, [Variant], :int
    attach_function :qvariant_to_float, [Variant], :double
    attach_function :qvariant_to_qobject, [Variant], QtObjectBase
    attach_function :qvariant_to_voidp, [Variant], :pointer
    attach_function :qvariant_unnest, [Variant], Variant

    callback :qvariant_get_string_callback, [:string], :void
    callback :qvariant_get_array_callback, [Variant], :void
    callback :qvariant_get_hash_callback, [:string, Variant], :void

    attach_function :qvariant_get_string, [Variant, :qvariant_get_string_callback], :void
    attach_function :qvariant_get_array, [Variant, :qvariant_get_array_callback], :void
    attach_function :qvariant_get_hash, [Variant, :qvariant_get_hash_callback], :void
    attach_function :qvariant_get_time, [Variant, :pointer], :void

    attach_function :qvariant_type, [Variant], :int
    attach_function :qvariant_convert, [Variant, :int], Variant
    attach_function :qvariant_is_valid, [Variant], IntAsBoolean

    attach_function :qvariant_dup, [Variant], Variant
    attach_function :qvariant_destroy, [:pointer], :void
  end
end