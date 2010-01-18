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
   * Lua tables and @ref UserData objects with iteration capabilities
   * are handled. Changes in exposed lua tables do @b not update the
   * model and the @ref update function must be called. This is
   * partially due to lack of lua mechanism to implement efficient
   * table change hook yet.
   *
   * Usage example:
   * @example examples/cpp/mvc/tabletreeview.cc:1
   * @see TableDialog
   */

  class TableModel : public QAbstractItemModel
  {
    Q_OBJECT;

  public:

    /** Create a new lua table model. */
    TableModel(const Value &root, QObject *parent = 0,
	       bool recursive = true);

    ~TableModel();

    /** Clear cached table content and reset model. */
    void update();

  private:
    Table * table_from_index(const QModelIndex &index) const;

    QModelIndex	index(int row, int column, const QModelIndex &parent) const;
    QModelIndex	parent(const QModelIndex &index) const;
    int		rowCount(const QModelIndex &parent) const;
    bool	hasChildren(const QModelIndex & parent) const;
    int		columnCount(const QModelIndex &parent) const;
    QVariant	data(const QModelIndex &index, int role) const;
    QVariant	headerData(int section, Qt::Orientation orientation, int role) const;

    Table *_table;
    bool _recursive;
  };

}

#endif

