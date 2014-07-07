require 'qml/qt_classes'

module QML

  # {Application} represents a QML application instance.
  #
  # This class is automatically created from Qt class QApplication.
  # Signals, slots and properties of QApplication (like #quit or #application_name) can also be used in {Application}.
  # (See {http://qt-project.org/doc/qt-5/qapplication.html Qt C++ documentation} for details)
  class Application

    # @return [Application] The application instance.
    def self.instance
      Kernel.application
    end

    def self.notify_error(error)
      instance.notify_error(error)
    end

    # This method cannot be called because {Application} is singleton. 
    # @api private
    def self.new
      fail ApplicationError, "cannot create Application instance manually"
    end

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

  # Returns the instance of {Application}.
  # If a block is given, yields the application instance and then call {Application#exec}.
  # @example
  #   QML.application do |app|
  #     app.context[:foo] = 'foo'
  #     app.load_path Pathname(__FILE__) + '../main.qml'
  #   end
  # @see Application.instance
  def application
    Application.instance.tap do |app|
      if block_given?
        yield app
        app.exec
      end
    end
  end

  module_function :application
end
