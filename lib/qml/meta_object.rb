require 'qml/qml'

module QML
  class MetaObject
    alias_method :to_s, :name

    def inspect
      "<QML::MetaObject:#{self}>"
    end
  end
end
