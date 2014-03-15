require 'spec_helper'

describe QML::TestObject do
  before do
    @obj = QML::TestObject.new
  end
  describe '#normalMethod' do
    it 'should call the method' do
      expect(@obj.normalMethod(1, "23")).to eq(24)
    end
    context 'if the arguments are not convertible' do
      it 'should raise error' do
        expect { @obj.normalMethod([1, 2], 'bar') }.to raise_error(ArgumentError)
      end
    end
    context 'if the number of arguments is wrong' do
      it 'should raise error' do
        expect { @obj.normalMethod(1) }.to raise_error(ArgumentError)
      end
    end
  end
  describe '#variantMethod' do
    it 'should call the method with QVariant arguments' do
      expect(@obj.variantMethod(20, [1, 3, 4, 9])).to eq(37)
    end
  end
  describe '#overloadedMethod' do
    it 'should call the method with the same arity' do
      expect(@obj.overloadedMethod('foo', 1)).to eq('2 params')
    end
    context 'when multiple methods with the same arity are provided' do
      it 'should call the method defined last' do
        expect(@obj.overloadedMethod('hoge')).to eq('last')
      end
    end
  end
  describe '#emitSomeSignal' do
    it 'should emit the signal' do
      spy = Ropework::SignalSpy.new(@obj.someSignal)
      @obj.emitSomeSignal('poyopoyo')
      expect(spy.args.first).to eq(['poyopoyo'])
    end
  end
  describe 'name property' do
    it 'should be set' do
      @obj.name = 'abcd'
      expect(@obj.getName).to eq('abcd')
    end
    it 'should notify when changed' do
      spy = Ropework::SignalSpy.new(@obj.name_changed)
      @obj.name = 'hogepoyo'
      expect(spy.args.first).to eq(['hogepoyo'])
    end
  end
  describe 'enums' do
    it 'should be set' do
      expect(QML::TestObject::Apple).to eq(0)
      expect(QML::TestObject::Banana).to eq(1)
      expect(QML::TestObject::Orange).to eq(2)
    end
  end
end