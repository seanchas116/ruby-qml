require 'logger'
require 'naught'

module QML
  NullLogger = Naught.build { |config| config.mimic Logger }
end
