require 'qml/plugins'
require 'qml/wrapper'
require 'qml/context'

module QML
  class Engine
    include Wrapper

    def initialize(qt_engine: nil)
      wrapper_init(qt_engine || Plugins.core.createEngine, Plugins.core.method(:createEngineWrapper))
    end

    def qt_engine
      wrapper.engine
    end

    def context
      Context.new(nil, qt_context: wrapper.rootContext)
    end
  end
end
