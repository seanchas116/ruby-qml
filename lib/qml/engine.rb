module QML
  # @!parse class Engine < QtObjectBase; end

  # {Engine} provides a QML engine.
  #
  # @see http://qt-project.org/doc/qt-5/qqmlengine.html QQmlEngine (C++)
  class Engine

    def evaluate(str, file = '<in QML::Engine#evaluate>', lineno = 1)
      evaluate_impl(str, file, lineno).tap do |result|
        raise result.to_error if result.is_a?(JSObject) && result.error?
      end
    end

  end
end
