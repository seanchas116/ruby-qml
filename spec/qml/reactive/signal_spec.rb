require 'spec_helper'

describe QML::Reactive::Signal do

  before do
    @signal = QML::Reactive::Signal.new([:foo, :bar])
    @variadic_signal = QML::Reactive::Signal.new(nil)
  end

  describe 'connection' do

    describe '#connect, #emit' do

      context 'when non-variadic' do

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
      end

      context 'when variadic' do

        it 'connects a block and calls it' do
          received = nil
          @variadic_signal.connect { |foo, bar| received = [foo, bar] }
          @variadic_signal.emit('apple', 'orange')
          expect(received).to eq(['apple', 'orange'])
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

    describe '#each' do
      it 'is an alias of #connect' do
        expect(@signal.method(:each)).to eq @signal.method(:connect)
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

      it 'disconnects the whole chained connections if possible' do
        signal = QML::Reactive::Signal.new [:arg]
        mapped = signal.map { |arg| arg * 2 }

        connection = mapped.connect {}

        connection.disconnect
        expect(signal.connection_count).to eq(0)
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

      context 'when non-variadic' do
        it 'returns the number of arguments' do
          expect(@signal.arity).to eq(2)
        end
      end

      context 'when variadic' do
        it 'returns the number of arguments' do
          expect(@variadic_signal.arity).to eq(-1)
        end
      end
    end

    describe '#parameters' do

      context 'when non-variadic' do
        it 'returns the parameter information' do
          expect(@signal.parameters).to eq([[:req, :foo], [:req, :bar]])
        end
      end

      context 'when variadic' do
        it 'returns the parameter information' do
          expect(@variadic_signal.parameters).to eq([[:rest, :args]])
        end
      end
    end
  end

  describe 'alternatives for #emit' do

    before do
      @signal.connect { |foo, bar| @received = foo + bar }
    end

    describe '#to_proc' do
      it 'returns a Proc that emits the original signal' do
        prc = @signal.to_proc
        prc.call('foo', 'bar')
        expect(prc).to be_a(Proc)
        expect(@received).to eq('foobar')
      end
    end

    describe '#[]' do
      it 'is an alias of #emit' do
        @signal["some", "text"]
        expect(@received).to eq("sometext")
      end
    end

    describe '#call' do
      it 'is an alias of #emit' do
        @signal.call("some", "text")
        expect(@received).to eq("sometext")
      end
    end
  end

  describe 'transformations' do

    describe '#map' do
      it 'returns another Signal that transforms the original signal parameters' do
        received = nil
        @signal
          .map { |foo, bar| foo * bar }
          .connect { |arg| received = arg }
        @signal.emit(20, 30)
        expect(received).to eq(600)
      end
    end

    describe '#select' do
      it 'returns another Signal which is filtered by the given block' do
        count = 0
        @signal.select { |foo| foo % 2 == 0 } .connect { count += 1 }
        10.times { |i| @signal.emit(i, i) }
        expect(count).to eq(5)
      end
    end

    describe '#merge' do
      it 'merges multiple signals' do
        received = []

        signal = QML::Reactive::Signal.new [:foo]
        signal2 = QML::Reactive::Signal.new [:foo]
        signal.merge(signal2).connect { |foo| received << foo }

        signal.emit(:one)
        signal2.emit(:two)
        signal.emit(:three)
        expect(received).to eq([:one, :two, :three])
      end
    end
  end
end


