module QML

  class MethodError < StandardError; end
  class SignalError < StandardError; end
  class PropertyError < StandardError; end
  class PluginError < StandardError; end
  class NullObjectError < StandardError; end
  class ConversionError < StandardError; end
  class QMLError < StandardError; end
  class AccessError < StandardError; end

  class CppError < StandardError
    attr_reader :class_name, :raw_message

    def initialize(classname, message)
      super("<#{classname}> #{message}")
      @class_name = classname
      @raw_message = message
    end
  end
end
