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

#include <cstdlib>
#include <cassert>

#include <QtLua/Value>
#include <QtLua/UserData>
#include <QtLua/String>
#include <QtLua/State>

#include <internal/Member>
#include <internal/QObjectWrapper>

extern "C" {
#include <lua.h>
}

namespace QtLua {

void Value::push_value() const
{
  check_state();
  lua_State *lst = _st->_lst;
  lua_pushnumber(lst, _id);
  lua_rawget(lst, LUA_REGISTRYINDEX);  
}

int Value::empty_fcn(lua_State *st)
{
  return 0;
}

void Value::init_table()
{
  check_state();
  lua_State *lst = _st->_lst;
  lua_pushnumber(lst, _id);
  lua_newtable(lst);
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

Value & Value::operator=(Bool n)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushnumber(lst, _id);
      lua_pushboolean(lst, n);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value & Value::operator=(double n)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushnumber(lst, _id);
      lua_pushnumber(lst, n);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value & Value::operator=(const String &str)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushnumber(lst, _id);
      lua_pushlstring(lst, str.constData(), str.size());
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value & Value::operator=(const Ref<UserData> &ud)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushnumber(lst, _id);
      ud->push_ud(lst);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value::Value(State *ls, QObject *obj, bool delete_, bool reparent)
  : ValueBase(ls)
  , _id(_id_counter++)
{
  lua_State *lst = _st->_lst;
  lua_pushnumber(lst, _id);
  QObjectWrapper::get_wrapper(_st, obj, reparent, delete_)->push_ud(lst);
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

Value & Value::operator=(QObject *obj)
{
  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushnumber(lst, _id);
      QObjectWrapper::get_wrapper(_st, obj)->push_ud(lst);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }
  return *this;
}

Value & Value::operator=(const QVariant &qv)
{
  if (_st)
    *this = Member::raw_get_object(_st, qv.type(), qv.constData());
  return *this;
}

Value & Value::operator=(const Value &lv)
{
  if (_st && _st != lv._st)
    {
      lua_State *lst = _st->_lst;
      lua_pushnumber(lst, _id);
      lua_pushnil(lst);
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }

  _st = lv._st;

  if (_st)
    {
      lua_State *lst = _st->_lst;
      lua_pushnumber(lst, _id);
      lv.push_value();
      lua_rawset(lst, LUA_REGISTRYINDEX);
    }

  return *this;
}

Value::Value(const Value &lv)
  : ValueBase(lv._st)
  , _id(_id_counter++)
{
  if (!_st)
    return;

  lua_State *lst = _st->_lst;
  lua_pushnumber(lst, _id);
  lv.push_value();
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

Value::Value(const State *ls, const Value &lv)
  : ValueBase(ls)
  , _id(_id_counter++)
{
  if (!_st)
    return;

  assert(_st == lv._st);

  lua_State *lst = _st->_lst;
  lua_pushnumber(lst, _id);
  lv.push_value();
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

void Value::cleanup()
{
  lua_State *lst = _st->_lst;
  lua_pushnumber(lst, _id);
  lua_pushnil(lst);
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

Value::Value(int index, const State *st)
  : ValueBase(st)
  , _id(_id_counter++)
{
  lua_State *lst = _st->_lst;

  lua_pushnumber(lst, _id);
  if (index < 0 && index != LUA_GLOBALSINDEX)
    index--;
  lua_pushvalue(lst, index);
  lua_rawset(lst, LUA_REGISTRYINDEX);
}

uint qHash(const Value &lv)
{
  lv.push_value();

  lua_State *lst = lv._st->_lst;
  uint	res;

  res = ValueBase::qHash(lst, -1);
  lua_pop(lst, 1);
  return res;
}

}

