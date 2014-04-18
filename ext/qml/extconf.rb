require 'mkmf'
require 'pathname'

qt_include, qt_lib = dir_config('qt')

# headers = ['QtQuick/QQuickView']
# headers.each do |header|
#   fail "header not found: #{header}" unless have_header(header)
# end

create_makefile "qml/clib"
