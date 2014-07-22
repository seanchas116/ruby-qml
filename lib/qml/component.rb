require 'qml/engine'

module QML
  # @!parse class Component < QtObjectBase; end

  # The Component class is used to instantiate objects like Window / ApplicationWindow objects from QML files.
  #
  # You usually do not need to use this class because Application#load, #load_path, #load_data do same
  # for the application top-level objects such as main windows.
  # @example
  #   component = Component.new(path: path_to_qml_file)
  #   root_object = component.create
  # @see http://qt-project.org/doc/qt-5/qqmlcomponent.html QQmlComponent (C++)
  class Component
    attr_reader :data, :path, :context

    # Creates an component. Either data or path must be specified.
    # @param opts [Hash]
    # @option opts [QML::Context] :context the context that the created objects will depend on (default to the root context of the application engine).
    # @option opts [String] :data the QML file data.
    # @option opts [#to_s] :path the QML file path.
    # @return QML::Component
    def self.new(opts)
      context = opts[:context]
      data = opts[:data]
      path = opts[:path]
      context ||= Engine.instance.context
      Plugins.core.createComponent(Engine.instance).instance_eval do
        @data = data
        @path = path
        @context = context

        if data
          @extension.loadString(data, (path && path.to_s) || '')
        elsif path
          @extension.loadFile(path.to_s)
        else
          fail QMLError, 'neither data nor path specified'
        end

        self
      end
    end

    def initialize
      super()
      Access.register_classes
      @extension = Plugins.core.createComponentExtension(self)
    end

    # Instantiates a object from the QML file.
    # @return [QtObjectBase] The created Qt object
    def create
      @extension.create(@context)
    end
  end
end
