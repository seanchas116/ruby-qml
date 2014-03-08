require 'ffi'
require 'qml/variant'
require 'qml/meta_object'

module QML
  module CLib

    extend FFI::Library
    LIBPATH = Pathname(__FILE__) + '../../../ext/ruby-qml/libruby-qml.1.0.0.dylib'
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

    attach_function :qt_static_metaobject, [], :pointer
    attach_function :qobject_static_metaobject, [], :pointer
    attach_function :qobject_metaobject, [:pointer], :pointer
    attach_function :qobject_destroy, [:pointer], :void

    callback :qmetaobject_signal_callback, [Variant], :void

    attach_function :qmetaobject_class_name, [MetaObject], Variant
    attach_function :qmetaobject_method_count, [MetaObject], :int
    attach_function :qmetaobject_method_name, [MetaObject, :int], Variant
    attach_function :qmetaobject_method_parameter_names, [MetaObject, :int], Variant
    attach_function :qmetaobject_method_parameter_types, [MetaObject, :int], Variant
    attach_function :qmetaobject_method_is_signal, [MetaObject, :int], IntAsBoolean
    attach_function :qmetaobject_method_invoke, [MetaObject, :pointer, :int, Variant], Variant
    attach_function :qmetaobject_signal_connect, [MetaObject, :pointer, :int, :qmetaobject_signal_callback], :void
    attach_function :qmetaobject_property_count, [MetaObject], :int
    attach_function :qmetaobject_property_name, [MetaObject, :int], Variant
    attach_function :qmetaobject_property_get, [MetaObject, :pointer, :int], Variant
    attach_function :qmetaobject_property_set, [MetaObject, :pointer, :int, Variant], :void
    attach_function :qmetaobject_property_notify_signal, [MetaObject, :int], :int
    attach_function :qmetaobject_enum_count, [MetaObject], :int
    attach_function :qmetaobject_enum_get, [MetaObject, :int], Variant

    attach_function :qvariant_type_name, [:int], :string

    attach_function :qvariant_new, [], Variant
    attach_function :qvariant_from_int, [:int], Variant
    attach_function :qvariant_from_float, [:double], Variant
    attach_function :qvariant_from_boolean, [IntAsBoolean], Variant
    attach_function :qvariant_from_string, [:string], Variant
    attach_function :qvariant_from_array, [Variant::FromArrayStruct.by_value], Variant
    attach_function :qvariant_from_hash, [Variant::FromHashStruct.by_value], Variant
    attach_function :qvariant_from_time, [:int, :int, :int, :int, :int, :int, :int, :int], Variant

    attach_function :qvariant_to_int, [Variant], :int
    attach_function :qvariant_to_float, [Variant], :double
    attach_function :qvariant_to_qvariant, [Variant], Variant

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