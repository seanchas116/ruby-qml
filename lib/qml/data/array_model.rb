module QML
  # {ArrayModel} is one of ruby-qml's list models and it stores data in Array simply.
  class ArrayModel < ListModel

    # @param [Array<Symbol|String>] columns
    def initialize(*columns)
      super
      @array = []
    end

    # Duplicates the internal array and returns it.
    # @return [Array]
    def to_a
      @array.dup
    end

    # @return [Integer] the number of the items.
    def count
      @array.count
    end

    # Returns an item.
    # @param [Integer] index the index of the item.
    # @return the item.
    def [](index)
      @array[index]
    end

    # Updates an item.
    # @param [Integer] index
    # @param item
    # @return the item.
    def []=(index, item)
      @array[index] = item
      update(index .. index)
      item
    end

    # Inserts items.
    # @param [Integer] index
    # @return [self]
    def insert(index, *items)
      inserting(index ... index + items.size) do
        @array.insert(index, *items)
      end
      self
    end

    # @overload delete_at(index)
    #   Deletes an item.
    #   @param [Integer] index
    #   @return the deleted item.
    # @overload delete_at(index, count)
    #   Deletes items.
    #   @param [Integer] index
    #   @return [Array] the deleted items.
    def delete_at(index, count = nil)
      if count
        removing(index ... index + count) do
          count.times.map { @array.delete_at(index) }
        end
      else
        removing(index .. index) do
          @array.delete_at(index)
        end
      end
    end

    # Prepend items.
    # @return [self]
    def unshift(*items)
      insert(0, *items)
    end

    # @overload shift
    #   Deletes the first item.
    #   @return the deleted item.
    # @overload shift(count)
    #   Deletes the first items.
    #   @return [Array] the deleted items.
    def shift(count = nil)
      delete_at(0, count)
    end

    # Append items.
    # @return [self]
    def push(*items)
      insert(@array.size, *items)
    end

    alias_method :<<, :push

    # @overload pop
    #   Deletes the last item.
    #   @return the deleted item.
    # @overload pop(count)
    #   Deletes the last items.
    #   @return [Array] the deleted items.
    def pop(count = nil)
      delete_at(@array.size - count, count)
    end

    # Deletes all items.
    # @return [self]
    def clear
      removing(0 ... count) do
        @array.clear
      end
      self
    end

    # Replaces entire array with given array.
    # @param [Array] ary
    # @return [self]
    def replace(ary)
      resetting do
        @array = ary.dup
      end
      self
    end
  end
end
