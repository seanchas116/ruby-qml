require 'spec_helper'

describe QML::Component do

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
      let(:component) { QML::Component.new(data: data) }

      it 'instantiates a object' do
        expect(component.create.name).to eq 'foo'
      end

      describe '#data' do
        it 'returns its data' do
          expect(component.data).to eq data
        end
      end
    end

    context 'with file path' do
      let(:path) { QML::ROOT_PATH + 'spec/assets/testobj.qml' }
      let(:component) { QML::Component.new(path: path) }

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

  describe '#initialize' do
    context 'when neither string nor path specified' do
      it 'fails with TypeError' do
        expect { QML::Component.new }.to raise_error(TypeError)
      end
    end
  end
end
