require 'spec_helper'

describe QML::JSArray do

  let(:engine) { QML::Engine.new }
  let(:function_script) do
    <<-JS
      (function(a, b) {
        return a + b;
      });
    JS
  end
  let(:function) { engine.evaluate(function_script) }

  describe '#call' do
    it 'calls function' do
      expect(function.call(1,2)).to eq 3
    end
  end
end
