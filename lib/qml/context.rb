require 'qml/plugins'
require 'qml/unique_wrapper'

module QML
  class Context < UniqueWrapper

    def self.wrap(qt_context)
      new(nil, qt_context: qt_context)
    end

    def initialize(engine, qt_context: nil)
      qt_context ||= Plugins.core.createContext(engine.qt_engine)
      super(qt_context)
      @extension = Plugins.core.createContextExtension(qt_context)
    end

    def engine
      Engine.from_qt(@extension.engine)
    end

    def qt_context
      @extension.context
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
      qt_context = Plugins.core.contextForObject(obj)
      qt_context && from_qt(qt_context)
    end
  end
end
