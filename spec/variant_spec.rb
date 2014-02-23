require 'spec_helper'

describe QML::Variant do

  it 'can store integer' do
    variant = QML::Variant.new 1234
    expect(variant.to_i).to eq(1234)
  end

  it 'can store string' do
    variant = QML::Variant.new 'poyopoyo'
    expect(variant.to_s).to eq('poyopoyo')
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
    hash = { one: 1, two: 2, three: "三" }
    variant = QML::Variant.new hash
    expect(variant.to_hash).to eq(hash)
  end

  describe '#type' do
    it 'returns the QMetaType id of the storage type' do
      variant = QML::Variant.new 10
      expect(variant.type_number).to eq(QML::Variant::TYPE_INT)
      variant = QML::Variant.new 'abcd'
      expect(variant.type_number).to eq(QML::Variant::TYPE_Q_STRING)
    end
  end

  describe '#convert' do
    it 'returns a new variant which value is converted to the specified type' do
      variant = QML::Variant.new 1234
      variant = variant.convert(QML::Variant::TYPE_Q_STRING)
      expect(variant.value).to eq('1234')
    end
  end

end

