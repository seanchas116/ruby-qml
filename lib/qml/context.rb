require 'qml/plugins'

module QML
  Context = Plugins.core.metaObjects['QQmlContext'].build_class

  class Context

    def self.new
      Plugins.core.createContext(Engine.instance)
    end

    def initialize
      super()
      @extension = Plugins.core.createContextExtension(self)
      @context_properties = {}
    end

    def eval(obj, str)
      @extension.evaluate(obj, str)
    end

    def []=(key, value)
      value.prefer_managed true if value.is_a? Wrapper
      # just hold referenece
      @context_properties[key] = value
      @extension.setContextProperty(key, value)
      value
    end

    def [](key)
      @extension.contextProperty(key)
    end

    def self.for_object(obj)
      Plugins.core.contextForObject(obj).tap do |context|
        context.managed = false if context
      end
    end
  end
end
