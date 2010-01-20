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

#include <QMessageBox>
#include <QtLua/TableModel>
#include <internal/Table>

namespace QtLua {

  TableModel::TableModel(const Value &root, QObject *parent, Attributes attr)
    : _table(new Table(root, attr))
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
    return _table->_attr & HideType ? 2 : 3;
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
	    case ColKey:
	      return QVariant(t->get_lua_index(index.row()));
	    case ColType:
	      return QVariant(t->get_value(index.row()).type_name_u());
	    case ColValue:
	      return QVariant(t->get_value(index.row()).to_string_p());
	    }

	default:
	  return QVariant();
	}

    } catch (const String &e) {
      return QVariant();
    }
  }

  Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
  {
    if (!index.isValid())
      return 0;

    Table *t = static_cast<Table*>(index.internalPointer());

    Qt::ItemFlags res = Qt::ItemIsEnabled;

    if (t->_attr & Editable)
      {
	switch (index.column())
	  {
	  case ColValue:
	    if (!t->is_table(index.row()))		// prevent edit if already explored table
	      res = res | Qt::ItemIsEditable | Qt::ItemIsSelectable;
	    break;

	  case ColKey:
	    if (t->_attr & EditKey)
	      res = res | Qt::ItemIsEditable | Qt::ItemIsSelectable;
	    break;
	  }
      }

    if (t->_attr & EditRemove)
      res |= Qt::ItemIsSelectable;

    return res;
  }

  bool TableModel::setData(const QModelIndex & index, const QVariant & value, int role)
  {
    if (!index.isValid())
      return false;

    Table *t = static_cast<Table*>(index.internalPointer());

    if (role != Qt::EditRole)
      return false;

    if (!value.canConvert(QVariant::ByteArray))
      return false;

    State &state = t->_value.get_state();
    String input = value.toString();
    Value newvalue(state);
    Value oldvalue(t->get_value(index.row()));
    Value::ValueType oldtype = oldvalue.type();

    try {

      // Use lua to transform user input to lua value
      if (t->_attr & EditLuaEval)
	{
	  Value::List res = t->_value.get_state().exec_statements(String("return ") + input);

	  if (res.empty() || res[0].is_nil())
	    throw String("expression is nil");

	  newvalue = res[0];
	}

      // Do not use lua, only handle string and number cases
      else
	{
	  bool ok = false;
	  double number = value.toDouble(&ok);

	  if (ok)
	    newvalue = number;
	  else
	    {
	      String str = value.toByteArray();

	      // strip double quotes if any
	      if (str.size() > 1 && str.startsWith('"') && str.endsWith('"'))
		newvalue = String(str.mid(1, str.size() - 2));
	      else
		newvalue = str;
	    }
	}

      Value::ValueType newtype = newvalue.type();

      switch (index.column())
	{
	case ColValue:

	  // convert to string type when enforced
	  if ((t->_attr & EditFixedType) &&
	      oldtype == Value::TString &&
	      newtype != Value::TString)
	    {
	      newvalue = newvalue.to_string_p();
	      newtype = Value::TString;
	    }

	  // check type change
	  if ((t->_attr & EditFixedType) &&
	      (oldtype != newtype))
	    throw String("% value type must be preserved").arg(Value::type_name(oldtype));

	  t->set_value(index.row(), newvalue);
	  return true;

	case ColKey: {
	  String str = newvalue.to_string();
	  Value old = t->get_value(index.row());

	  t->set_value(index.row(), Value(state));
	  t->set_lua_index(index.row(), str);
	  t->set_value(index.row(), old);
	  return true;
	}

	default:
	  ;
	}

    } catch (const String &s) {
      QMessageBox::critical(0, "Value update error",
			    String("\"%\" expression error: %").arg(input).arg(s));
    }

    return false;
  }

  QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    if (role != Qt::DisplayRole)
      return QVariant();

    if (orientation == Qt::Vertical)
      return QVariant(section + 1);

    switch (section)
      {
      case ColKey:
	return QVariant("key");
      case ColType:
	return QVariant("type");
      case ColValue:
	return QVariant("value");
      default:
	return QVariant();
      }
  }

}

