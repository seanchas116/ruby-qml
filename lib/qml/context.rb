require 'qml/plugins'

module QML
  Context = Plugins.core.metaObjects['QQmlContext'].build_class

  class Context

    def self.new(engine)
      Plugins.core.createContext(engine)
    end

    def initialize
      super()
      @extension = Plugins.core.createContextExtension(self)
    end

    def engine
      @extension.engine
    end

    def eval(obj, str)
      @extension.evaluate(obj, str)
    end

    def []=(key, value)
      @extension.setContextProperty(key, value)
      value
    end

    def [](key)
      @extension.contextProperty(key)
    end

    def self.for_object(obj)
      Plugins.core.contextForObject(obj).tap do |context|
        context.object_pointer.owned = false if context
      end
    end
  end
end
