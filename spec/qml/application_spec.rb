require 'spec_helper'

describe QML::Application do
  let(:application) { QML.application }

  describe '#engine' do
    it 'returns the default engine of the application' do
      expect(application.engine).to be_a(QML::Engine)
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

describe QML do
  describe '.application' do
    it 'returns the QML::Application instance' do
      expect(QML.application).to be_a(QML::Application)
    end
  end

  describe '.next_tick' do
    it 'do a task later in event loop' do
      finished = false
      QML.next_tick do
        finished = true
      end
      QML.application.process_events
      expect(finished).to eq true
    end
  end
end
