/*
    This file is part of LibQtLua.

    LibQtLua is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LibQtLua is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with LibQtLua.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (C) 2010, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/


#ifndef QTLUA_TABLEMODEL_HH_
#define QTLUA_TABLEMODEL_HH_

#include <QtLua/Value>
#include <QAbstractItemModel>

namespace QtLua {

  class Table;

  /**
   * @short Qt Model/View lua table model class
   * @header QtLua/TableModel
   * @module {Model/View}
   *
   * This class can be used to expose lua tables content to Qt view
   * widgets in a flat (list) or hierarchical (tree) manner.
   *
   * Lua tables and @ref UserData objects with valid table operations
   * are handled.
   *
   * Lua tables can be edited from Qt views using this model. The
   * @ref Attribute flags can be used to finely control which editing
   * actions are allowed. User input may be evaluated as a lua
   * expression when editing a table entry.
   *
   * Changes in exposed lua tables may @b not update the model on the
   * fly and the @ref update function must be called to refresh on
   * heavy modifications. This is partially due to lack of lua
   * mechanism to implement efficient table change event.
   *
   * Usage example:
   * @example examples/cpp/mvc/tabletreeview.cc:1
   * @see TableDialog
   */

  class TableModel : public QAbstractItemModel
  {
    Q_OBJECT;
    Q_ENUMS(Attribute);
    Q_FLAGS(Attributes);

  public:

    enum Attribute
      {
	Recursive = 1,		//< Expose nested tables too.
	UserDataIter = 2,	//< Iterate over UserData objects too.
	HideType = 4,		//< Do not show entry type in an additionnal column.
	UnquoteKeys = 8,	//< Strip double quote from string keys

	Editable = 16,		//< Allow editing exposed tables using views.
	EditFixedType = 32,	//< Prevent value type change when editing.
	EditLuaEval = 64,	//< Evaluate user input as a lua expression.
	EditInsert = 128,	//< Allow insertion of new entries.
	EditRemove = 256,	//< Allow deletion of existing entries.
	EditKey = 512,	        //< Allow entry key update.
	EditAll = 16 + 128 + 256 + 512, //< Editable, EditInsert, EditRemove and EditKey allowed)
      };

    Q_DECLARE_FLAGS(Attributes, Attribute);

    /** Create a new lua table model. */
    TableModel(const Value &root, QObject *parent = 0,
	       Attributes attr = Attributes(Recursive | UserDataIter));

    ~TableModel();

    /** Clear cached table content and reset model. */
    void update();

    /** Columns ids */
    enum ColumnId
      {
	ColKey = 0, ColValue = 1, ColType = 2,
      };

    /** Get lua value at given model index */
    Value get_value(const QModelIndex &index) const;

    /** Get supported operations for given model index */
    Attributes get_attr(const QModelIndex &index) const;

  private:
    QModelIndex	index(int row, int column, const QModelIndex &parent) const;
    QModelIndex	parent(const QModelIndex &index) const;
    int		rowCount(const QModelIndex &parent) const;
    bool	hasChildren(const QModelIndex & parent) const;
    int		columnCount(const QModelIndex &parent) const;
    QVariant	data(const QModelIndex &index, int role) const;
    QVariant	headerData(int section, Qt::Orientation orientation, int role) const;
    bool	setData(const QModelIndex & index, const QVariant & value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool	removeRows(int row, int count, const QModelIndex &parent);
    bool	insertRows(int row, int count, const QModelIndex &parent);

    Table * table_from_index(const QModelIndex &index) const;

    Table *_table;
  };

  Q_DECLARE_OPERATORS_FOR_FLAGS(TableModel::Attributes);

}

#endif

