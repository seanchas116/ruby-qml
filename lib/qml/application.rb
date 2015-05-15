module QML
  # {Application} represents a Qt application instance.
  # It provides the event loop and manages Application-level configurations.
  #
  # @see http://qt-project.org/doc/qt-5/qapplication.html QApplication (C++)
  class Application

    # @return [Engine] The engine of the application.
    def engine
      QML.engine
    end

    # @return [Component] The root component of the application that represents the loaded QML file.
    # @see #load
    # @see #load_data
    # @see #load_path
    def root_component
      @root_component or fail ApplicationError, "QML data or file has not been loaded"
    end

    # Loads a QML file. The loaded component can be accessed by {#root_component}
    # @param [Hash] opts
    # @param [String] data
    # @param [String] path
    # @see Component
    def load(data: nil, path: nil)
      @root_component = Component.new(data: data, path: path)
      @root = @root_component.create
    end

    # Loads a QML file from string data.
    # @see #load
    def load_data(data)
      load(data: data)
    end

    # Loads a QML file from a file path.
    # @see #load
    def load_path(path)
      load(path: path)
    end

    # @return The root object created by the root component.
    def root
      @root or fail "QML data or file has not been loaded"
    end

    # Runs garbage collection of Ruby and QML and deletes unused objects.
    def collect_garbage
      ::GC.start
      engine.collect_garbage
      process_events
    end
  end

  # Creates an {Application}, yields it and then call {Application#exec}.
  # @return [Application]
  # @example
  #   QML.run do |app|
  #     app.load_path Pathname(__FILE__) + '../main.qml'
  #   end
  def run
    Application.new.tap do |app|
      yield app
      app.exec
    end
  end

  module_function :run
end
