# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'qml/version'

Gem::Specification.new do |spec|
  spec.name          = "qml"
  spec.version       = QML::VERSION
  spec.authors       = ["Ryohei Ikegami"]
  spec.email         = ["iofg2100@gmail.com"]
  spec.summary       = %q{A QML / Qt Quick wrapper for Ruby}
  spec.description   = "ruby-qml provides bindings between QML and Ruby and enables you to use Qt Quick-based GUI from Ruby."
  spec.homepage      = "https://github.com/seanchas116/ruby-qml"
  spec.license       = "MIT"

  spec.files         = `git ls-files -z`.split("\x0")
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]
  spec.extensions = ["ext/qml/extconf.rb"]

  spec.add_development_dependency "bundler", "~> 1.5"
  spec.add_development_dependency "rake", "~> 10.3"
  spec.add_development_dependency "rspec", "~> 3.0"
  spec.add_development_dependency 'pry'
  spec.add_development_dependency 'celluloid'
  spec.add_development_dependency 'twitter'
end
