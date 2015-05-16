require 'spec_helper'

describe QML::PluginLoader do

  let(:loader) { QML::PluginLoader.new path, 'rubyqml-plugin' }

  describe '#instance' do

    context 'with signle path' do
      let(:path) { QML::ROOT_PATH + 'ext/qml/rubyqml-plugin' + QML::PluginLoader.lib_filename('rubyqml-plugin') }
      let(:loader) { QML::PluginLoader.new path }
      it 'returns an object instance' do
        expect(loader.instance).to respond_to('createListModel')
      end
    end

    context 'with correct file path' do
      let(:path) { QML::ROOT_PATH + 'ext/qml/rubyqml-plugin' }
      it 'returns an object instance' do
        expect(loader.instance).to respond_to('createListModel')
      end
    end

    context 'with wrong file path' do
      let(:path) { QML::ROOT_PATH + 'ext/qml/plugins/wrong' }
      it 'fails with QML::PluginError' do
        expect { loader.instance }.to raise_error(QML::PluginError)
      end
    end
  end
end
