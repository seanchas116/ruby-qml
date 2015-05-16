require 'spec_helper'

describe QML::ListModel do
  let(:model) { QML::ListModel.allocate }

  describe '#count' do
    it 'fails with NotImplementedError by default' do
      expect { model.count }.to raise_error(NotImplementedError)
    end
  end

  describe '#[]' do
    it 'fails with NotImplementedError by default' do
      expect { model[0] }.to raise_error(NotImplementedError)
    end
  end
end
