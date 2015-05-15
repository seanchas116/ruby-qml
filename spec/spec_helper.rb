require 'coveralls'
Coveralls.wear!

$LOAD_PATH.unshift File.expand_path('../../lib', __FILE__)
require 'qml'
QML.init(%w{--platform offscreen})

Dir.glob(Pathname(__FILE__) +'../shared/**/*.rb') do |f|
  require f
end
