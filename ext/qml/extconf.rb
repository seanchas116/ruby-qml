require 'mkmf'
require 'pathname'

class Configurator

  PKGS = %w{Qt5Core Qt5Gui Qt5Widgets Qt5Qml Qt5Quick libffi}
  HEADERS = %w{QObject QVariant QApplication QQmlEngine QQmlComponent QQmlContext ffi.h}
  CLASSES = %w{listmodel}

  def initialize
    find_dependencies
    @cppflags = []
    @ldflags = []
    @debug_enabled = enable_config('debug')
  end

  def find_dependencies
    @pkgconfig = with_config('pkg-config') || find_executable('pkg-config')
    abort 'pkg-config executable not found' unless @pkgconfig

    @qt_path = with_config('qt-dir')
    @qt_path &&= Pathname(@qt_path).realpath

    @ffi_path = with_config('libffi-dir')
    @ffi_path &&= Pathname(@ffi_path).realpath

    [@qt_path, @ffi_path].compact.each do |path|
      ENV['PKG_CONFIG_PATH'] = "#{path + 'lib/pkgconfig'}:#{ENV['PKG_CONFIG_PATH']}"
    end

    @qmake = @qt_path ? @qt_path + 'bin/qmake' : find_executable('qmake')
    @moc = @qt_path ? @qt_path + 'bin/moc' : find_executable('moc')
    abort 'qmake executable not found' unless @qmake
    abort 'moc executable not found' unless @moc
  end

  def build_plugins
    puts "building plugins..."
    qmake_opts = @debug_enabled ? 'CONFIG+=debug' : ''
    Pathname(__FILE__).+("../plugins").children.select(&:directory?).each do |dir|
      Dir.chdir(dir) do
        system("#{@qmake} #{qmake_opts}") or abort "failed to configurate plugin: #{dir.basename}"
        system('make clean') if enable_config('clean-plugin', false)
        system('make') or abort "failed to build plugin: #{dir.basename}"
      end
    end
  end

  def configure_extension
    RbConfig::CONFIG['CPP'].gsub!(RbConfig::CONFIG['CC'], RbConfig::CONFIG['CXX'])

    PKGS.each do |mod|
      @cppflags << `#{@pkgconfig} --cflags #{mod}`.chomp

      libs =`#{@pkgconfig} --libs #{mod}`.chomp
      @ldflags << libs

      # add framework search path to cppflags
      libs.split.each do |lib|
        @cppflags << lib if /^-F/ =~ lib
      end
    end

    # add private header directory of QtCore
    qtversion = `#{@pkgconfig} --modversion Qt5Core`.chomp
    `#{@pkgconfig} --cflags-only-I Qt5Core`.split.map { |i| Pathname(i.gsub("-I", "")) }.each do |dir|
      private_dir = dir + "#{qtversion}/QtCore"
      @cppflags << "-I#{private_dir}" if private_dir.exist?
    end

    # include framework headers in Mac
    # (Qt official installer for Mac does not install header files in /include directory)
    if /darwin/ =~ RUBY_PLATFORM
      PKGS.map { |pkg| pkg.gsub(/^Qt5/, 'Qt') }.each do |framework|
        @cppflags << "-I#{@qt_path + "lib/#{framework}.framework/Headers"}"
      end
      # add private header directory of QtCore
      @cppflags << "-I#{@qt_path}/lib/QtCore.framework/Headers/#{qtversion}/QtCore"
    end

    $CPPFLAGS += " #{@cppflags.join(" ")}"
    $LDFLAGS += " #{@ldflags.join(" ")}"
    $CPPFLAGS += " -fPIC"

    HEADERS.each do |h|
      abort "header not found: #{h}" unless have_header(h)
    end

    unless have_func('rb_thread_call_with_gvl') && have_func('rb_thread_call_without_gvl')
      abort "rb_thread_call_with_gvl and rb_thread_call_without_gvl not found"
    end
    $CPPFLAGS += " -DHAVE_RUBY_THREAD_H" if have_header('ruby/thread.h')

    $CPPFLAGS += " -std=c++11 -Wall -Wextra -pipe -Wno-reserved-user-defined-literal"
    if @debug_enabled
      $CPPFLAGS += " -O0 -ggdb3"
    else
      $CPPFLAGS += " -O3"
    end

    CLASSES.each do |header|
      File.write("moc_#{header}.cpp", `#{@moc} #{header}.h`)
    end

    create_makefile 'qml/qml'
  end
end

configurator = Configurator.new
configurator.build_plugins
configurator.configure_extension
