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
        expect(@metaobj.class_name).to eq(:TestObject)
      end
    end
    describe '#invoke_method' do
      it 'should invoke method' do
        index = @metaobj.method_count.times.find { |i| @metaobj.method_name(i) == :normalMethod}
        result = @metaobj.invoke_method(@obj, index, 2, '345')
        expect(result).to eq(347)
      end
    end
    describe '#connect_signal' do
      it 'should connect proc to signal' do
        received = nil
        prc = lambda do |str|
          received = str
        end
        index = @metaobj.method_count.times.find { |i| @metaobj.method_name(i) == :someSignal}
        @metaobj.connect_signal(@obj, index, &prc)
        emit_index = @metaobj.method_count.times.find { |i| @metaobj.method_name(i) == :emitSomeSignal}
        @metaobj.invoke_method(@obj, emit_index, 'foo')
        expect(received).to eq('foo')
      end
    end
    describe '#set_property, #get_property' do
      it 'should set property' do
        index = @metaobj.property_count.times.find { |i| @metaobj.property_name(i) == :name}
        @metaobj.set_property(@obj, index, 'foobar')
        expect(@metaobj.get_property(@obj, index)).to eq('foobar')
      end
    end
  end
end