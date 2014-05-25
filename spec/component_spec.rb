require 'spec_helper'

describe QML::Component do

  let(:engine) { QML::Engine.new }

  describe '#create' do

    context 'with string' do
      let(:data) do
        <<-EOS
          import QtQuick 2.0
          QtObject {
            property string name: 'foo'
          }
        EOS
      end
      let(:component) { QML::Component.new engine, data: data }

      it 'instantiates a object' do
        expect(component.create.name).to eq 'foo'
      end

      describe '#data' do
        it 'returns its data' do
          expect(component.data).to eq data
        end
      end

      describe '#engine' do
        it 'returns its engine' do
          expect(component.engine).to eq engine
        end
      end
    end

    context 'with file path' do
      let(:path) { QML::ROOT_PATH + 'spec/assets/testobj.qml' }
      let(:component) { QML::Component.new engine, path: path }

      it 'instantiates a object' do
        expect(component.create.name).to eq 'foo'
      end

      describe '#path' do
        it 'returns its path' do
          expect(component.path).to eq path
        end
      end
    end
  end
end
