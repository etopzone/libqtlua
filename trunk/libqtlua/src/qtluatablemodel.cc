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

    Copyright (C) 2008, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/

#include <QtLua/TableModel>
#include <internal/Table>

namespace QtLua {

  TableModel::TableModel(const Value &root, QObject *parent, bool recursive)
    : _table(new Table(root)),
      _recursive(recursive)
  {
  }

  TableModel::~TableModel()
  {
    delete _table;
  }

  void TableModel::update()
  {
    _table->clear();
    reset();
  }

  Table * TableModel::table_from_index(const QModelIndex &index) const
  {
    if (!index.isValid())
      return _table;

    if (!_recursive)
      return NULL;

    Table *t = static_cast<Table*>(index.internalPointer());
    return t->set_table(index.row());
  }

  QModelIndex TableModel::index(int row, int column, const QModelIndex &parent) const
  {
    Table *t = table_from_index(parent);
    assert(t);

    t->update();

    if ((unsigned)row < t->count())
      return createIndex(row, column, t);
    else
      return QModelIndex();
  }

  QModelIndex TableModel::parent(const QModelIndex &index) const
  {
    if (!index.isValid())
      return QModelIndex();

    Table *t = static_cast<Table*>(index.internalPointer());
    assert(t);

    if (t->_parent != NULL)
      return createIndex(t->_row, 0, t->_parent);
    else
      return QModelIndex();
  }

  int TableModel::rowCount(const QModelIndex &parent) const
  {
    Table *t = table_from_index(parent);

    if (t == NULL)
      return 0;

    t->update();
    return t->count();
  }

  bool TableModel::hasChildren(const QModelIndex &parent) const
  {
    Table *t = table_from_index(parent);

    return t != NULL;
  }

  int TableModel::columnCount(const QModelIndex &parent) const
  {
    return 3;
  }

  QVariant TableModel::data(const QModelIndex &index, int role) const
  {
    if (!index.isValid())
      return QVariant();

    Table *t = static_cast<Table*>(index.internalPointer());

    try {

      switch (role)
	{
	case Qt::DisplayRole:

	  switch (index.column())
	    {
	    case 0:
	      return QVariant(t->get_lua_index(index.row()));
	    case 1:
	      return QVariant(t->get_value(index.row()).type_name_u());
	    case 2:
	      return QVariant(t->get_value(index.row()).to_string_p());
	    }

	default:
	  return QVariant();
	}

    } catch (const String &e) {
      return QVariant();
    }
  }

  QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (role != Qt::DisplayRole)
      return QVariant();

    switch (section)
      {
      case 0:
	return QVariant("key");
      case 1:
	return QVariant("type");
      case 2:
	return QVariant("value");
      default:
	return QVariant();
      }
  }

}

