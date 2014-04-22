require 'spec_helper'

describe QML::PluginLoader do

  describe 'load' do
    context 'with correct file path' do
      it 'creates an object instance' do
        path = Pathname(__FILE__) + '../../ext/qml/plugins/testobject'
        loader = QML::PluginLoader.new path, 'testobject'
        loader.load
        expect(loader.instance).to be_a QML::ObjectBase
      end
    end
    context 'with wrong file path' do
      it 'fails with QML::PluginError' do
        path = Pathname(__FILE__) + '../../ext/qml/plugins/wrong'
        loader = QML::PluginLoader.new path, 'testobject'
        expect { loader.load }.to raise_error(QML::PluginError)
      end
    end
  end

end