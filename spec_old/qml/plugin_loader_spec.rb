require 'spec_helper'

describe QML::PluginLoader do

  let(:loader) { QML::PluginLoader.new path, 'rubyqml-testutil' }

  describe 'load' do

    context 'with signle path' do
      let(:path) { QML::ROOT_PATH + 'ext/qml/plugins/testutil' + QML::PluginLoader.lib_filename('rubyqml-testutil') }
      let(:loader) { QML::PluginLoader.new path }
      it 'creates an object instance' do
        loader.load
        expect(loader.instance).to be_a QML::QtObjectBase
      end
    end

    context 'with correct file path' do
      let(:path) { QML::ROOT_PATH + 'ext/qml/plugins/testutil' }
      it 'creates an object instance' do
        loader.load
        expect(loader.instance).to be_a QML::QtObjectBase
      end
    end

    context 'with wrong file path' do
      let(:path) { QML::ROOT_PATH + 'ext/qml/plugins/wrong' }
      it 'fails with QML::PluginError' do
        expect { loader.load }.to raise_error(QML::PluginError)
      end
    end
  end
end
