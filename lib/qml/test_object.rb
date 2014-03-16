require 'qml/c_lib'
require 'qml/meta_object'

module QML
  class TestObject < MetaObject.new(CLib.testobject_static_metaobject).ruby_class
    def initialize
      super(CLib.testobject_new)
    end
  end
end