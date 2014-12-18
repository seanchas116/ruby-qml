require 'qml/qt_classes'

module QML
  # @!parse class Engine < QtObjectBase; end

  # {Engine} provides a QML engine.
  # 
  # @see http://qt-project.org/doc/qt-5/qqmlengine.html QQmlEngine (C++)
  class Engine

    # @return [Engine] the instance of {Engine}.
    # @see QML.engine
    def self.instance
      Kernel.engine
    end

    # @note This method cannot be called because {Engine} is singleton. 
    def self.new
      fail EngineError, "cannot create Engine instance manually"
    end

    def initialize
      super()
      @extension = Plugins.core.createEngineExtension(self)
    end

    # @return [Context] the root {Context} the {Engine}.
    def context
      @extension.rootContext
    end

    # Starts garbage collection on the {Engine}.
    def collect_garbage
      @extension.collect_garbage
    end

    # Adds an {ImageProvider} to the {Engine}.
    # @param id [String]
    # @param provider [ImageProvider]
    # @return [ImageProvider]
    # @see ImageProvider
    def add_image_provider(id, provider)
      @extension.add_image_provider(id, provider.qt_image_provider)
      provider
    end

    # Adds a QML import path to the {Engine}.
    # @param path [String]
    # @see http://doc.qt.io/qt-5/qtqml-syntax-imports.html#qml-import-path
    def add_import_path(path)
      @extension.add_import_path(path)
    end

    # Adds a QML import path to the {Engine}.
    # @param path [String]
    # @see http://doc.qt.io/qt-5/qtqml-syntax-imports.html#qml-import-path
    def import_paths()
      @extension.import_paths()
    end

  end

  # @return [Engine] the instance of {Engine}.
  # @see Engine.instance
  def engine
    Kernel.engine
  end
  module_function :engine
end
