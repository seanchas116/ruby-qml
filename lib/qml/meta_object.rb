require 'qml/qml'
require 'qml/class_builder'

module QML
  class MetaObject
    alias_method :to_s, :name

    def inspect
      "<QML::MetaObject:#{self}>"
    end

    def build_class
      @@classes ||= {}
      klass = @@classes[name]
      builder = ClassBuilder.new(self, klass)
      builder.build
      @@classes[name] = builder.klass
    end
  end
end
