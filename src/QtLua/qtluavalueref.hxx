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

  ValueRef::ValueRef(const Value &table, const Value &key)
    : _table(table),
      _key(key)
  {
    assert(table._st == key._st);
  }

  template <typename T>
  ValueRef::ValueRef(const Value &table, const T &key)
    : _table(table),
      _key(Value(table._st, key))
  {
  }

  ValueRef::ValueRef(const ValueRef &ref)
    : _table(ref._table),
      _key(ref._key)
  {
  }

#if __cplusplus >= 201103L
  ValueRef::ValueRef(const Value &&table, const Value &key)
    : _table(std::move(table))
    , _key(key)
  {
  }

  template <typename T>
  ValueRef::ValueRef(const Value &&table, const T &key)
    : _table(std::move(table))
    , _key(Value(table._st, key))
  {
  }

  ValueRef::ValueRef(const Value &&table, const Value &&key)
    : _table(std::move(table))
    , _key(std::move(key))
  {
  }

  ValueRef::ValueRef(const ValueRef &&ref)
    : _table(std::move(ref._table))
    , _key(std::move(ref._key))
  {
  }
#endif

  Value ValueRef::value() const
  {
    return _table.at(_key);
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
    table_set(Value(_table._st, n));
    return *this;
  }

  ValueRef ValueRef::operator[] (const Value &key) const
  {
    return ValueRef(_table.at(_key), key);
  }

  template <typename T>
  ValueRef ValueRef::operator[] (const T &key) const
  {
    return (*this)[Value(_table._st, key)];
  }

}

#endif

