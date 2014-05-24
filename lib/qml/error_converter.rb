module QML
  module ErrorConverter

    class << self
      attr_reader :patterns

      def add_pattern(cpp_classname, ruby_class)
        @patterns ||= {}
        @patterns[cpp_classname] = ruby_class
      end
    end

    add_pattern 'RubyQml::QmlException', QMLError
  end
end
