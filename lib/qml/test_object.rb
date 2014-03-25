require 'qml/c_lib'
require 'qml/meta_object'

module QML
  class TestObject < MetaObject.new(CLib.rbqml_testobject_static_metaobject).ruby_class
    def initialize
      super(CLib.rbqml_testobject_new)
    end
  end
end