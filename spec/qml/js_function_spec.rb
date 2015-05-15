require 'spec_helper'

describe QML::JSFunction do

  let(:function) do
    QML.engine.evaluate <<-JS
      (function(a, b) {
        return a + b;
      });
    JS
  end

  let(:constructor) do
    QML.engine.evaluate <<-JS
      (function(a, b) {
        this.value = a + b;
      });
    JS
  end

  describe '#call' do
    it 'calls function' do
      expect(function.call(1,2)).to eq 3
    end
  end

  describe '#new' do
    it 'calls function as a constructor' do
      expect(constructor.new(1, 2).value).to eq 3
    end
  end
end
