require 'spec_helper'

describe QML::Wrappable do
  describe '#create_wrapper' do
    it 'is not implemented by default' do
      obj = Class.new { include QML::Wrappable }.new
      expect { obj.create_wrapper }.to raise_error(NotImplementedError)
    end
  end
end
