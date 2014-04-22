require 'pathname'
require 'qml/platform'

module QML
  class PluginLoader

    module Extension

      def initialize(dirname, libname)
        dirname = Pathname.new(dirname)
        dirname += lib_filename(libname)
        super(dirname.to_s)
      end

      private

      def lib_filename(libname)
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

    prepend Extension

  end
end