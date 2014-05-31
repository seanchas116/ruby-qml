require 'mkmf'
require 'pathname'
$LOAD_PATH.unshift Pathname(__FILE__) + '../../../lib'
require 'qml/platform'

qt_include, qt_lib = dir_config('qt')
qt_path =
  if qt_include
    Pathname(qt_include).realpath.parent
  else
    qmake_path = `which qmake`.strip
    fail "qmake not found" if qmake_path == ''
    Pathname(qmake_path).parent.parent
  end

qmake = qt_path + 'bin/qmake'

# build plugins

Pathname(__FILE__).+("../plugins").children.select(&:directory?).each do |dir|
  Dir.chdir(dir) do
    `#{qmake}`
    `make`
  end
end

# build extension itself

RbConfig::CONFIG['CPP'].gsub!(RbConfig::CONFIG['CC'], RbConfig::CONFIG['CXX'])

case
when QML::Platform.mac?
  framework_flag = " -F'#{qt_path + 'lib'}'"
  $CPPFLAGS += framework_flag
  $LDFLAGS += framework_flag
else
  $LDFLAGS += " -L'#{qt_path + 'lib'}'"
end

modules = %w{QtCore QtGui QtQml QtQuick}
modules.each do |mod|
  case
  when QML::Platform.mac?
    $CPPFLAGS += " -I'#{qt_path + "lib/#{mod}.framework/Headers"}'"
    $LDFLAGS += " -framework #{mod}"
  else
    $CPPFLAGS += " -I'#{include_path + mod}'"
    $LDFLAGS += " -l#{mod.downcase}"
  end
end

headers = %w{
  QtCore/QObject
  QtCore/QVariant
  QtGui/QGuiApplication
  QtQml/QQmlEngine
  QtQml/QQmlComponent
  QtQml/QQmlContext
}
headers.each do |h|
  fail "header not found: #{h}" unless have_header(h)
end

$CPPFLAGS += ' -std=c++11 -Wall -Wextra -g'

create_makefile "qml/qml"
