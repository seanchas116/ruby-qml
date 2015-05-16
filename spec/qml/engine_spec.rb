require 'spec_helper'

describe QML::Engine do

  describe '#add_import_path' do
    context 'with test module' do
      let(:path) { QML::ROOT_PATH + 'spec/assets' }

      before do
        QML.engine.add_import_path(path)
      end

      let(:data) do
        <<-EOS
          import QtQuick 2.0
          import testmodule 1.0
          Test {}
        EOS
      end
      let(:component) { QML::Component.new(data: data) }

      it 'loads a module' do
        expect(component.create.name).to eq 'poyo'
      end
    end
  end

  describe '#evaluate' do
    it 'evaluates JS scripts' do
      result = QML.engine.evaluate <<-JS
        (function() {
          return "foo";
        })();
      JS
      expect(result).to eq('foo')
    end
    context 'with error' do
      it 'fails with QMLError' do
        block = proc do
          QML.engine.evaluate <<-JS
            (function() {
              throw new Error("hoge");
            })();
          JS
        end
        expect(&block).to raise_error(/hoge/)
      end
    end
  end
end
