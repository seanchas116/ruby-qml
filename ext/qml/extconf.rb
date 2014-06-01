require 'mkmf'
require 'pathname'

pkgconfig = with_config('pkg-config') || find_executable('pkg-config')
abort 'pkg-config executable not found' unless pkgconfig

qt_include, qt_lib = dir_config('qt')
qt_path = qt_include && Pathname(qt_include).parent.realpath

if qt_path
  ENV['PKG_CONFIG_PATH'] = "#{ENV['PKG_CONFIG_PATH']}:#{qt_path + 'lib/pkgconfig'}"
end

qmake = qt_path ? qt_path + 'bin/qmake' : find_executable('qmake')
abort 'qmake executable not found' unless qmake

# build plugins

Pathname(__FILE__).+("../plugins").children.select(&:directory?).each do |dir|
  Dir.chdir(dir) do
    `#{qmake}`
    `make`
  end
end

# build extension itself

RbConfig::CONFIG['CPP'].gsub!(RbConfig::CONFIG['CC'], RbConfig::CONFIG['CXX'])

def add_cppflags(flags)
  $CPPFLAGS += " #{flags}"
end

def add_ldflags(flags)
  $LDFLAGS += " #{flags}"
end

%w{Qt5Core Qt5Gui Qt5Qml Qt5Quick}.each do |mod|
  add_cppflags `#{pkgconfig} --cflags #{mod}`.chomp
  add_ldflags `#{pkgconfig} --libs #{mod}`.chomp
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
  abort "header not found: #{h}" unless have_header(h)
end

add_cppflags '-std=c++11 -Wall -Wextra -g'

create_makefile "qml/qml"
