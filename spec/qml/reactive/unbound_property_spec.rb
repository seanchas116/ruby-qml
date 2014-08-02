require 'spec_helper'

describe QML::Reactive::UnboundProperty do
  include_context 'Reactive test objects'
  let(:property) { button_class.instance_property(:name) }
  let(:alias_property) { button_class.instance_property(:title) }

  describe '#name' do
    it 'returns the name' do
      expect(property.name).to eq(:name)
      expect(alias_property.name).to eq(:title)
    end
  end

  describe '#owner' do
    it 'returns the owner class' do
      expect(property.owner).to be(button_class)
      expect(alias_property.owner).to be(button_class)
    end
  end

  describe '#original' do
    it 'returns the original name of the alias property' do
      expect(alias_property.original).to eq(:name)
    end
  end

  describe '#alias?' do
    context 'for non-alias properties' do
      it 'returns false' do
        expect(property).not_to be_alias
      end
    end
    context 'for alias properties' do
      it 'returns true' do
        expect(alias_property).to be_alias
      end
    end
  end
end
