require 'spec_helper'
require 'qml/meta_object'

describe QML::MetaObject do

  context 'with TestObject' do

    before do
      @metaobj = QML::MetaObject.new(QML::CLib.testobject_static_metaobject)
      @obj = QML::QtObjectBase.new(QML::CLib.testobject_new)
    end

    describe '#name' do
      it 'should return class name' do
        expect(@metaobj.name).to eq(:TestObject)
      end
    end

    describe '#meta_methods' do
      it 'returns the meta methods' do
        meta_methods = @metaobj.meta_methods
        expect(meta_methods.length).to eq(9)
      end
    end

    describe '#meta_properties' do
      it 'returns the meta properties' do
        meta_properties = @metaobj.meta_properties
        expect(meta_properties.length).to eq(1)
      end
    end

    describe '#enums' do
      it 'returns the enums' do
        enums = @metaobj.enums
        expected = [
          {
            Apple: 0, Banana: 1, Orange: 2
          }
        ]
        expect(enums).to eq(expected)
      end
    end

    describe '#super_meta_object' do
      it 'returns the super meta object' do
        expect(@metaobj.super_meta_object.name).to eq(:QObject)
      end
    end

    describe '#ancestors' do
      it 'returns the ancestor meta objects' do
        expect(@metaobj.ancestors.map(&:name)).to eq([:TestObject, :QObject])
      end
    end

    describe 'QML::MetaMethod' do

      before do
        @method = @metaobj.meta_methods.find { |m| m.name == :normalMethod }
        @signal = @metaobj.meta_methods.find { |m| m.name == :someSignal }
      end

      describe '#name' do
        it 'returns the name of the method' do
          expect(@method.name).to eq(:normalMethod)
        end
      end

      describe '#arity' do
        it 'returns the number of arguments' do
          expect(@method.arity).to eq(2)
        end
      end

      describe '#arg_names' do
        it 'returns the names of the arguments' do
          expect(@method.arg_names).to eq([:x, :y])
        end
      end

      describe '#arg_types' do
        it 'returns the QMetaType ids of the arguments' do
          expect(@method.arg_types).to eq([QML::MetaType::INT, QML::MetaType::Q_STRING])
        end
      end

      describe '#signal?' do
        it 'returns whether the method is a signal' do
          expect(@method.signal?).to be(false)
          expect(@signal.signal?).to be(true)
        end
      end

      describe '#invoke' do
        it 'should invoke method' do
          result = @method.invoke(@obj, 2, '345')
          expect(result).to eq(347)
        end
      end

      describe '#connect_signal' do
        it 'should connect proc to the signal' do
          received = nil
          prc = ->(str) { received = str }
          @signal.connect_signal(@obj, &prc)

          @metaobj.meta_methods.find { |m| m.name == :emitSomeSignal }.invoke(@obj, 'foo')
          expect(received).to eq('foo')
        end
      end
    end

    describe 'QML::MetaProperty' do

      before do
        @property = @metaobj.meta_properties.find { |p| p.name == :name }
      end

      describe '#name' do
        it 'returns the property name' do
          expect(@property.name).to eq(:name)
        end
      end

      describe '#notify_signal' do
        it 'returns the notification signal for the property' do
          expect(@property.notify_signal.name).to eq(:nameChanged)
        end
      end

      describe '#set_value, #get_value' do
        it 'should set and get the property value' do
          @property.set_value(@obj, 'foobar')
          expect(@property.get_value(@obj)).to eq('foobar')
        end
      end
    end
  end
end