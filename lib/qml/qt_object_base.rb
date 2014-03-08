require 'qml/c_lib'
module QML
  class QtObjectBase

    def initialize(ptr)
      self.pointer = ptr
      @_gc_protected = []
    end

    def pointer
      @_pointer
    end

    def pointer=(ptr)
      @_pointer = FFI::AutoPointer.new(ptr, CLib.method(:qobject_destroy))
    end

    def gc_protect(obj)
      @_gc_protected << obj
    end

  end
end