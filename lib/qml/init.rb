module QML

  # @return [Boolean] whether {#init} is already called.
  def initialized?
    Kernel.initialized?
  end

  # Initializes ruby-qml.
  # @param [Hash] opts
  # @option opts [Boolean] :offscreen (false) set this to true to run application offscreen (without GUI)
  def init(opts = {})
    opts = {offscreen: false}.merge opts
    fail AlreadyInitializedError, "ruby-qml already initialized" if initialized?
    argv = [$PROGRAM_NAME]
    argv += %w{-platform offscreen} if opts[:offscreen]
    Kernel.init(argv)
    @on_init.each(&:call)
  end

  @on_init = []

  # Registers a block to be called just after {#init} is called.
  # @yield
  def on_init(&block)
    @on_init << block
  end

  module_function :initialized?, :init, :on_init
end
