require 'spec_helper'

describe QML::Engine do

  let(:engine) { QML::Engine.new }

  describe '#import_paths' do
    it 'returns array' do
      expect(engine.import_paths()).to be_a(Array)
    end
  end

  describe '#add_import_path' do
    context 'with test module' do
      let(:path) { (QML::ROOT_PATH + 'spec/assets').to_s }

      it 'adds QML import path' do
        engine.add_import_path(path)
        paths = engine.import_paths()
        expect(paths[0]).to eq path
      end

      let(:data) do
        <<-EOS
          import QtQuick 2.0
          import testmodule 1.0
          Test {}
        EOS
      end
      let(:component) { QML::Component.new(engine: engine, data: data) }

      it 'loads a module' do
        expect(component.create.name).to eq 'poyo'
      end
    end
  end
end
