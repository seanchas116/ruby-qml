require 'qml/plugins'
require 'qml/wrapper'

module QML
  class Context
    include Wrapper

    def initialize(engine, qt_context: nil)
      wrapper_init(qt_context || Plugins.core.createContext(engine.qt_engine),
                   Plugins.core.method(:createContextWrapper))
    end

    def qt_context
      wrapper.context
    end

    def eval(obj, str)
      wrapper.evaluate(obj, str)
    end

    def []=(key, value)
      wrapper.setContextProperty(key, value)
      value
    end

    def [](key)
      wrapper.contextProperty(key)
    end

    def self.for_object(obj)
      qt_context = Plugins.core.contextForObject(obj)
      qt_context && new(nil, qt_context: qt_context)
    end
  end
end
