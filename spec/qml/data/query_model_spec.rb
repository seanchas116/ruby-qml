require 'spec_helper'

describe QML::Data::QueryModel do

  let(:klass) do
    Class.new(QML::Data::QueryModel) do
      attr_accessor :data

      def initialize(count)
        @data = count.times.map { |i| {title: "title: #{i}", number: i} }
        super(:title, :number)
      end

      def query(offset, count)
        @data[offset ... offset + count]
      end

      def query_count
        @data.size
      end
    end
  end

  let(:model) { klass.new(2000) }
  let(:expected_array) { model.data }

  describe '#count' do
    it 'returns the count and updated by #update' do
      count = model.data.size
      expect(model.count).to eq(count)
      model.data << {value: 0}
      expect(model.count).to eq(count)
      model.update
      expect(model.count).to eq(count + 1)
    end
  end

  describe '#[]' do
    it 'returns the item' do
      model.data.size.times do |i|
        expect(model[i]).to eq(model.data[i])
      end
    end
  end

  describe '#query_count' do
    it 'fails with NotImplementedError by default' do
      expect { QML::Data::QueryModel.allocate.query_count }.to raise_error(NotImplementedError)
    end
  end

  describe '#query' do
    it 'fails with NotImplementedError by default' do
      expect { QML::Data::QueryModel.allocate.query(0, 100) }.to raise_error(NotImplementedError)
    end
  end

  include_context 'ListView for model available'
  it_behaves_like 'ListView data source' do
    let(:model) { klass.new(10) }
  end
end
