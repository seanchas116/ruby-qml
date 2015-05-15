require 'spec_helper'

describe 'JS value conversions' do

  let(:through) { QML.engine.evaluate("(function(x) { return x; })") }

  primitives = {
    integer: 10,
    float: 1.5,
    true: true,
    false: false,
    string: 'ほげ'
  }

  primitives.each do |type, value|
    describe "#{type}" do
      it 'is converted from/to Ruby correspondings' do
        expect(through.call(value)).to eq value
      end
    end
  end

  describe 'Hash#to_qml' do
    it 'converts hash to JS object' do
      hash = {a: 1, b: 2}
      obj = through.call(hash.to_qml);
      expect(obj).to be_a QML::JSObject
      hash.each do |k, v|
        expect(obj[k]).to eq v
      end
    end
  end

  describe 'Array#to_qml' do
    it 'converts array to JS array' do
      array = [1,2,3]
      jsarray = through.call(array.to_qml)
      expect(jsarray).to be_a QML::JSArray
      expect(jsarray.each.to_a).to eq array
    end
  end
end
