require 'spec_helper'

describe QML::Reactive::Property do

  describe '#initialize' do

    context 'when a value is given' do
      it 'initializes with the value' do
        p = QML::Reactive::Property.new(2)
        expect(p.value).to eq(2)
      end
    end

    context 'when a block is given' do
      it 'initializes with the property binding' do
        p1 = QML::Reactive::Property.new(2)
        p2 = QML::Reactive::Property.new { p1.value + 2 }
        p1.value = 10
        expect(p2.value).to eq(12)
      end
    end
  end

  describe 'access to the value' do
    describe '#value, #value=' do
      it 'sets and gets the contained value' do
        property = QML::Reactive::Property.new
        property.value = 'hogehoge'
        expect(property.value).to eq('hogehoge')
      end
    end
  end

  describe 'property binding' do

    describe '#bind' do

      it 'sets a property binding' do
        p1 = QML::Reactive::Property.new 'foo'
        p2 = QML::Reactive::Property.new 'bar'
        p3 = QML::Reactive::Property.new
        p4 = QML::Reactive::Property.new
        p3.bind { p1.value + p2.value }
        p4.bind { p3.value }
        p1.value = 'hoge'
        p2.value = 'piyo'
        expect(p3.value).to eq('hogepiyo')
        expect(p4.value).to eq('hogepiyo')
      end

      context 'when binding is recursive' do
        it 'raises error' do
          property = QML::Reactive::Property.new
          expect{ property.bind { property.value } }.to raise_error(QML::Reactive::Error)
        end
      end
    end
  end

  describe '#changed' do
    it 'returns a signal that is emitted when the value is changed' do
      property = QML::Reactive::Property.new
      changed_value = nil
      property.changed.connect { |val| changed_value = val }

      property.value = 'piyopiyo'
      expect(changed_value).to eq('piyopiyo')
    end
  end
end
