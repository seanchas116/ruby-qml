require 'spec_helper'

=begin

require 'qml/meta_object'

describe QML::MetaObject do

  context 'with TestObject' do

    before do
      @metaobj = QML::CLib.rbqml_testobject_static_metaobject
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
        expect(meta_methods.length).to eq(8)
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
        metaobj2 = QML::CLib.rbqml_testobject_static_metaobject
        expect(@metaobj == metaobj2).to eq(true)
        expect(@metaobj.eql?(metaobj2)).to eq(true)
      end
    end

    describe '#hash' do
      it 'generates the hash from its pointer value' do
        expect(@metaobj.hash).to eq(@metaobj.pointer.to_i.hash)
      end
    end

  end
end

=end