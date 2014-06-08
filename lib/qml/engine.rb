require 'qml/plugins'
require 'qml/context'
require 'qml/unique_wrapper'

module QML
  class Engine < UniqueWrapper

    def self.wrap(qt_engine)
      new(qt_engine: qt_engine)
    end

    def initialize(qt_engine: nil)
      qt_engine ||= Plugins.core.createEngine
      super(qt_engine)
      @extension = Plugins.core.createEngineExtension(qt_engine)
    end

    def context
      Context.from_qt(@extension.rootContext)
    end
  end
end
