require 'qml/c_lib'

module QML
  class QtObjectBase

    attr_reader :pointer

    def initialize(ptr, destroy: true)
      ptr = ptr.pointer if ptr.is_a? QtObjectBase
      fail TypeError, 'ptr must be a FFI::Pointer' unless ptr.is_a? FFI::Pointer
      @pointer = ptr
      if destroy
        ObjectSpace.define_finalizer(self, self.class.method(:disposed))
      end
      @gc_protected = []
    end

    def meta_object
      CLib.rbqml_object_metaobject(self)
    end

    def gc_protect(obj)
      @gc_protected << obj
    end

    def self.created(obj)
      addr = obj.pointer.to_i
      @refcounts ||= {}
      @refcounts[addr] ||= 0
      @refcounts[addr] += 1
    end

    def self.disposed(obj)
      addr = obj.pointer.to_i
      @refcounts[addr] -= 1
      if @refcounts[addr] == 0
        CLib.rbqml_qobject_destroy(obj.pointer)
        @refcounts.delete(addr)
      end
    end

  end
end