module QML
  # @!parse class Engine < QtObjectBase; end

  # {Engine} provides a QML engine.
  #
  # @see http://qt-project.org/doc/qt-5/qqmlengine.html QQmlEngine (C++)
  class Engine

    # Starts garbage collection on the {Engine}.
    def collect_garbage
      fail 'TODO'
    end

    # Adds an {ImageProvider} to the {Engine}.
    # @param id [String]
    # @param provider [ImageProvider]
    # @return [ImageProvider]
    # @see ImageProvider
    def add_image_provider(id, provider)
      fail 'TODO'
    end

    # Adds a QML import path to the {Engine}.
    # @param path [String]
    # @see http://doc.qt.io/qt-5/qtqml-syntax-imports.html#qml-import-path
    def add_import_path(path)
      fail 'TODO'
    end
  end
end
