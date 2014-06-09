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

  let(:engine) { QML::Engine.new }
  let(:component) { QML::Component.new engine, data: data }
  let(:root) { component.create }

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
      engine.context[:foo] = foo
      engine.context[:bar] = bar
      root
    end

    describe '#some_method' do
      it 'returns value' do
        expect(root.qml_eval('foo.some_method(100, 200)')).to eq 300
      end
    end
    describe 'subclass #some_method' do
      it 'returns value' do
        expect(root.qml_eval('bar.some_method(100, 200)')).to eq 'overridden'
      end
    end
    describe '#variadic_method' do
      it 'will not be exported' do
        expect { root.qml_eval('foo.variadic_method()') }.to raise_error(QML::QMLError)
      end
    end
    describe 'name property' do
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
    describe 'some_signal signal' do
      it 'can be connected' do
        foo.some_signal.emit('foo')
        expect(root.qml_eval('connections.args')).to eq ['foo']
      end
    end
  end

  context 'when used as QML type' do
  end
end