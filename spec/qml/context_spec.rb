require 'spec_helper'

describe QML::Context do

  let(:context) { QML::Context.new }
  let(:component) do
    QML::Component.new context: context, data: <<-EOS
      import QtQuick 2.0
      QtObject {
      }
    EOS
  end
  let(:object) { component.create }

  describe '#[]=' do
    it 'makes a value available to QML context' do
      context[:foo] = 'foo'
      expect(object.qml_eval('foo')).to eq 'foo'
    end

    it 'takes ownership of Qt object' do
      life_checker = lambda do
        obj = QML::Plugins.test_util.create_test_object
        context[:obj] = obj
        QML::TestUtil::ObjectLifeChecker.new(obj)
      end.call

      QML.application.collect_garbage
      expect(life_checker.alive?).to eq true

      context[:obj] = nil
      QML.application.collect_garbage
      expect(life_checker.alive?).to eq false
    end
  end

  describe '#[]' do
    it 'returns the value previously set by []=' do
      context[:foo] = 'foo'
      expect(context[:foo]).to eq 'foo'
    end
  end
end
