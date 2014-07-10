require 'spec_helper'

describe "test object" do

  let(:plugin) { QML::Plugins.test_util }
  let(:obj) { plugin.createTestObject }

  describe '#normalMethod' do

    it 'should call the method' do
      expect(obj.normalMethod(1, "23")).to eq(24)
    end

    context 'if the arguments are not convertible' do
      it 'should raise error' do
        expect { obj.normalMethod([1, 2], 'bar') }.to raise_error QML::MethodError
      end
    end

    context 'if the number of arguments is wrong' do
      it 'should raise error' do
        expect { obj.normalMethod(1) }.to raise_error QML::MethodError
      end
    end
  end

  describe '#normal_method' do
    it 'is same as #normalMethod' do
      expect(obj.normalMethod(1, "23")).to eq(24)
    end
  end

  describe '#variantMethod' do

    it 'should call the method with QVariant arguments' do
      expect(obj.variantMethod(20, [1, 3, 4, 9])).to eq(37)
    end
  end

  describe '#overloadedMethod' do

    it 'should call the method with the same arity' do
      expect(obj.overloadedMethod('foo', 1)).to eq('2 params')
    end

    context 'when multiple methods with the same arity are provided' do

      it 'should call the method defined last' do
        expect(obj.overloadedMethod('hoge')).to eq('last')
      end
    end
  end

  describe '#someSignal signal' do

    it 'should emit the signal' do
      spy = QML::Reactive::SignalSpy.new(obj.someSignal)
      obj.emitSomeSignal('poyopoyo')
      expect(spy.args.first).to eq(['poyopoyo'])
    end
  end

  describe '#some_signal signal' do

    it 'is same as #someSignal' do
      spy = QML::Reactive::SignalSpy.new(obj.some_signal)
      obj.emit_some_signal('poyopoyo')
      expect(spy.args.first).to eq(['poyopoyo'])
    end
  end

  describe '#throwingMethod' do
    it 'raises a QML::CxxError' do
      expect { obj.throwingMethod }.to raise_error(QML::CppError, "<std::runtime_error> test error")
    end
  end

  describe '#metaObjectMethod' do
    it 'returns the QML::MetaObject of the test object' do
      expect(obj.metaObjectMethod).to eq obj.class.meta_object
    end
  end

  describe '#selfReturningMethod' do
    it 'returns the same Ruby object' do
      expect(obj.selfReturningMethod).to be obj
    end
  end

  describe '#name property' do

    it 'should be set' do
      obj.name = 'abcd'
      expect(obj.getName).to eq('abcd')
    end

    it 'should notify when changed' do
      spy = QML::Reactive::SignalSpy.new(obj.name_changed)
      obj.name = 'hogepoyo'
      expect(spy.args.first).to eq(['hogepoyo'])
    end

    context 'with wrong type' do

      it 'raise error' do
        expect { obj.name = 123 }.to raise_error QML::PropertyError
      end
    end
  end

  describe '#someValue property' do
    context 'as #some_value' do
      it 'can be set and get' do
        obj.some_value = 123
        expect(obj.some_value).to eq 123
      end
    end
  end

  describe 'enums' do
    
    it 'should be set' do
      k = obj.class
      expect(k::Foo).to eq(0)
      expect(k::Bar).to eq(1)
      expect(k::FooBar).to eq(2)
      expect(k::FOO_BAR).to eq(2)
      expect(k::Enums).to match_array [0,1,2]
    end

    it 'can be set and get' do
      obj.enum_value = obj.class::FOO_BAR
      expect(obj.enum_value).to eq(obj.class::FOO_BAR)
    end
  end

  describe 'Wrapper#inspect' do
    it 'inspects the class name and property values' do
      expect(obj.inspect).to eq %{#<[class for RubyQml::TestObject]:#{obj.__id__} enumValue=0 name="" objectName="" someValue=0.0>}
    end
  end

  context 'in subclass' do
    let(:subobj) { plugin.createTestObject }
    it "can call TestObject's method" do
      expect(subobj.normalMethod(1, "23")).to eq(24)
    end
  end

  context 'in non-main threads' do

    require 'celluloid'

    class TestCaller
      include Celluloid

      def initialize(obj)
        @obj = obj
      end

      def call(method, *args)
        @obj.public_send(method, *args)
      rescue => e
        e
      end
    end

    let(:test_caller) { TestCaller.new(obj) }

    describe '#normal_method' do
      it 'fails' do
        expect(test_caller.call(:normal_method, 1, "23")).to be_a QML::InvalidThreadError
      end
    end
    describe '#name' do
      it 'fails' do
        expect(test_caller.call(:name)).to be_a QML::InvalidThreadError
      end
    end
    describe '#name=' do
      it 'fails' do
        expect(test_caller.call(:name=, "hoge")).to be_a QML::InvalidThreadError
      end
    end
  end
end
