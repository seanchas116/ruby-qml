require 'spec_helper'

describe QML::Access do

  let(:component) { QML::Component.new data: data }
  let(:root) { component.create }

  describe '.register_to_qml' do

    context 'when namespace, version, name are given' do

      let(:data) do
        <<-EOS
          import QtQuick 2.2
          import AccessExampleNS 1.2
          Item {
            id: root
            property var bound: example.text + example.text
            function getExample() {
              return example;
            }
            function callSomeMethod() {
              return example.some_method(100, 200);
            }
            AccessExample {
              id: example
              property var signalArg
              onSome_signal: {
                signalArg = arg;
              }
            }
          }
        EOS
      end
      let(:example) { root.getExample }

      it 'registers the class as a QML type' do
        expect { component.create }.not_to raise_error
      end
      describe 'AccessExamle#some_method' do
        it 'returns value' do
          expect(root.callSomeMethod()).to eq 300
        end
      end
      describe 'AccessExamle text property' do
        it 'can be used to property binding' do
          example.text = "foo"
          expect(root.bound).to eq 'foofoo'
          example.text = "bar"
          expect(root.bound).to eq 'barbar'
        end
      end
      describe 'AccessExamle some_signal signal' do
        it 'can be connected' do
          example.unwrap.some_signal.emit('foo')
          expect(example.signalArg).to eq 'foo'
        end
      end
    end

    context 'when arguments are omitted' do

      let(:data) do
        <<-EOS
          import AccessExampleModule 0.1
          AccessExample {}
        EOS
      end

      it 'guesses them from the Ruby class name, namespace and VERSION constant' do
        expect { component.create }.not_to raise_error
      end
    end
  end
end
