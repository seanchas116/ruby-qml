require 'spec_helper'
require 'qml/meta_object'

describe QML::MetaObject do

  context 'with TestObject' do

    before do
      @metaobj = QML::MetaObject.new(QML::CLib.testobject_static_metaobject)
      @obj = QML::QtObjectBase.new(QML::CLib.testobject_new)
    end
    describe '#class_name' do
      it 'should return class name' do
        expect(@metaobj.name).to eq(:TestObject)
      end
    end
    describe '#invoke_method' do
      it 'should invoke method' do
        method = @metaobj.meta_methods.find { |m| m.name == :normalMethod }
        result = method.invoke(@obj, 2, '345')
        expect(result).to eq(347)
      end
    end
    describe '#connect_signal' do
      it 'should connect proc to signal' do
        received = nil
        prc = ->(str) { received = str }

        signal = @metaobj.meta_methods.find { |m| m.name == :someSignal }
        signal.connect_signal(@obj, &prc)

        @metaobj.meta_methods.find { |m| m.name == :emitSomeSignal }.invoke(@obj, 'foo')
        expect(received).to eq('foo')
      end
    end
    describe '#set_property, #get_property' do
      it 'should set property' do
        property = @metaobj.meta_properties.find { |p| p.name == :name }
        property.set_value(@obj, 'foobar')
        expect(property.get_value(@obj)).to eq('foobar')
      end
    end
  end
end