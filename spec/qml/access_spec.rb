require 'spec_helper'

describe QML::Access do
  class Foo
    include QML::Access

    property :text
    signal :some_signal, [:arg]

    def some_method(a, b)
      a + b
    end

    def variadic_method(*args)
    end
  end

  class Bar < Foo
    def some_method(a, b)
      'overridden'
    end

    def bar_method
      'bar'
    end
  end

  let(:context) { QML::Context.new }
  let(:component) { QML::Component.new context: context, data: data }
  let(:root) { component.create }

  describe '.register_to_qml' do

    context 'when namespace, version, name are given' do

      class Hoge
        include QML::Access

        register_to_qml under: 'HogeNS', version: '1.2', name: 'Hoge'

        property :text

        def some_method(a, b)
          a + b
        end
      end

      let(:data) do
        <<-EOS
          import QtQuick 2.2
          import HogeNS 1.2
          Item {
            id: root
            property var bound: hoge.text + hoge.text
            Hoge { id: hoge }
          }
        EOS
      end
      it 'registers the class as a QML type' do
        expect { component.create }.not_to raise_error
      end
      describe 'Hoge#some_method' do
        it 'returns value' do
          expect(root.qml_eval('hoge.some_method(100, 200)')).to eq 300
        end
      end
      describe 'Hoge text property' do
        it 'can be used to property binding' do
          root.qml_eval('hoge').text = "foo"
          expect(root.qml_eval('root').bound).to eq 'foofoo'
          root.qml_eval('hoge').text = "bar"
          expect(root.qml_eval('root').bound).to eq 'barbar'
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

  context 'when used in context' do

    let(:data) do
      <<-EOS
        import QtQuick 2.2
        Item {
          id: root
          property string text: foo.text + foo.text
          Connections {
            id: connections
            target: foo
            property var args
            onSome_signal: {
              args = [arg]
            }
          }
        }
      EOS
    end
    let(:foo) { Foo.new }
    let(:bar) { Bar.new }

    before do
      context[:foo] = foo
      context[:bar] = bar
      root
    end

    describe 'Foo#some_method' do
      it 'returns value' do
        expect(root.qml_eval('foo.some_method(100, 200)')).to eq 300
      end
    end
    describe 'Bar#some_method' do
      it 'returns value' do
        expect(root.qml_eval('bar.some_method(100, 200)')).to eq 'overridden'
      end
    end
    describe 'Foo#variadic_method' do
      it 'will not be exported' do
        expect { root.qml_eval('foo.variadic_method()') }.to raise_error(QML::QMLError)
      end
    end
    describe 'Foo name property' do
      it 'can get and set value' do
        root.qml_eval('foo.name = "test"')
        expect(root.qml_eval('foo.name')).to eq 'test'
      end
      it 'can be used for property binding' do
        foo.text = 'hoge'
        expect(root.qml_eval('root.text')).to eq 'hogehoge'
        foo.text = 'piyo'
        expect(root.qml_eval('root.text')).to eq 'piyopiyo'
      end
    end
    describe 'Foo some_signal signal' do
      it 'can be connected' do
        foo.some_signal.emit('foo')
        expect(root.qml_eval('connections.args')).to eq ['foo']
      end
    end
  end
end
