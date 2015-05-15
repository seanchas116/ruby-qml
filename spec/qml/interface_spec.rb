require 'spec_helper'

describe QML::Interface do

  describe '.notify_error' do
    let(:error) do
      begin
        fail "hoge"
      rescue => e
        e
      end
    end

    it 'prints an error to stderr' do
      expect { QML::Interface.notify_error(error) }
        .to output(/#{error.message}/).to_stderr
    end
  end

end
