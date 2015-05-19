module QML
  # {ListModel} is the base class of list models which provides data to QML list views.
  # @see http://qt-project.org/doc/qt-5/qabstractitemmodel.html QAbstractItemModel (C++)
  # @see http://qt-project.org/doc/qt-5/qabstractlistmodel.html QAbstractListModel (C++)
  class ListModel
    include Enumerable

    # @return [Array<Symbol|String>]
    attr_reader :columns

    # @param [Array<Symbol|String>] columns the column names of the model.
    def initialize(*columns)
      @columns = columns
      @access = ListModelAccess.create(self)
      @qml_model = QML::Plugins.rubyqml.createListModel(@access)
    end

    # Iterates each item.
    # @overload each
    #   @return [Enumerator]
    # @overload each
    #   @yield [item]
    #   @return [self]
    def each
      return to_enum unless block_given?
      count.times do |i|
        yield self[i]
      end
      self
    end

    # @abstract
    # @return [Integer] the number of the items.
    def count
      fail ::NotImplementedError
    end

    # Returns an item.
    # @abstract
    # @param [Integer] index the index of the item.
    # @return the item.
    def [](index)
      fail ::NotImplementedError
    end

    # @return [QML::JSValue]
    def to_qml
      @qml_model
    end

    protected

    # Notifies the list views that the data of the items was changed.
    # @param [Range<Integer>] range the index range of changed items.
    def update(range)
      @access.update(range.min, range.max)
    end

    # Notifies the list views that items are about to be and were moved.
    # @param [Range<Integer>] range the index range of the item being moved.
    # @param [Integer] destination the first index of the items after moved.
    # @yield the block that actually do moving operation of the items.
    # @return the result of given block.
    # @see http://qt-project.org/doc/qt-5/qabstractitemmodel.html#beginMoveRows QAbstractItemModel::beginMoveRows
    # @see http://qt-project.org/doc/qt-5/qabstractitemmodel.html#endMoveRows QAbstractItemModel::endMoveRows
    # @see #inserting
    # @see #removing
    def moving(range, destination)
      return if range.count == 0

      @access.begin_move(range.min, range.max, destination)
      ret = yield
      @access.end_move
      ret
    end

    # Notifies the list views that items are about to be and were inserted.
    # @param [Range<Integer>] range the index range of the items after inserted.
    # @yield the block that actually do insertion of the items.
    # @return the result of give block.
    # @example
    #   inserting(index ... index + items.size) do
    #     @array.insert(index, *items)
    #   end
    # @see http://qt-project.org/doc/qt-5/qabstractitemmodel.html#beginInsertRows QAbstractItemModel::beginInsertRows
    # @see http://qt-project.org/doc/qt-5/qabstractitemmodel.html#endInsertRows QAbstractItemModel::endInsertRows
    # @see #removing
    # @see #moving
    def inserting(range, &block)
      return if range.count == 0

      @access.begin_insert(range.min, range.max)
      ret = yield
      @access.end_insert
      ret
    end

    # Notifies the list views that items are about to be and were removed.
    # @param [Range<Integer>] range the index range of the items before removed.
    # @yield the block that actually do removal of the items.
    # @return the result of give block.
    # @see http://qt-project.org/doc/qt-5/qabstractitemmodel.html#beginRemoveRows QAbstractItemModel::beginRemoveRows
    # @see http://qt-project.org/doc/qt-5/qabstractitemmodel.html#endRemoveRows QAbstractItemModel::endRemoveRows
    # @see #inserting
    # @see #moving
    def removing(range, &block)
      return if range.count == 0

      @access.begin_remove(range.min, range.max)
      ret = yield
      @access.end_remove
      ret
    end

    def resetting(&block)
      @access.begin_reset
      ret = yield
      @access.end_reset
      ret
    end
  end
end
