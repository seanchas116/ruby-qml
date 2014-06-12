require 'qml/plugins'

module QML
  Engine = Plugins.core.metaObjects['QQmlEngine'].build_class

  class Engine

    def self.new
      Plugins.core.createEngine
    end

    def initialize
      super()
      @extension = Plugins.core.createEngineExtension(self)
    end

    def context
      @extension.rootContext
    end
  end
end
