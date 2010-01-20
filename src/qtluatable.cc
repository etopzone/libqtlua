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

  Table::Table(const Value &val)
    : _value(val),
      _parent(0)
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

  Table * Table::set_table(int n)
  {
    if (n >= _entries.count())
      return NULL;

    Entry *e = &_entries[n];

    if (e->_table_chk)
      return e->_table;

    Table *res = 0;
    Value value = _value[_entries[n]._index];

    switch (value.type())
      {
      case Value::TUserData:
	try {
	  if (!value.to_userdata()->support(UserData::OpIterate))
	    break;
	} catch (const String &e) {
	  // not a QtLua::UserData userdata
	  break;
	}

      case Value::TTable:
	res = new Table(value);
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
      {
	switch (i.key().type())
	  {
	  case Value::TNumber:
	  case Value::TString: {
	    Entry e;
	    e._index = i.key().to_string();
	    e._table = 0;
	    e._table_chk = false;
	    _entries.push_back(e);
	    break;
	  }

	  default:
	    break;
	  }
      }
  }

}
