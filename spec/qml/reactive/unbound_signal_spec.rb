require 'spec_helper'

describe QML::Reactive::UnboundSignal do
  include_context 'Reactive test objects'
  let(:signal) { button_class.instance_signal(:pressed) }
  let(:alias_signal) { button_class.instance_signal(:clicked) }
  let(:variadic_signal) { button_class.instance_signal(:message) }

  describe '#name' do
    it 'returns the name' do
      expect(signal.name).to eq(:pressed)
      expect(alias_signal.name).to eq(:clicked)
    end
  end

  describe '#owner' do
    it 'returns the owner class' do
      expect(signal.owner).to be(button_class)
      expect(alias_signal.owner).to be(button_class)
    end
  end

  describe '#original' do
    it 'returns the original name of the alias signal' do
      expect(alias_signal.original).to eq(:pressed)
    end
  end

  describe '#variadic?' do
    context 'for non-variadic signals' do
      it 'returns false' do
        expect(signal).not_to be_variadic
      end
    end
    context 'for variadic signals' do
      it 'returns true' do
        expect(variadic_signal).to be_variadic
      end
    end
  end

  describe '#alias?' do
    context 'for non-alias signals' do
      it 'returns false' do
        expect(signal).not_to be_alias
      end
    end
    context 'for alias signals' do
      it 'returns true' do
        expect(alias_signal).to be_alias
      end
    end
  end

  describe '#arity' do
    it 'returns the number of arguments' do
      expect(signal.arity).to eq(1)
      expect(variadic_signal.arity).to eq(-1)
      expect(alias_signal.arity).to eq(1)
    end
  end

  describe '#parameters' do
    it 'returns parameter names' do
      expect(signal.parameters).to eq([[:req, :pos]])
      expect(alias_signal.parameters).to eq([[:req, :pos]])
      expect(variadic_signal.parameters).to eq([[:rest, :args]])
    end
  end
end
