require 'qml/plugins'

module QML
  Engine = Kernel.engine_meta_object.build_class

  class Engine

    def self.instance
      Kernel.engine
    end

    def self.new
      fail ApplicationError, "cannot create Application instance manually"
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

  def engine
    Kernel.engine
  end
  module_function :engine
end
