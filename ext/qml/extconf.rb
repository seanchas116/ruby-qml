require 'mkmf'
require 'pathname'

qt_include, qt_lib = dir_config('qt')
qmake = qt_include ? Pathname(qt_include).realpath.parent.+('bin/qmake') : 'qmake'

RbConfig::CONFIG['CPP'].gsub!(RbConfig::CONFIG['CC'], RbConfig::CONFIG['CXX'])

headers = ['QtQuick/QQuickView']
headers.each do |header|
  fail "header not found: #{header}" unless have_header(header)
end

# build plugins

Pathname(__FILE__).+("../plugins").children.select(&:directory?).each do |dir|
  Dir.chdir(dir) do
    `#{qmake}`
    `make`
  end
end

# build extension itself

$CPPFLAGS += ' -std=c++11 -Wall -Wextra -g'

# TODO: add support for other platforms than Mac
$LDFLAGS += " -F#{qt_lib} -framework QtCore -framework QtTest"

create_makefile "qml/qml"
