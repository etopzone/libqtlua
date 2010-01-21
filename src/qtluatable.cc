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

#include <QtLua/Value>
#include <internal/Table>

namespace QtLua {

  Table::Table(const Value &val, TableModel::Attributes attr)
    : _value(val),
      _parent(0),
      _attr(attr)
  {
  }

  Table::~Table()
  {
    clear();
  }

  void Table::clear()
  {
    while (!_entries.empty())
      {
	const Entry &e = _entries.takeLast();
	if (e._table)
	  delete e._table;
      }
  }

  Value Table::get_value(int n) const
  {
    Value key = _entries[n]._key;
    if (key.is_nil())
      return key;
    return _value[_entries[n]._key];
  }

  void Table::set_value(int n, const Value &value)
  {
    Value key = _entries[n]._key;
    try {
      if (!key.is_nil())
	_value[key] = value;
    } catch (const String &e) {
    }
  }

  Table * Table::set_table(int n)
  {
    if (!(_attr & TableModel::Recursive))
      return NULL;

    if (n >= _entries.count())
      return NULL;

    Entry *e = &_entries[n];

    if (e->_table_chk)
      return e->_table;

    Table *res = 0;
    Value value = get_value(n);
    TableModel::Attributes attr_mask;

    switch (value.type())
      {
      case Value::TUserData:
	if (!(_attr & TableModel::UserDataIter))
	  break;

	try {
	  if (!value.to_userdata()->support(UserData::OpIterate))
	    break;
	  if (!value.to_userdata()->support(UserData::OpIndex))
	    break;

	  if (!value.to_userdata()->support(UserData::OpNewindex))
	    attr_mask |= TableModel::EditAll;

	} catch (const String &e) {
	  // not a QtLua::UserData userdata
	  break;
	}

      case Value::TTable:
	res = new Table(value, _attr & ~attr_mask);
	res->_parent = this;
	res->_row = n;
	e->_table = res;

      default:
	break;
      }

    e->_table_chk = true;

    return res;
  }

  void Table::update()
  {
    if (!_entries.empty())
      return;

    Value::const_iterator i;

    try {
      i = _value.begin();
    } catch (const String &e) {
      return;
    }

    for (; i != _value.end(); i++)
      _entries.push_back(Entry(i.key()));

    qSort(_entries.begin(), _entries.end());
  }

}

