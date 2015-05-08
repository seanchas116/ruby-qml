require 'mkmf'
require 'pathname'
require '../../lib/qml/platform'

# find qmake

qmake = with_config('qmake') || find_executable('qmake')
debug_enabled = enable_config('debug')

# build libqmlbind

qmlbind_dir = Pathname.pwd + 'lib/libqmlbind/qmlbind'

Dir.chdir(qmlbind_dir) do
  system "#{qmake} && make" or fail "Failed to make libqmlbind"
end

case
when QML::Platform.mac?
  lib_path = qmlbind_dir + 'libqmlbind.dylib'
  system "install_name_tool -id #{lib_path} #{lib_path}"
when QML::Platform.windows?
  # TODO
else
  $LDFLAGS << " -Wl,-rpath #{qmlbind_dir}"
end

$LDFLAGS << " -L#{qmlbind_dir} -lqmlbind"
$CPPFLAGS << " -I#{qmlbind_dir + 'include'}"

$CPPFLAGS << " -g" if debug_enabled

# create makefile

create_makefile 'qml/qml'
