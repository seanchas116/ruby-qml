require 'spec_helper'

describe QML::Variant do

  describe '.new' do

    it 'can store integer' do
      variant = QML::Variant.new 1234
      expect(variant.to_i).to eq(1234)
    end

    it 'can store void pointer' do
      ptr = FFI::MemoryPointer.new(:int)
      variant = QML::Variant.new(ptr)
      result = variant.value
      expect(result).to be_a(FFI::Pointer)
      expect(result).to eq(ptr)
    end

    it 'can store string' do
      variant = QML::Variant.new 'poyopoyo'
      expect(variant.to_s).to eq('poyopoyo')
    end

    it 'can store non-ascii string' do
      variant = QML::Variant.new 'ぽよぽよ'
      expect(variant.to_s).to eq('ぽよぽよ')
    end

    it 'can store float' do
      variant = QML::Variant.new 3.1416
      expect(variant.to_f).to eq(3.1416)
    end

    it 'can store true' do
      variant = QML::Variant.new true
      expect(variant.value).to eq(true)
    end
    
    it 'can store false' do
      variant = QML::Variant.new false
      expect(variant.value).to eq(false)
    end

    it 'can store symbol as string' do
      variant = QML::Variant.new :hoge
      expect(variant.value).to eq('hoge')
    end

    it 'can store time' do
      time = Time.now
      variant = QML::Variant.new(time)
      result = variant.value

      expect(result.year).to eq(time.year)
      expect(result.month).to eq(time.month)
      expect(result.day).to eq(time.day)
      expect(result.hour).to eq(time.hour)
      expect(result.min).to eq(time.min)
      expect(result.sec).to eq(time.sec)
      expect(result.nsec / 1_000_000).to eq(time.nsec / 1_000_000)
      expect(result.gmt_offset).to eq(result.gmt_offset)
    end

    it 'can store array' do
      array = [1, 'foo', [2, 3]]
      variant = QML::Variant.new array
      expect(variant.to_a).to eq(array)
    end

    it 'can store hash' do
      hash = { one: 1, two: 2, three: "three" }
      variant = QML::Variant.new hash
      expect(variant.to_hash).to eq(hash)
    end

    it 'can store hash with non-ascii keys' do
      hash = { あ: 1, い: 2 }
      variant = QML::Variant.new(hash)
      expect(variant.to_hash).to eq(hash)
    end

    it 'can store Qt object' do
      obj = QML::TestObject.new
      obj.name = 'lorem ipsum'
      variant = QML::Variant.new(obj)
      expect(variant.value.name).to eq('lorem ipsum')
    end

    it 'can store MetaObject' do
      metaobj = QML::CLib.rbqml_testobject_static_metaobject
      variant = QML::Variant.new(metaobj)
      expect(variant.value).to eq(metaobj)
    end

    it 'raises error when parameter is not convertible to Variant' do
      expect { QML::Variant.new(Class.new.new) }.to raise_error(TypeError)
    end

  end

  describe '#meta_type' do
    it 'returns the MetaType of the storage type' do
      variant = QML::Variant.new 10
      expect(variant.meta_type).to eq(QML::MetaType::INT)
      variant = QML::Variant.new 'abcd'
      expect(variant.meta_type).to eq(QML::MetaType::Q_STRING)
    end
  end

  describe '#convert' do
    it 'returns a new variant which value is converted to the specified type' do
      variant = QML::Variant.new 1234
      variant = variant.convert(QML::MetaType::Q_STRING)
      expect(variant.value).to eq('1234')
    end
  end

end

