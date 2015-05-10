require 'spec_helper'

describe QML::JSArray do

  let(:engine) { QML::Engine.new }
  let(:array_script) do
    <<-JS
      [1, 2, 3]
    JS
  end
  let(:array) { engine.evaluate(array_script) }

  describe '#each' do
    it 'enumerates each values' do
      expect(array.each.to_a).to eq [1,2,3]
    end
  end

  describe '#to_a' do
    it 'converts it to an array' do
      expect(array.to_a).to eq [1,2,3]
    end
  end

  describe '#length' do
    it 'returns length' do
      expect(array.length).to eq 3
    end
  end
end
