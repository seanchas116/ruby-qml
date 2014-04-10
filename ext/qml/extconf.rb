require 'mkmf'
require 'pathname'

qt_include, qt_lib = dir_config('qt')
qmake_path = qt_include ? Pathname(qt_include).realpath + "../bin/qmake" : "qmake"

create_makefile "qml/clib"
`#{qmake_path} ruby-qml.pro`
