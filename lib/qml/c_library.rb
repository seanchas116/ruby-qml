require 'ffi'

module QML
  module CLibrary
    extend FFI::Library
    LIBPATH = Pathname(__FILE__) + '../../../ext/ruby-qml/libruby-qml.1.0.0.dylib'
    ffi_lib LIBPATH.to_s

    attach_function :qvariant_new, [], :pointer
    attach_function :qvariant_from_int, [:int], :pointer
    attach_function :qvariant_from_float, [:double], :pointer
    attach_function :qvariant_from_boolean, [:int], :pointer
    attach_function :qvariant_from_string, [:string], :pointer
    attach_function :qvariant_from_array, [:int, :pointer], :pointer
    attach_function :qvariant_from_hash, [:int, :pointer, :pointer], :pointer

    attach_function :qvariant_to_int, [:pointer], :int
    attach_function :qvariant_to_float, [:pointer], :double

    callback :qvariant_get_string_callback, [:string], :void
    callback :qvariant_get_array_callback, [:pointer], :void
    callback :qvariant_get_hash_callback, [:string, :pointer], :void

    attach_function :qvariant_get_string, [:pointer, :qvariant_get_string_callback], :void
    attach_function :qvariant_get_array, [:pointer, :qvariant_get_array_callback], :void
    attach_function :qvariant_get_hash, [:pointer, :qvariant_get_hash_callback], :void

    attach_function :qvariant_type, [:pointer], :int
    attach_function :qvariant_convert, [:pointer, :int], :pointer

    attach_function :qvariant_dup, [:pointer], :pointer
    attach_function :qvariant_destroy, [:pointer], :void
  end
end