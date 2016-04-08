module QML
  module Interface

    def self.call_method(obj, name, args)
      begin
        obj.__send__ name, *args
      rescue => error
        notify_error(error)
        nil
      end
    end

    # Called when an Ruby error is occured in executing Qt code.
    # @param error The error (or the exception)
    def self.notify_error(error)
      warn "-- An error occured when running Ruby code from Qt --"
      warn "#{error.class.name}: #{error.message}"
      warn "Backtrace: \n\t#{error.backtrace.join("\n\t")}"
    end
  end
end
