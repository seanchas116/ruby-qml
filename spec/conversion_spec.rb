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

  it 'can convert Time' do
    time = Time.now
    result = convert(time)
    puts time
    puts result

    expect(result.year).to eq(time.year)
    expect(result.month).to eq(time.month)
    expect(result.day).to eq(time.day)
    expect(result.hour).to eq(time.hour)
    expect(result.min).to eq(time.min)
    expect(result.sec).to eq(time.sec)
    expect(result.nsec / 1_000_000).to eq(time.nsec / 1_000_000)
    expect(result.gmt_offset).to eq(result.gmt_offset)
  end

end
