require 'spec_helper'

describe QML::Component do

  let(:engine) { QML::Engine.new }

  describe '#create' do

    context 'with string' do
      let(:component) do
        QML::Component.new engine, data: <<-EOS
          import QtQuick 2.0
          QtObject {
            property string name: 'foo'
          }
        EOS
      end

      it 'instantiates a object' do
        expect(component.create.name).to eq 'foo'
      end
    end

    context 'with file path' do
      let(:component) do
        QML::Component.new engine,
          path: QML::ROOT_PATH + 'spec/assets/testobj.qml'
      end

      it 'instantiates a object' do
        expect(component.create.name).to eq 'foo'
      end
    end
  end
end
