require 'qml/qml'
require 'qml/qt_object_base'

module QML
  class MetaObject
    alias_method :to_s, :name

    def inspect
      "<QML::MetaObject:#{self}>"
    end

    def build_class
      @@classes ||= {}
      klass = @@classes[name]
      builder = QtObjectBase::SubclassBuilder.new(self, klass)
      builder.build
      @@classes[name] = builder.subclass
    end
  end
end
