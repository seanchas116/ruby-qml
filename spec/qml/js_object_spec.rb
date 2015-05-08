require 'spec_helper'

describe QML::JSObject do

  let(:engine) { QML::Engine.new }
  let(:obj_script) do
    <<-JS
      ({one: 1, two: 2})
    JS
  end
  let(:obj) { engine.evaluate(obj_script) }

  describe '#[]' do
    it 'get property' do
      expect(obj['one']).to eq(1)
      expect(obj['two']).to eq(2)
    end
  end

  describe '#each' do
    it 'enumerates each item' do
      expect(obj.each.to_a).to eq [["one", 1], ["two", 2]]
    end
  end

  describe '#each_pair' do
    it 'enumerates each item' do
      expect(obj.each_pair.to_a).to eq [["one", 1], ["two", 2]]
    end
  end

  describe '#keys' do
    it 'returns all keys' do
      expect(obj.keys).to eq %w{one two}
    end
  end

  describe '#values' do
    it 'returns all values' do
      expect(obj.values).to eq [1, 2]
    end
  end

  describe '#has_key?' do
    it 'returns whether it has the key' do
      expect(obj.has_key?(:one)).to eq true
      expect(obj.has_key?(:hoge)).to eq false
    end
  end

  describe '#respond_to?' do
    it 'returns whether it has the key' do
      expect(obj.has_key?(:one)).to eq true
      expect(obj.has_key?(:hoge)).to eq false
    end
  end

  describe 'method call' do

    let(:obj_script) do
      <<-JS
        ({
          one: 1,
          addOne: function(x) {
            return x + this.one;
          }
        })
      JS
    end

    context 'for non-function property' do
      it 'get it' do
        expect(obj.one).to eq 1
      end
    end
    context 'for function' do
      it 'call it as method' do
        expect(obj.addOne(2)).to eq 3
      end
    end
    context 'for non-existing key' do
      it 'fails with NoMethodError' do
        expect { obj.hoge }.to raise_error(NoMethodError)
      end
    end
  end
end
