require 'qml/qml'
require 'qml/class_builder'
require 'qml/class_provider'

module QML
  class MetaObject
    alias_method :to_s, :name

    def inspect
      "<QML::MetaObject:#{self}>"
    end

    def update_class
      ClassProvider.instance.update_class(self)
    end
  end
end
