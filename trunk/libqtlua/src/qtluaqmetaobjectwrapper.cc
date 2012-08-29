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

#include <QObject>
#include <QMetaObject>

#include <internal/QMetaObjectWrapper>
#include <internal/QObjectIterator>
#include <internal/MetaCache>
#include <internal/Member>

namespace QtLua {

  QMetaObjectWrapper::QMetaObjectWrapper(const QMetaObject *mo)
    : _mo(mo)
  {
  }

  Value QMetaObjectWrapper::meta_index(State *ls, const Value &key)
  {
    const MetaCache &mc = MetaCache::get_meta(_mo);
    String name(key.to_string());

    Member::ptr m = mc.get_member(name);
    if (m.valid())
      return Value(ls, m);

    int enum_value = mc.get_enum_value(name);    
    if (enum_value >= 0)
      return Value(ls, enum_value);

    return Value(ls);
  }

  Ref<Iterator> QMetaObjectWrapper::new_iterator(State *ls)
  {
    return QTLUA_REFNEW(QObjectIterator, ls, _mo);
  }

  bool QMetaObjectWrapper::support(Value::Operation c) const
  {
    switch (c)
      {
      case Value::OpIndex:
      case Value::OpIterate:
	return true;
      default:
	return false;
      }
  }

  void QMetaObjectWrapper::completion_patch(String &path, String &entry, int &offset)
  {
    entry += ".";
  }

  String QMetaObjectWrapper::get_value_str() const
  {
    String res(_mo->className());

    if (_mo->superClass())
      res += String(" : public ") + _mo->superClass()->className();

    return res;
  }

};

