require 'spec_helper'

def convert(value)
  QML::TestUtil.echo_conversion(value)
end

describe 'Conversion between C++ and Ruby' do

  shared_examples 'conversion' do
    it 'works' do
      actual = convert(@expected)
      expect(actual).to eq @expected
    end
  end

  samples = {
    'Integer' => 1234,
    'true' => true,
    'false' => false,
    'nil' => nil,
    'Float' => 3.1416,
    'String' => 'hoge',
    'non-ascii String' => 'ほげ',
    'Array' => [1, "foobar", true],
    'Hash' => {'one' => 1, 'two' => 2, '三' => 3}
  }

  samples.each do |k, v|
    it "can convert #{k}" do
      actual = convert(v)
      expect(actual).to eq v
    end
  end

  it 'can convert Symbol as String' do
    expected = :hogehoge
    actual = convert(expected)
    expect(actual).to eq expected.to_s
  end

end
