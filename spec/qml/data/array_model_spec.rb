require 'spec_helper'

describe QML::Data::ArrayModel do

  class TestArrayModel < QML::Data::ArrayModel
    def initialize
      super(:title, :number)
    end
  end

  let(:original_array) do
    [
      {title: 'hoge', number: 12},
      {title: 'piyo', number: 34},
      {title: 'fuga', number: 1234}
    ]
  end

  let(:additional_array) do
    [
      {title: 'foo', number: 56},
      {title: 'bar', number: 78}
    ]
  end

  let(:expected_array) { original_array.dup }

  let(:model) do
    TestArrayModel.new.tap do |model|
      model.push(*original_array)
    end
  end

  include_context 'ListView for model available'

  shared_examples 'same as expected array' do |text|
    it text do
      expect(model.to_a).to eq(expected_array)
    end
  end

  describe '#each' do
    context 'with no block' do
      it 'returns an Enumerator' do
        expect(model.each).to be_a Enumerator
        expect(model.each.to_a).to eq expected_array
      end
    end

    context 'with block' do
      it 'iterates each item' do
        items = []
        model.each do |item|
          items << item
        end
        expect(items).to eq expected_array
      end
    end
  end

  describe '#to_a' do
    it 'returns the array the elements are stored in' do
      expect(model.to_a).to eq expected_array
    end
  end

  describe '#count' do
    it 'returns the number of items' do
      expect(model.count).to eq expected_array.size
    end
  end

  describe '#[]' do
    it 'returns the item for given index' do
      expect(model[1]).to eq(expected_array[1])
    end
  end

  describe '#[]=' do

    it 'returns the assigned item' do
      item = additional_array[0]
      expect(model[2] = item).to eq item
    end

    context 'after called' do
      before do
        model[2] = additional_array[0]
        expected_array[2] = additional_array[0]
      end
      it_behaves_like 'same as expected array', 'sets the element to given index'
      it_behaves_like 'ListView data source'
    end
  end

  describe '#insert' do

    it 'returns self' do
      expect(model.insert(1, *additional_array)).to eq model
    end

    context 'after called' do
      before do
        model.insert(1, *additional_array)
        expected_array.insert(1, *additional_array)
      end
      it_behaves_like 'same as expected array', 'inserts item'
      it_behaves_like 'ListView data source'
    end
  end

  describe '#delete_at' do

    context 'with no number' do
      it 'deletes 1 item and returns it' do
        expect(model.delete_at(1)).to eq original_array[1]
      end
      context 'after called' do
        before do
          model.delete_at(1)
          expected_array.delete_at(1)
        end
        it_behaves_like 'same as expected array', 'deletes item'
        it_behaves_like 'ListView data source'
      end
    end

    context 'with number' do
      it 'deletes multiple items and returns them as an array' do
        expect(model.delete_at(1, 2)).to eq original_array[1..2]
      end
      context 'after called' do
        before do
          model.delete_at(1, 2)
          2.times { expected_array.delete_at(1) }
        end
        it_behaves_like 'same as expected array', 'deletes items'
        it_behaves_like 'ListView data source'
      end
    end
  end

  describe '#unshift' do
    it 'prepends items' do
      expect(model.unshift(*additional_array).to_a).to eq expected_array.unshift(*additional_array)
    end
  end

  describe '#shift' do
    it 'removes first items and returns them' do
      expect(model.shift(2)).to eq expected_array[0..1]
      expect(model.to_a).to eq expected_array.tap { |a| a.shift(2) }
    end
  end

  describe '#push' do
    it 'appends items' do
      expect(model.push(*additional_array).to_a).to eq expected_array.push(*additional_array)
    end
  end

  describe '#pop' do
    it 'removes last items and returns them' do
      expect(model.pop(2)).to eq expected_array[-2..-1]
      expect(model.to_a).to eq expected_array.tap { |a| a.pop(2) }
    end
  end

  describe '#<<' do
    it 'is an alias of #push' do
      expect((model << additional_array[0]).to_a).to eq(expected_array << additional_array[0])
    end
  end

  describe '#clear' do
    it 'returns self' do
      expect(model.clear).to be(model)
    end
    context 'after called' do
      before do
        model.clear
        expected_array.clear
      end
      it_behaves_like 'same as expected array', 'clears items'
      it_behaves_like 'ListView data source'
    end
  end

  describe '#replace' do
    it 'returns self' do
      expect(model.replace(expected_array + additional_array)).to be(model)
    end
    context 'after called' do
      before do
        model.replace(expected_array + additional_array)
        expected_array.push *additional_array
      end
      it_behaves_like 'same as expected array', 'replaces entire array'
      it_behaves_like 'ListView data source'
    end
  end
end
