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

#include <cassert>
#include <cstdlib>

#include <QtLua/ValueRef>

extern "C" {
#include <lua.h>
}

namespace QtLua {

  void ValueRef::table_set(const Value &v) const
  {
    _table.push_value();

    lua_State *lst = _table._st->_lst;
    int t = lua_type(lst, -1);

    switch (t)
      {
      case Value::TUserData: {
	UserData::ptr ud = UserData::pop_ud(lst);

	if (!ud.valid())
	  throw String("Can not index null lua::userdata value.");

	ud->meta_newindex(_table._st, _key, v);
	return;
      }

      case Value::TTable:
	_key.push_value();
	if (lua_isnil(lst, -1))
	  {
	    lua_pop(lst, 2);
	  }
	else
	  {
	    v.push_value();
	    lua_settable(lst, -3);
	    lua_pop(lst, 1);
	  }
	return;

      default:
	lua_pop(lst, 1);
	throw String("Can not index lua::% value.").arg(lua_typename(lst, t));
      }
  }

}

