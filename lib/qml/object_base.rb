
module QML
  class ObjectBase

    # @api private
    def initialize(objptr = nil)
      fail 'ObjectBase#initialize is internal use only' unless objptr.is_a? ObjectPointer
      @objptr = objptr
    end
  end
end
