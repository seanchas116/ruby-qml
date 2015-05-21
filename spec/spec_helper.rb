require 'coveralls'
Coveralls.wear!

$LOAD_PATH.unshift File.expand_path('../../lib', __FILE__)
require 'qml'

Dir.glob(Pathname(__FILE__) +'../shared/**/*.rb') do |f|
  require f
end

QML.init(%w{-platform offscreen})
