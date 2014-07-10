require 'qml/data/error'
require 'qml/wrappable'

module QML
  module Data
    class ListModel
      include ::Enumerable
      include Wrappable

      class << self
        def column(*columns)
          @columns ||= []
          @columns |= columns
        end

        attr_reader :columns
      end

      attr_reader :qt_models

      def initialize
        fail Error, 'columns not specified' unless self.class.columns
        @qt_models = []
      end

      def each
        return to_enum unless block_given?
        count.times do |i|
          yield self[i]
        end
      end

      def count
        fail ::NotImplementedError
      end

      def [](index)
        fail ::NotImplementedError
      end

      protected

      def update(range)
        @qt_models.each do |qt_model|
          qt_model.update(range.min, range.max)
        end
      end

      def moving(range, destination, &block)
        @qt_models.each do |qt_model|
          qt_model.begin_move(range.min, range.max, destination)
        end

        ret = block.call

        @qt_models.each do |qt_model|
          qt_model.end_move
        end

        ret
      end

      def inserting(range, &block)
        @qt_models.each do |qt_model|
          qt_model.begin_insert(range.min, range.max)
        end

        ret = block.call

        @qt_models.each do |qt_model|
          qt_model.end_insert
        end

        ret
      end

      def removing(range, &block)
        @qt_models.each do |qt_model|
          qt_model.begin_remove(range.min, range.max)
        end

        ret = block.call

        @qt_models.each do |qt_model|
          qt_model.end_remove
        end

        ret
      end
    end
  end
end
