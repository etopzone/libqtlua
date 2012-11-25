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
#include <QMetaMethod>

#include <internal/QMetaObjectWrapper>
#include <internal/QObjectIterator>
#include <internal/MetaCache>
#include <internal/Member>

namespace QtLua {

  QMetaObjectWrapper::QMetaObjectWrapper(const QMetaObject *mo, qobject_creator *creator)
    : _mo(mo)
    , _creator(creator)
  {
  }

  QObject * QMetaObjectWrapper::create(const Value::List &lua_args) const
  {
    // try constructor without argument if available
    if (lua_args.size() <= 1 && _creator)
      return _creator();

    void *qt_args[11];
    int qt_tid[11];

#warning Not tested yet

    // iterate over Q_INVOKABLE constructors
    for (int j = 0; j < _mo->constructorCount(); j++)
      {
	QMetaMethod mm = _mo->constructor(j);

	int tid, i;

	QList<QByteArray> ptlist = mm.parameterTypes();

	if (ptlist.size() != lua_args.size() - 1)
	  continue;

	// get argument types
	try {
	  for (i = 0; j < ptlist.size(); i++)
	    {
	      tid = QMetaType::type(ptlist[i].constData());

	      if (!tid)
		goto wrong_prototype;

#if QT_VERSION < 0x050000
	      void *arg = QMetaType::construct(tid);
#else
	      void *arg = QMetaType::create(tid, 0);
#endif

	      qt_tid[i+1] = tid;
	      qt_args[i+1] = arg;

	      Member::raw_set_object(tid, arg, lua_args[i + 1]);
	    }

	} catch (...) {
	  for (int k = i; k >= 0; k--)
	    if (qt_args[k])
	      QMetaType::destroy(qt_tid[k], qt_args[k]);	  
	  goto wrong_prototype;
	}

	_mo->static_metacall(QMetaObject::CreateInstance, j, qt_args);

	for (int j = i - 1; j > 0; j--)
	  if (qt_args[j])
	    QMetaType::destroy(qt_tid[j], qt_args[j]);

	return (QObject*)qt_args[0];

      wrong_prototype:;
      }

    throw QtLua::String("No suitable constructor found for `%' class").arg(_mo->className());
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

