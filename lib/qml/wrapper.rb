module QML
  class Wrapper
    include Reactive::Object

    class << self
      attr_accessor :meta_object
      private :meta_object=

      # Transfers the ownership of the actual Qt object to a newly created instance of the class.
      # This method is used to use subclasses of classes that are auto-generated from Qt meta object.
      # @param obj [Wrapper]
      # @return [Wrapper]
      def hijack(obj)
        return obj if obj.is_a? self
        fail QtObjectError, "#{self} cannot hijack an instance of #{obj.class}" unless self < obj.class
        allocate.tap do |newobj|
          newobj.instance_eval do
            self.pointer = obj.pointer
            self.pointer.associate(self)
            initialize
          end
        end
      end
    end

    attr_accessor :pointer
    private :pointer=

    def custom_data
      @custom_data ||= {}
    end

    def qml_eval(str)
      context = Context.for_object(self)
      fail QMLError, 'belongs to no context' unless context
      context.eval(self, str)
    end

    def inspect
      klass = self.class
      property_inspect = klass.instance_properties.sort
        .reject { |name| klass.instance_property(name).alias? }
        .map do |name|
          "#{name}=" +
            begin
              property(name).value.inspect
            rescue ConversionError
              "<unsupported type>"
            end
        end
        .join(' ')
      classname = klass.name || "[class for #{klass.meta_object.name}]"
      "#<#{classname}:#{__id__} #{property_inspect}>"
    end

    alias_method :to_s, :inspect

    def managed?
      pointer.managed?
    end

    def managed=(managed)
      pointer.managed = managed
    end

    def prefer_managed(managed)
      pointer.prefer_managed managed
    end
  end
end
