require 'qml/qml'
require 'qml/class_builder'

module QML
  class MetaObject
    alias_method :to_s, :name

    def inspect
      "<QML::MetaObject:#{self}>"
    end

    def object_class
      update_class unless @object_class
      @object_class
    end

    def update_class
      cb = ClassBuilder.new(self, @object_class)
      cb.update
      @object_class = cb.klass
    end
  end
end
