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


#ifndef QTLUA_TABLE_HH_
#define QTLUA_TABLE_HH_

#include <QList>
#include <QtLua/qtluatablemodel.hh>

namespace QtLua {

  /**
   * @short Qt Model/View lua table internal state class
   * @header internal/Table
   * @module {Model/View}
   * @internal
   *
   * This class is used internally by the @ref TableModel class. It is
   * used to keep lua tables display attributes and index list as
   * native lua tables can not be accessed randomly using an integer
   * index.
   */

  class Table
  {
    friend class TableModel;

    Table(const Value &val, TableModel::Attributes attr);
    ~Table();

    inline Value get_value(int n) const;
    inline void set_value(int n, const Value &value);
    inline const String & get_lua_index(int n) const;
    inline void set_lua_index(int n, const String &index);
    inline size_t count() const;
    inline bool is_table(int n) const;

    Table * set_table(int n);
    void update();
    void clear();

    struct Entry
    {
      String _index;		// FIXME keep trace of number keys
      Table *_table;
      bool _table_chk;

      inline bool operator<(const Entry &e) const;
    };

    Value _value;
    Table *_parent;
    int _row;
    TableModel::Attributes _attr;
    QList<Entry> _entries;
  };

}

#endif
