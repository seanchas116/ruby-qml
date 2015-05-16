require 'qml/engine'

module QML
  # The Component class is used to instantiate objects like Window / ApplicationWindow objects from QML files.
  #
  # You usually do not need to use this class because Application#load, #load_path, #load_data do same
  # for the application top-level objects such as main windows.
  # @example
  #   component = Component.new(engine: engine, path: path_to_qml_file)
  #   root_object = component.create
  # @see http://qt-project.org/doc/qt-5/qqmlcomponent.html QQmlComponent (C++)
  class Component
    attr_reader :data, :path

    # Creates an component. Either data or path must be specified.
    # @param [String] data the QML file data.
    # @param [#to_s] path the QML file path.
    # @return QML::Component
    def initialize(data: nil, path: nil)
      fail TypeError, "neither data nor path privided" unless data || path

      initialize_impl

      case
      when data
        load_data(data)
      when path
        load_path(path)
      end
    end

    def load_path(path)
      path = path.to_s
      check_error_string do
        @path = Pathname.new(path)
        load_path_impl(path)
      end
      self
    end

    def load_data(data)
      check_error_string do
        @data = data
        load_data_impl(data, "<<STRING>>")
      end
      self
    end

    # Instantiates a object from the QML file.
    # @return [QtObjectBase] The created Qt object
    def create
      check_error_string do
        create_impl
      end
    end

    def check_error_string
      yield.tap do
        fail QMLError, error_string if error_string
      end
    end
  end
end
