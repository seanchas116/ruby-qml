require 'spec_helper'

describe QML::Application do
  let(:application) { QML::Application.instance }
  describe '.instance' do
    it 'returns the QML::Application instance' do
      expect(application).to be_a(QML::Application)
    end
  end
  describe '#engine' do
    it 'returns the default engine of the application' do
      expect(application.engine).to be_a(QML::Engine)
    end
  end
  describe '#context' do
    it 'returns engine.context' do
      expect(application.context).to be(application.engine.context)
    end
  end
  describe '#load_data' do
    let(:data) do
      <<-EOS
        import QtQuick 2.0
        QtObject {
          property string name: 'foo'
        }
      EOS
    end
    it 'loads root object 1from data' do
      application.load_data data
      expect(application.root_component.data).to eq data
      expect(application.root.name).to eq 'foo'
    end
  end
  describe '#load_path' do
    let(:path) { QML::ROOT_PATH + 'spec/assets/testobj.qml' }
    it 'loads root object from path' do
      application.load_path path
      expect(application.root_component.path).to eq path
      expect(application.root.name).to eq 'foo'
    end
  end
end
