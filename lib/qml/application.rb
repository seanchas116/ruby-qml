require 'qml/qt_classes'

module QML
  # @!parse class Application < QtObjectBase; end

  # {Application} represents a Qt application instance.
  # It provides the event loop and manages Application-level configurations.
  #
  # @see http://qt-project.org/doc/qt-5/qapplication.html QApplication (C++)
  class Application

    # @return [Application] The application instance.
    def self.instance
      Kernel.application
    end

    def self.notify_error(error)
      instance.notify_error(error)
    end

    # @note This method cannot be called because {Application} is singleton. 
    def self.new
      fail ApplicationError, "cannot create Application instance manually"
    end

    # @!method events_processed
    # This signal is emitted every time events are processed in the event loop.
    # @return [Reactive::Signal]
    signal :events_processed, []

    def initialize
      super()
      @extension = Plugins.core.createApplicationExtension(self)
      @extension.events_processed.each do
        events_processed.call
      end
    end

    # @return [Engine] The engine of the application.
    def engine
      Kernel.engine
    end

    # @return [Context] The root context of the engine.
    # @see #engine
    def context
      engine.context
    end

    # @return [Component] The root component of the application that represents the loaded QML file.
    # @see #load
    # @see #load_data
    # @see #load_path
    def root_component
      @root_component or fail ApplicationError, "QML data or file has not been loaded"
    end

    # Loads a QML file. The loaded component can be accessed by {#root_component}
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

    # Starts the event loop of the application.
    # This method never returns until the application quits.
    def exec
      @extension.exec
    end

    # Processes queued events in the event loop manually.
    # This method is useful when you are combining an external event loop like EventMachine.
    def process_events
      @extension.process_events
    end

    def force_deferred_deletes
      @extension.force_deferred_deletes
    end

    # Runs garbage collection of Ruby and QML and deletes unused objects.
    def collect_garbage
      ::GC.start
      engine.collect_garbage
      force_deferred_deletes
    end

    # Called when an Ruby error is occured in executing Qt code.
    # @param error The error (or the exception)
    def notify_error(error)
      warn "-- An error occured when running Ruby code from Qt --"
      warn "#{error.class.name}: #{error.message}"
      warn "Backtrace: \n\t#{error.backtrace.join("\n\t")}"
    end
  end

  # @overload application
  #   Returns the instance of {Application}.
  #   @return [Application]
  # @overload application
  #   Call {init} if ruby-qml is not initialized, then yields the application instance and then call {Application#exec}.
  #   @return [Application]
  #   @example
  #     QML.application do |app|
  #       app.context[:foo] = 'foo'
  #       app.load_path Pathname(__FILE__) + '../main.qml'
  #     end
  # @see Application.instance
  def application
    if block_given?
      QML.init unless QML.initialized?
      Application.instance.tap do |app|
        yield app
        app.exec
      end
    else
      Application.instance
    end
  end

  module_function :application
end
