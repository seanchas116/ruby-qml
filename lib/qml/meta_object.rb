require 'qml/qml'
require 'qml/class_builder'

module QML
  class MetaObject
    alias_method :to_s, :name

    def inspect
      "<QML::MetaObject:#{self}>"
    end

    def object_class
      @object_class ||= ClassBuilder.new(self).create_class
    end
  end
end
