require 'ropework'

module QML
  class QtObjectBase
    include Ropework::Object

    class << self
      attr_accessor :meta_object
      private :meta_object=
    end

    attr_accessor :object_pointer
    private :object_pointer=

    def custom_data
      @custom_data ||= {}
    end

    def qml_eval(str)
      context = Context.for_object(self)
      fail QMLError, 'belongs to no context' unless context
      context.eval(self, str)
    end

    def inspect
      property_inspect = properties.map { |name| "#{name}=#{property(name).value.inspect}" }.join(' ')
      "#<QML::ObjectBase[#{self.class.meta_object.name}]:#{object_id} #{property_inspect}>"
    end

    alias_method :to_s, :inspect
  end
end
