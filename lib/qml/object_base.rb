require 'qml/context'

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

    def custom_data
      @custom_data ||= {}
    end

    def qml_eval(str)
      context = Context.for_object(self)
      fail QMLError, 'belongs to no context' unless context
      begin
        context.eval(self, str)
      rescue CppError => error
        raise QMLError, error.raw_message if error.class_name == 'RubyQml::QmlException'
        raise
      end
    end
  end
end
