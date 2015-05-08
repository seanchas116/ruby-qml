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

    # Adds a QML import path to the {Engine}.
    # @param path [String]
    # @see http://doc.qt.io/qt-5/qtqml-syntax-imports.html#qml-import-path
    def add_import_path(path)
      fail 'TODO'
    end

    def new_array(array)
      build_array(array.size).tap do |jsarray|
        array.each_with_index do |x, i|
          jsarray[i] = x
        end
      end
    end

    def new_object(hash)
      build_object.tap do |jsobj|
        hash.each do |key, value|
          jsobj[key] = value
        end
      end
    end
  end
end
