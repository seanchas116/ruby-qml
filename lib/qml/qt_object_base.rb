require 'qml/c_lib'
module QML
  class QtObjectBase

    attr_reader :pointer

    def initialize(ptr, destroy: true)
      fail TypeError, 'ptr must be a FFI::Pointer' unless ptr.is_a? FFI::Pointer
      @pointer = destroy ? FFI::AutoPointer.new(ptr, CLib.method(:qobject_destroy)) : ptr
      @gc_protected = []
    end

    def gc_protect(obj)
      @gc_protected << obj
    end

  end
end