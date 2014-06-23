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

    # @param id [String]
    # @param provider [QML::ImageProvider]
    def add_image_provider(id, provider)
      @extension.add_image_provider(id, provider.qt_image_provider)
    end
  end
end
