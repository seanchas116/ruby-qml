require 'spec_helper'

describe QML::Access do

  let(:engine) { QML::Engine.new }
  let(:component) { QML::Component.new engine: engine, data: data }
  let(:root) { component.create }

  describe '.register_to_qml' do

    context 'when namespace, version, name are given' do

      class Hoge
        include QML::Access

        property :text
        signal :some_signal, [:arg]

        def some_method(a, b)
          a + b
        end

        register_to_qml under: 'HogeNS', version: '1.2', name: 'Hoge'
      end

      let(:data) do
        <<-EOS
          import QtQuick 2.2
          import HogeNS 1.2
          Item {
            id: root
            property var bound: hoge.text + hoge.text
            function getHoge() {
              return hoge;
            }
            function callSomeMethod() {
              return hoge.some_method(100, 200);
            }
            Hoge {
              id: hoge
              property var signalArg
              onSome_signal: {
                signalArg = arg;
              }
            }
          }
        EOS
      end
      let(:hoge) { root.getHoge }

      it 'registers the class as a QML type' do
        expect { component.create }.not_to raise_error
      end
      describe 'Hoge#some_method' do
        it 'returns value' do
          expect(root.callSomeMethod()).to eq 300
        end
      end
      describe 'Hoge text property' do
        it 'can be used to property binding' do
          hoge.text = "foo"
          expect(root.bound).to eq 'foofoo'
          hoge.text = "bar"
          expect(root.bound).to eq 'barbar'
        end
      end
      describe 'Hoge some_signal signal' do
        it 'can be connected' do
          hoge.unwrap.some_signal.emit('foo')
          expect(hoge.signalArg).to eq 'foo'
        end
      end
    end

    context 'when arguments are omitted' do

      module HogeModule
        VERSION = '0.1.0'
        class Hoge
          include QML::Access
          register_to_qml
        end
      end

      let(:data) do
        <<-EOS
          import HogeModule 0.1
          Hoge {}
        EOS
      end

      it 'guesses them from the Ruby class name, namespace and VERSION constant' do
        expect { component.create }.not_to raise_error
      end
    end
  end
end
