require 'qml/dispatchable'

module QML

  # {QtObjectBase} is the base class for Qt object wrappers.
  #
  # In ruby-qml you can access the followings of Qt objects in Ruby.
  #
  # * Properties as {Reactive::Property}
  # * Signals as {Reactive::Signal}
  # * Q_INVOKABLE methods, slots and QML methods as Ruby methods
  #
  # Properties and signals support is provided by {Reactive::Object}.
  #
  # While their names are camelCase in Qt, ruby-qml aliases them as underscore_case.
  #
  # @example
  #   # QML::Application is actually a wrapper for QApplication
  #   app = QML.application
  #
  #   # set property
  #   app.applicationName = "Test"
  #   app.application_name = "Test" # aliased version
  #
  #   # connect to signal
  #   app.aboutToQuit.connect do # "about_to_quit" is also OK
  #     puts "quitting..."
  #   end
  #
  #   # call slot
  #   app.quit
  class QtObjectBase
    include Dispatchable
    include Reactive::Object

    class << self
      attr_accessor :meta_object
      private :meta_object=
    end

    attr_accessor :pointer
    private :pointer=

    # @api private
    def custom_data
      @custom_data ||= {}
    end

    # Evaluates a JavaScript expression on the QML context of the object.
    # Fails with {QMLError} when the object is not created by QML and does not belong to any context.
    # @param [String] str
    # @return the result.
    # @example
    #   component = QML::Component.new data: <<-EOS
    #     import QtQuick 2.0
    #     QtObject {
    #       property string foo: 'foo'
    #       property string bar: 'bar'
    #     }
    #   EOS
    #   obj = component.create
    #   obj.qml_eval("foo + bar") #=> "foobar"
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

    # @return whether the object is managed by Ruby and QML and garbage collected when no longer used.
    def managed?
      pointer.managed?
    end

    # Sets the management status of the object.
    # @param [Boolean] managed
    # @return [Boolean]
    def managed=(managed)
      pointer.managed = managed
    end

    # Sets the management status of the object in safer way.
    # Objects that are created by QML will remain managed and objects that have parents will remain unmanaged.
    # @param [Boolean] managed
    # @return [Boolean]
    def prefer_managed(managed)
      pointer.prefer_managed managed
    end
  end
end
