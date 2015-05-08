require 'coveralls'
Coveralls.wear!

$LOAD_PATH.unshift File.expand_path('../../lib', __FILE__)
require 'qml'
$application = QML::Application.new(%w{platform offscreen})

# QML::TestUtil.gc_debug_message_enabled = true

Dir.glob(Pathname(__FILE__) +'../shared/**/*.rb') do |f|
  require f
end
