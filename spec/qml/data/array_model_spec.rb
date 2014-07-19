require 'spec_helper'

describe QML::Data::ArrayModel do

  class NoColumnArrayModel < QML::Data::ArrayModel
  end

  class TestArrayModel < QML::Data::ArrayModel
    column :title, :number
  end

  let(:context) { QML::Context.new }

  let(:component) do
    QML::Component.new context: context, data: <<-EOS
      import QtQuick 2.0
      ListView {
        model: arrayModel
        delegate: Item {
          property var itemTitle: title
          property var itemNumber: number
        }
      }
    EOS
  end

  let(:list_view) { component.create }

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
      model.push *original_array
    end
  end

  before do
    context[:arrayModel] = model
  end

  shared_examples 'model content' do |text|
    it text do
      expect(model.to_a).to eq(expected_array)
    end
  end

  shared_examples 'ListView' do
    it 'updates QML ListView correctly' do
      count = list_view.count
      expect(count).to eq expected_array.size
      list_view.count.times do |i|
        list_view.current_index = i
        current = list_view.current_item
        expect(current.item_title).to eq(expected_array[i][:title])
        expect(current.item_number).to eq(expected_array[i][:number])
      end
    end
  end

  describe '#initialize' do
    context 'when columns are not specified' do
      it 'fails' do
        expect { NoColumnArrayModel.new }.to raise_error(QML::Data::Error)
      end
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
      include_examples 'model content', 'sets the element to given index'
      include_examples 'ListView'
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
      include_examples 'model content', 'inserts item'
      include_examples 'ListView'
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
        include_examples 'model content', 'deletes item'
        include_examples 'ListView'
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
        include_examples 'model content', 'deletes items'
        include_examples 'ListView'
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
      include_examples 'model content', 'clears items'
      include_examples 'ListView'
    end
  end
end
