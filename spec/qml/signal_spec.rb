require 'spec_helper'

describe QML::Signal do

  before do
    @signal = QML::Signal.new([:foo, :bar])
  end

  describe 'connection' do

    describe '#connect, #emit' do

      before do
        @signal.connect { |foo, bar| @received = [foo, bar] }
      end

      it 'connects a block and calls it' do
        @signal.emit('apple', 'orange')
        expect(@received).to eq(['apple', 'orange'])
      end

      context 'when the number of arguments is wrong' do
        it 'fails with an ArgumentError' do
          expect { @signal.emit('apple') }.to raise_error(ArgumentError)
        end
      end

      it 'calls connected blocks in the order they have been conncted' do
        received = []
        @signal.connect { received << 'first' }
        @signal.connect { received << 'second' }
        @signal.emit('foo', 'bar')
        expect(received).to eq(['first', 'second'])
      end
    end

    describe '::Connection#disconnect' do

      it 'disconnects the connection' do
        connection = @signal.connect { |foo, bar| @received = foo + bar }
        @signal.emit('apple', 'orange')
        connection.disconnect
        @signal.emit('banana', 'grape')
        expect(@received).to eq('appleorange')
      end
    end

    describe '#connection_count' do
      it 'returns the number of connections' do
        prc1 = ->(foo, bar) {}
        prc2 = ->(foo, bar) {}
        @signal.connect(&prc1)
        @signal.connect(&prc2)
        expect(@signal.connection_count).to eq(2)
      end
    end
  end

  describe 'attributes' do

    describe '#arity' do
      it 'returns the number of arguments' do
        expect(@signal.arity).to eq(2)
      end
    end

    describe '#parameters' do
      it 'returns the parameter information' do
        expect(@signal.parameters).to eq([[:req, :foo], [:req, :bar]])
      end
    end
  end
end
