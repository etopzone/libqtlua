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


#ifndef QTLUAVALUEREF_HH_
#define QTLUAVALUEREF_HH_

#include "qtluavalue.hh"

namespace QtLua {

  class State;

  /**
   * @short Lua value reference object class
   * @header QtLua/ValueRef
   * @module {Base}
   *
   * This class acts as a reference to a lua value stored in a lua
   * table (or userdata value). It stores two lua values: a table
   * along with a key value.
   *
   * This is mainly used in the @ref State, @ref Value and
   * @ref Value::iterator classes to allow modification of lua tables with
   * the C++ square bracket operator functions.
   */
  class ValueRef : public ValueBase
  {
    friend class Value;
    friend class State;

  public:
    /** Construct reference with given table and key. */
    inline ValueRef(const Value &table, const Value &key);

    /** Construct reference with given table and key. */
    template <typename T>
    inline ValueRef(const Value &table, const T &key);

    inline ValueRef(const ValueRef &ref);

    inline ~ValueRef();

#if __cplusplus >= 201103L
    /** Construct reference with given table and key. */
    inline ValueRef( Value &&table, const Value &key);

    /** Construct reference with given table and key. */
    inline ValueRef(const Value &table, Value &&key);

    /** Construct reference with given table and key. */
    template <typename T>
    inline ValueRef(Value &&table, const T &key);

    /** Construct reference with given table and key. */
    inline ValueRef(Value &&table, Value &&key);

    inline ValueRef(ValueRef &&ref);
#endif

    Value value() const;

    /** Assign new value to referenced value. @multiple */
    inline const ValueRef & operator=(const Value &v) const;

    template <typename T>
    inline const ValueRef & operator=(T n) const;

#if 0
    inline ValueRef operator[] (const Value &key) const;

    template <typename T>
    inline ValueRef operator[] (const T &key) const;
#endif

  private:
    inline const ValueRef & operator=(const ValueRef &v) const;
    void table_set(const Value &v) const;

    void copy_table_key(double tid, double kid);
    void copy_table(double id);
    void copy_key(double id);

    void push_value() const;
    void cleanup();

    double _table_id;
    double _key_id;
  };

}

#endif

