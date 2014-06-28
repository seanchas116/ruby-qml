module QML
  module Data
    class ArrayModel < ListModel
      def initialize
        super()
        @array = []
      end

      def to_a
        @array.dup
      end

      def count
        @array.count
      end

      def [](index)
        @array[index]
      end

      def []=(index, value)
        @array[index] = value
        update(index ... index + 1)
      end

      def insert(index, *items)
        inserting(index ... index + items.size) do
          @array.insert(index, *items)
        end
        self
      end

      def delete_at(index, count = 1)
        removing(index ... index + count) do
          if count == 1
            @array.delete_at(index)
          else
            count.times.map { @array.delete_at(index) }
          end
        end
      end

      def unshift(*items)
        insert(0, *items)
      end

      def shift(count = 1)
        delete_at(0, count)
      end

      def push(*items)
        insert(@array.size, *items)
      end

      alias_method :<<, :push

      def pop(count = 1)
        delete_at(@array.size - count, count)
      end
    end
  end
end
