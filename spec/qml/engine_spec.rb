require 'spec_helper'

describe QML::Engine do
  describe '.new' do
    it 'fails with QML::EngineError' do
      expect { QML::Engine.new }.to raise_error(QML::EngineError)
    end
  end
end

describe QML do
  describe '.engine' do
    it 'returns the instance of QML::Engine' do
      expect(QML.engine).to be_a(QML::Engine)
    end
  end
end
