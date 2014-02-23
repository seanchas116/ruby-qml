require 'spec_helper'

describe QML do
  it 'should have a version number' do
    QML::VERSION.should_not be_nil
  end
end
