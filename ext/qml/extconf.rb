require 'mkmf'
require 'pathname'

pkgconfig = with_config('pkg-config') || find_executable('pkg-config')
abort 'pkg-config executable not found' unless pkgconfig

qt_path = with_config('qt-dir')
qt_path &&= Pathname(qt_path).realpath

ffi_path = with_config('ffi-dir')
ffi_path &&= Pathname(ffi_path).realpath

[qt_path, ffi_path].reject(&:nil?).each do |path|
  ENV['PKG_CONFIG_PATH'] = "#{path + 'lib/pkgconfig'}:#{ENV['PKG_CONFIG_PATH']}"
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

%w{Qt5Core Qt5Gui Qt5Qml Qt5Quick libffi}.each do |mod|
  add_cppflags `#{pkgconfig} --cflags #{mod}`.chomp
  add_ldflags `#{pkgconfig} --libs #{mod}`.chomp
end

qtversion = `#{pkgconfig} --modversion Qt5Core`.chomp
`#{pkgconfig} --cflags-only-I Qt5Core`.split.map { |i| Pathname(i.gsub("-I", "")) }.each do |dir|
  private_dir = dir + "#{qtversion}/QtCore"
  add_cppflags "-I#{private_dir}" if private_dir.exist?
end


headers = %w{
  QtCore/QObject
  QtCore/QVariant
  QtGui/QGuiApplication
  QtQml/QQmlEngine
  QtQml/QQmlComponent
  QtQml/QQmlContext
  ffi.h
}
headers.each do |h|
  abort "header not found: #{h}" unless have_header(h)
end

add_cppflags '-std=c++11 -Wall -Wextra -g -pipe'

create_makefile "qml/qml"
