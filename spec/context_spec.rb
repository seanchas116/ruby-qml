require 'spec_helper'

describe QML::Context do

  let(:component) do
    QML::Component.new data: <<-EOS
      import QtQuick 2.0
      QtObject {
      }
    EOS
  end
  let(:object) { component.create }
  let(:context) { engine.context }

  describe '#engine' do
    it 'returns its engine' do
      expect(context.engine).to eq engine
    end
  end

  describe '#[]=' do
    it 'makes a value available to QML context' do
      context[:foo] = 'foo'
      expect(object.qml_eval('foo')).to eq 'foo'
    end
  end

  describe '#[]' do
    it 'returns the value previously set by []=' do
      context[:foo] = 'foo'
      expect(context[:foo]).to eq 'foo'
    end
  end
end
