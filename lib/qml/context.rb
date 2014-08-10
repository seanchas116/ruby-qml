require 'qml/qt_classes'

module QML
  # @!parse class Context < QtObjectBase; end

  # {Context} represents QML contexts and used to expose Ruby values to QML.
  # Each context has values that is called "context properties" that can be accessed in QML by their names.
  #
  # This class is automatically created from QQmlContext (C++).
  # @see http://qt-project.org/doc/qt-5/qqmlcontext.html QQmlContext(C++)
  #
  # @example
  #   QML.run do |app|
  #     app.context[:foo] = 'foo'
  #     app.context[:bar] = 'bar'
  #     ...
  #   end
  class Context

    # Creates a new instance of {Context}.
    def self.new
      Plugins.core.createContext(Engine.instance)
    end

    def initialize
      super()
      @extension = Plugins.core.createContextExtension(self)
      @context_properties = {}
    end

    # Evaluates an JavaScript expression on the object in this context.
    # @param obj The object the expression is evaluated on
    # @param str The JavaScript expression string
    # @see QtObjectBase#qml_eval
    def eval(obj, str)
      @extension.evaluate(obj, str)
    end

    # Sets a context property.
    # @param key [String|Symbol] The property key
    # @param value The value
    # @return The value
    def []=(key, value)
      # be sure that the value is managed when it is a QObject
      value = value.create_wrapper if value.is_a? Wrappable
      value.prefer_managed true if value.is_a? QtObjectBase

      # hold referenece because QQmlContext::setContextProperty does not take ownership of objects
      @context_properties[key] = value

      @extension.setContextProperty(key, value)
      value
    end

    # Gets a context property.
    # @param key [String|Symbol] The property key
    # @return The value
    def [](key)
      @extension.contextProperty(key)
    end

    # Gets a context that an object belongs to. If the object belongs to no context, returns nil.
    # @param obj
    # @return [Context|nil]
    def self.for_object(obj)
      Plugins.core.contextForObject(obj).tap do |context|
        context.managed = false if context
      end
    end
  end
end
