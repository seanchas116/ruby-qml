require 'mkmf'
require 'pathname'

qmake = with_config('qmake') || find_executable('qmake')

qmlbind_dir = Pathname.pwd + 'lib/libqmlbind/qmlbind'

Dir.chdir(qmlbind_dir) do
  system "#{qmake} && make" or fail "Failed to make libqmlbind"
end

$LDFLAGS << " -L#{qmlbind_dir} -lqmlbind"
$CPPFLAGS << " -I#{qmlbind_dir + 'include'}"

create_makefile 'qml/qml'
