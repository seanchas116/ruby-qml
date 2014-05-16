module QML
  class ObjectBase
    include Ropework::PropertyDef
    include Ropework::SignalDef

    class << self
      attr_accessor :meta_object
      private :meta_object=
    end

    # @api private
    def initialize(objptr = nil)
      fail 'ObjectBase#initialize is internal use only' unless objptr.is_a? ObjectPointer
      @objptr = objptr
      super()
    end
  end
end
