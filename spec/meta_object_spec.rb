require 'spec_helper'
require 'qml/meta_object'

describe QML::MetaObject do

  context 'with TestObject' do

    before do
      @metaobj = QML::MetaObject.new(QML::CLib.rbqml_testobject_static_metaobject)
      @obj = QML::QtObjectBase.new(QML::CLib.rbqml_testobject_new)
    end

    describe '#name, #to_s' do
      it 'should return class name' do
        expect(@metaobj.name).to eq('RubyQml::TestObject')
        expect(@metaobj.to_s).to eq('RubyQml::TestObject')
      end
    end

    describe '#inspect' do
      it 'returns the inspection' do
        expect(@metaobj.inspect).to eq('<QML::MetaObject:RubyQml::TestObject>')
      end
    end

    describe '#meta_methods' do
      it 'returns a hash of meta method arrays grouped by their names' do
        meta_methods = @metaobj.meta_methods
        expect(meta_methods).to be_a(Hash)
        expect(meta_methods.length).to eq(7)
      end
    end

    describe '#meta_properties' do
      it 'returns a hash of the meta properties' do
        meta_properties = @metaobj.meta_properties
        expect(meta_properties).to be_a(Hash)
        expect(meta_properties.length).to eq(1)
      end
    end

    describe '#enums' do
      it 'returns the enums' do
        enums = @metaobj.enums
        expected = {
          Apple: 0, Banana: 1, Orange: 2
        }
        expect(enums).to eq(expected)
      end
    end

    describe '#superclass' do
      it 'returns the super meta object' do
        expect(@metaobj.superclass.name).to eq('QObject')
      end
    end

    describe '#ancestors' do
      it 'returns the ancestor meta objects' do
        expect(@metaobj.ancestors.map(&:name)).to eq(['RubyQml::TestObject', 'QObject'])
      end
    end

    describe '#==, #eql?' do
      it 'compares 2 meta objects by its pointer value' do
        metaobj2 = QML::MetaObject.new(QML::CLib.rbqml_testobject_static_metaobject)
        expect(@metaobj == metaobj2).to eq(true)
        expect(@metaobj.eql?(metaobj2)).to eq(true)
      end
    end

    describe '#hash' do
      it 'generates the hash from its pointer value' do
        expect(@metaobj.hash).to eq(@metaobj.pointer.to_i.hash)
      end
    end

    describe 'QML::MetaMethod' do

      before do
        @method = @metaobj.meta_methods[:normalMethod].first
        @signal = @metaobj.meta_methods[:someSignal].first
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

          @metaobj.meta_methods[:emitSomeSignal].first.invoke(@obj, 'foo')
          expect(received).to eq('foo')
        end
      end
    end

    describe 'QML::MetaProperty' do

      before do
        @property = @metaobj.meta_properties[:name]
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