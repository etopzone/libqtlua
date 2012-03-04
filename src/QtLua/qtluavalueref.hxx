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


#ifndef QTLUAVALUEREF_HXX_
#define QTLUAVALUEREF_HXX_

#include <cassert>

#include "qtluavalue.hxx"

namespace QtLua {

  ValueRef::ValueRef(const ValueRef &ref)
    : ValueBase(ref._st)
    , _table_id(_id_counter++)
    , _key_id(_id_counter++)
  {
    copy_table_key(ref._table_id, ref._key_id);
  }

  ValueRef::ValueRef(const Value &table, const Value &key)
    : ValueBase(table._st)
    , _table_id(_id_counter++)
    , _key_id(_id_counter++)
  {
    assert(table._st == key._st);
    copy_table_key(table._id, key._id);
  }

#if __cplusplus >= 201103L

  ValueRef::ValueRef(Value &&table, const Value &key)
    : ValueBase(table._st)
    , _table_id(table._id)
    , _key_id(_id_counter++)
  {
    assert(table._st == key._st);
    table._st = 0;
    copy_key(key._id);
  }

  template <typename T>
  ValueRef::ValueRef(Value &&table, const T &key)
    : ValueBase(table._st)
    , _table_id(table._id)
    , _key_id(_id_counter++)
  {
    table._st = 0;
    Value k(table._st, key);
    _key_id = k._id;
    k._st = 0;
  }

  ValueRef::ValueRef(const Value &table, Value &&key)
    : ValueBase(table._st)
    , _table_id(_id_counter++)
    , _key_id(key._id)
  {
    assert(table._st == key._st);
    key._st = 0;
    copy_table(table._id);
  }

  ValueRef::ValueRef(Value &&table, Value &&key)
    : ValueBase(table._st)
    , _table_id(table._id)
    , _key_id(key._id)
  {
    assert(table._st == key._st);
    table._st = 0;
    key._st = 0;
  }

  ValueRef::ValueRef(ValueRef &&ref)
    : ValueBase(ref._st)
    , _table_id(ref._table_id)
    , _key_id(ref._key_id)
  {
    ref._st = 0;
  }

#endif

  template <typename T>
  ValueRef::ValueRef(const Value &table, const T &key)
    : ValueBase(table._st)
    , _table_id(_id_counter++)
  {
    copy_table(table._id);
    Value k(table._st, key);
    _key_id = k._id;
    k._st = 0;
  }

  ValueRef::~ValueRef()
  {
    if (_st)
      cleanup();
  }

#if 0
  const ValueRef & ValueRef::operator=(const ValueRef &ref) const
  {
    table_set(ref._table[ref._key]);
    return *this;
  }
#endif

  const ValueRef & ValueRef::operator=(const Value &v) const
  {
    table_set(v);
    return *this;
  }

  template <typename T>
  const ValueRef & ValueRef::operator=(T n) const
  {
    table_set(Value(_st, n));
    return *this;
  }

}

#endif

