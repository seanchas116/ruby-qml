require 'pathname'
require 'qml/platform'

module QML
  # {PluginLoader} loads Qt C++ plugins and enables you to use your Qt C++ codes from Ruby easily.
  # @see http://qt-project.org/doc/qt-5/qpluginloader.html QPluginLoader (C++)
  class PluginLoader

    # @overload initialize(path)
    #   @param [String|Pathname] path the library path (may be platform-dependent).
    #   @example
    #      loader = QML::PluginLoader.new('path/to/libhoge.dylib')
    # @overload initialize(dir, libname)
    #   @param [String|Pathname] dir the library directory.
    #   @param [String] libname the platform-independent library name.
    #   @example
    #      loader = QML::PluginLoader.new('path/to', 'hoge')
    def initialize(path, libname = nil)
      path = Pathname(path) + self.class.lib_filename(libname) if libname
      initialize_impl(path.to_s)
    end

    alias_method :instance, :load

    def self.lib_filename(libname)
      case
      when Platform::windows?
        "#{libname}.dll"
      when Platform::mac?
        "lib#{libname}.dylib"
      else
        "lib#{libname}.so"
      end
    end
  end
end
