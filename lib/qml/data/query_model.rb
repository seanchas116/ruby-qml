module QML
  # {QueryModel} provides a list model implementation with database backends like ActiveRecord.
  class QueryModel < ListModel
    attr_reader :count

    # @param [Array<Symbol|String>] columns
    def initialize(*columns)
      super
      @count = 0
      @caches = []
      update
    end

    def [](index)
      block_index = index / CACHE_SIZE
      cache = @caches.find { |c| c.block_index == block_index } || add_cache(block_index)
      cache.items[index % CACHE_SIZE]
    end

    # Updates the model.
    def update
      @caches = []
      resetting do
        @count = query_count
      end
    end

    # @abstract
    # Queries the count of the records.
    # Called when {#update} is called and the result is set as the {#count} of the model.
    # @return [Integer]
    def query_count
      fail ::NotImplementedError
    end

    # @abstract
    # Queries a block of records. The results are chached.
    # @param [Integer] offset
    # @param [Integer] count
    # @return [Array]
    def query(offset, count)
      fail ::NotImplementedError
    end

    private

    Cache = Struct.new(:block_index, :items)
    CACHE_SIZE = 256
    CACHE_COUNT = 4

    def add_cache(block_offset)
      @caches.shift if @caches.size >= CACHE_COUNT
      Cache.new(block_offset, query(block_offset * CACHE_SIZE, CACHE_SIZE)).tap do |cache|
        @caches << cache
      end
    end
  end
end
