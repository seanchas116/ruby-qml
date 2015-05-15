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
end
