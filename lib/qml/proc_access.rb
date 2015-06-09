module QML
  class ProcAccess
    include Access
    register_to_qml under: 'RubyQml', version: QML::VERSION

    def call(args)
      @proc.call(*args)
    end

    # @return [QML::JSFunction]
    def self.wrap_proc(prc)
      @bind_call ||= QML.engine.evaluate <<-JS
        (function (access) { return access.call.bind(access); })
      JS

      @component ||= QML::Component.new(data: <<-QML)
        import RubyQml 1.0
        ProcAccess {}
      QML

      access = @component.create
      access.unwrap.instance_eval do
        @proc = prc
      end
      @bind_call.call(access)
    end
  end
end
