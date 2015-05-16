require 'spec_helper'

describe '#to_qml' do

  let(:through) { QML.engine.evaluate("(function(x) { return x; })") }

  primitives = {
    Fixnum: 10,
    Float: 1.5,
    Rational: 5/2r,
    TrueClass: true,
    FalseClass: false,
    String: 'ほげ'
  }

  primitives.each do |type, value|
    describe "#{type}\#to_qml" do
      it "converts #{type} to JS correspondings" do
        expect(through.call(value)).to eq value
      end
    end
  end

  describe 'Symbol#to_qml' do
    it 'converts Symbol to JS string' do
      expect(through.call(:hoge)).to eq 'hoge'
    end
  end

  describe 'Hash#to_qml' do
    it 'converts hash to JS object' do
      hash = {a: 1, b: 2}
      obj = through.call(hash);
      expect(obj).to be_a QML::JSObject
      hash.each do |k, v|
        expect(obj[k]).to eq v
      end
    end
  end

  describe 'Array#to_qml' do
    it 'converts array to JS array' do
      array = [1,2,3]
      jsarray = through.call(array)
      expect(jsarray).to be_a QML::JSArray
      expect(jsarray.each.to_a).to eq array
    end
  end

  describe 'Time#to_qml' do
    it 'converts time to JS Date' do
      time = Time.now
      # millisecond precision
      time -= (time.nsec % 1000000) / 1000000000r
      jsarray = through.call(time)
      expect(jsarray.to_time).to eq time
    end
  end
end
