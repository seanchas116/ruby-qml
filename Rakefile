require "bundler/gem_tasks"
require "rspec/core/rake_task"
require 'rake/extensiontask'

RSpec::Core::RakeTask.new(:spec)
Rake::ExtensionTask.new('qml') do |ext|
  ext.lib_dir = 'lib/qml'
  ext.config_options << "--with-qt-dir=#{ENV['qt_dir']}" if ENV['qt_dir']
end

task :default => :spec
