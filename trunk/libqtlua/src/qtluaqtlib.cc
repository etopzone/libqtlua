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

#include <QFile>
#include <QUiLoader>
#include <QWidget>

#include <QLayout>
#include <QColorDialog>
#include <QFileDialog>
#include <QErrorMessage>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>
#include <QTranslator>
#include <QActionGroup>
#include <QMainWindow>
#include <QToolBar>

#include <QMenu>
#include <QMenuBar>

#include <QtLua/State>
#include <QtLua/Function>
#include <internal/QObjectWrapper>
#include <QtLua/QHashProxy>
#include <QtLua/TableDialog>

#include <internal/Method>
#include <internal/MetaCache>
#include <internal/QMetaObjectWrapper>

#include "qtluaqtlib.hh"

namespace QtLua {

  typedef QMap<String, QMetaObjectWrapper > qmetaobject_table_t;

  class QMetaObjectTable
    : public QHashProxyRo<qmetaobject_table_t>
    , public QObject
  {
  public:
    QMetaObjectTable()
      : QHashProxyRo<qmetaobject_table_t>(_mo_table)
    {
      for (const meta_object_table_s *me = meta_object_table; me->_mo; me++)
	{
	  String name(me->_mo->className());
	  name.replace(':', '_');
	  _mo_table.insert(name, QMetaObjectWrapper(me->_mo, me->_creator));
	}

      _mo_table.insert("Qt", QMetaObjectWrapper(&staticQtMetaObject));
      _mo_table.insert("QSizePolicy", QMetaObjectWrapper(&QtLua::SizePolicy::staticMetaObject));
      _mo_table.insert("QFileDialog", QMetaObjectWrapper(&QtLua::FileDialog::staticMetaObject, 
							 &create_qobject<FileDialog>));
    }

    qmetaobject_table_t _mo_table;
  };

  static QMetaObjectTable qt_meta;

  void qtlib_register_meta(const QMetaObject *mo, qobject_creator *creator)
  {
    String name(mo->className());
    name.replace(':', '_');
    qt_meta._mo_table.insert(name, QMetaObjectWrapper(mo, creator));
  }


  ////////////////////////////////////////////////// widgets and qobjects


  QTLUA_FUNCTION(connect, "Connect a Qt signal to a Qt slot or lua function.",
		 "usage: qt.connect(qobjectwrapper, \"qt_signal_signature()\", qobjectwrapper, \"qt_slot_signature()\")\n"
		 "       qt.connect(qobjectwrapper, \"qt_signal_signature()\", lua_function)\n")
  {
    meta_call_check_args(args, 3, 4, Value::TUserData, Value::TString, Value::TNone, Value::TString);

    QObjectWrapper::ptr sigqow = args[0].to_userdata_cast<QObjectWrapper>();

    String signame = args[1].to_string();
    QObject &sigobj = sigqow->get_object();

    int sigindex = sigobj.metaObject()->indexOfSignal(signame.constData());
    if (sigindex < 0)
      throw String("No such signal `%'.").arg(signame);

    switch (args.size())
      {
      case 3: {
	// connect qt signal to lua function
	sigqow->_lua_connect(sigindex, args[2]);
	return Value::List();
      }

      case 4: {
	// connect qt signal to qt slot
	String slotname = args[3].to_string();
	QObject &sloobj = args[2].to_userdata_cast<QObjectWrapper>()->get_object();	

	int slotindex = sloobj.metaObject()->indexOfSlot(slotname.constData());
	if (slotindex < 0)
	  throw String("No such slot `%'.").arg(slotname);

	if (!QMetaObject::checkConnectArgs(signame.constData(), slotname.constData()))
	  throw String("Unable to connect signal/slot, incompatible argument types.");

	if (QMetaObject::connect(&sigobj, sigindex, &sloobj, slotindex))
	  return Value::List();
      }
      }
    throw String("Unable to connect signal/slot.");
  }


  QTLUA_FUNCTION(disconnect, "Disconnect a Qt signal",
		 "usage: qt.disconnect(qobjectwrapper, \"qt_signal_signature()\", qobjectwrapper, \"qt_slot_signature()\")\n"
		 "       qt.disconnect(qobjectwrapper, \"qt_signal_signature()\", lua_function)\n"
		 "       qt.disconnect(qobjectwrapper, \"qt_signal_signature()\")\n")
  {
    meta_call_check_args(args, 2, 4, Value::TUserData, Value::TString, Value::TNone, Value::TString);

    QObjectWrapper::ptr sigqow = args[0].to_userdata_cast<QObjectWrapper>();

    String signame = args[1].to_string();
    QObject &sigobj = sigqow->get_object();

    int sigindex = sigobj.metaObject()->indexOfSignal(signame.constData());
    if (sigindex < 0)
      throw String("No such signal `%'.").arg(signame);

    switch (args.size())
      {
      case 2:
	// disconnect qt signal from all lua functions
	sigqow->_lua_disconnect_all(sigindex);
	return Value::List();

      case 3:
	// disconnect qt signal from lua function
	return Value(ls, (Value::Bool)sigqow->_lua_disconnect(sigindex, args[2]));

      case 4: {
	// disconnect qt signal from qt slot
	String slotname = args[3].to_string();
	QObject &sloobj = args[2].to_userdata_cast<QObjectWrapper>()->get_object();	

	int slotindex = sloobj.metaObject()->indexOfSlot(slotname.constData());
	if (slotindex < 0)
	  throw String("No such slot `%'.").arg(slotname);

	return Value(ls, (Value::Bool)QMetaObject::disconnect(&sigobj, sigindex, &sloobj, slotindex));
      }

      }

    abort();
  }


  QTLUA_FUNCTION(load_ui, "Load a Qt ui file.",
		 "usage: qt.load_ui(\"file.ui\", parent_qobjectwrapper)\n"
		 "usage: qt.load_ui(\"file.ui\")\n")
  {
    static QUiLoader uil;

    meta_call_check_args(args, 1, 2, Value::TString, Value::TUserData);
    QWidget *p = 0;

    if (args.size() > 1)
      p = args[0].to_qobject_cast<QWidget>();

    QFile f(args[0].to_qstring());
    QObject *w = uil.load(&f, p);

    if (!w)
      throw String("Unable to load '%' ui file.").arg(f.fileName());

    return Value(ls, w, true, true);
  }


  QTLUA_FUNCTION(new_widget, "Dynamically create a new Qt Widget using QUiLoader.",
		 "usage: qt.new_widget(\"QtClassName\", [ \"name\", parent_qobjectwrapper ] )\n")
  {
    static QUiLoader uil;

    meta_call_check_args(args, 1, 3, Value::TString, Value::TString, Value::TUserData);
    QWidget *p = 0;
    String classname(args[0].to_string());
    String name;

    if (args.size() > 2)
      p = args[2].to_qobject_cast<QWidget>();

    if (args.size() > 1)
      name = args[1].to_string();

    QObject *w = uil.createWidget(classname.to_qstring(), p, name.to_qstring());

    if (!w)
      throw String("Unable to create % type widget.").arg(classname);

    return Value(ls, w, true, true);
  }


  QTLUA_FUNCTION(new_qobject, "Dynamically create a new QObject.",
		 "usage: qt.new_qobject( qt.meta.QClassName, [ Constructor arguments ] )\n")
  {
    static QUiLoader uil;

    meta_call_check_args(args, 1, 0, Value::TUserData, Value::TNone);
    QMetaObjectWrapper::ptr mow = args[0].to_userdata_cast<QMetaObjectWrapper>();

    return Value(ls, mow->create(args), true, true);
  }


  QTLUA_FUNCTION(layout_add, "Add an item to a QLayout or set QLayout of a QWidget.",
		 "usage: qt.layout_add( layout, widget|layout )\n"
		 "       qt.layout_add( widget, layout )\n")
  {
    meta_call_check_args(args, 2, 2, Value::TUserData, Value::TUserData);

    QObject *obj = args[0].to_qobject();

    QObjectWrapper::ptr qow = args[1].to_userdata_cast<QObjectWrapper>();
    QObject &item = qow->get_object();

    if (QLayout *la = dynamic_cast<QLayout*>(obj))
      {
	if (QLayoutItem *li = dynamic_cast<QLayoutItem*>(&item))
	  {
	    qow->set_delete(false);
	    la->addItem(li);
	  }
	else if (QWidget *w = dynamic_cast<QWidget*>(&item))
	  la->addWidget(w);
	else
	  goto err;
      }
    else if (QWidget *w = dynamic_cast<QWidget*>(obj))
      {
	if (QLayout *la = dynamic_cast<QLayout*>(&item))
	  {
	    delete w->layout();
	    w->setLayout(la);
	  }
	else
	  goto err;
      }
    else
      goto err;

    return QtLua::Value(ls);
  err:
    throw String("Bad layout object type");
  }


  QTLUA_FUNCTION(layout_spacer, "Add an item to a QLayout.",
		 "usage: qt.layout_spacer( layout, width, height, hpolicy, vpolicy )\n")
  {
    meta_call_check_args(args, 3, 5, Value::TUserData, Value::TNumber, Value::TNumber, Value::TNumber, Value::TNumber);

    QLayout *la = args[0].to_qobject_cast<QLayout>();

    la->addItem(new QSpacerItem(get_arg<int>(args, 1),
				get_arg<int>(args, 2),
				(QSizePolicy::Policy)get_arg<int>(args, 3, QSizePolicy::Minimum),
				(QSizePolicy::Policy)get_arg<int>(args, 4, QSizePolicy::Minimum)));

    return QtLua::Value(ls);
  }

  
  ////////////////////////////////////////////////// translation


  QTLUA_FUNCTION(tr, "Translate utf8 text using the QCoreApplication::translate function.",
		 "usage: qt.tr(\"context\", \"text\", [ \"disambiguation\", n ])\n")
  {
    return Value(ls, QCoreApplication::translate(get_arg<String>(args, 0),
						 get_arg<String>(args, 1),
						 get_arg<String>(args, 2, ""),
						 QCoreApplication::UnicodeUTF8,
						 get_arg<int>(args, 3, -1)));
  }


  QTLUA_FUNCTION(translator, "Install a translation file and return associated QTranslator object.",
		 "usage: qt.translator(\"filename\")\n")
  {
    String filename(get_arg<String>(args, 0));
    QTranslator *qtr = new QTranslator();

    if (!qtr->load(filename))
      {
	delete qtr;
	throw String("Unable to load translation file `%'").arg(filename);
      }

    QCoreApplication::installTranslator(qtr);
    return Value(ls, qtr, true, true);
  }


  ////////////////////////////////////////////////// menus
  

  QTLUA_FUNCTION(add_toolbar, "Add a new QToolBar to a QMainWindow.",
		 "usage: qt.menu.add_toolbar( main_window, \"text\", [ \"name\" ] )\n")
  {
    meta_call_check_args(args, 2, 3, Value::TUserData, Value::TString, Value::TString);

    QMainWindow *mw = args[0].to_qobject_cast<QMainWindow>();
    String text = args[1].to_string();
    QObject *result = mw->addToolBar(text);

    if (args.size() > 2)
      result->setObjectName(args[2]);

    return QtLua::Value(ls, result, true, true);
  }


  QTLUA_FUNCTION(add_menu, "Add a new QMenu to a QMenu or QMenuBar container.",
		 "usage: qt.menu.add_menu( container, \"text\", [ \"name\" ] )\n")
  {
    meta_call_check_args(args, 2, 3, Value::TUserData, Value::TString, Value::TString);

    QObject *obj = args[0].to_qobject();
    String text = args[1].to_string();
    QObject *result;

    if (QMenu *menu = dynamic_cast<QMenu*>(obj))
      result = menu->addMenu(text);
    else if (QMenuBar *menubar = dynamic_cast<QMenuBar*>(obj))
      result = menubar->addMenu(text);
    else
      throw String("Bad menu owner object type");

    if (args.size() > 2)
      result->setObjectName(args[2]);

    return QtLua::Value(ls, result, true, true);
  }


  QTLUA_FUNCTION(add_separator, "Add a separator QAction to a QMenu or QToolBar.",
		 "usage: qt.menu.add_separator( container, [ \"name\" ] )\n")
  {
    meta_call_check_args(args, 1, 2, Value::TUserData, Value::TString);

    QObject *obj = args[0].to_qobject();
    QObject *result;

    if (QMenu *menu = dynamic_cast<QMenu*>(obj))
      result = menu->addSeparator();
    else if (QToolBar *tb = dynamic_cast<QToolBar*>(obj))
      result = tb->addSeparator();
    else
      throw String("Bad QMenu object type");

    if (args.size() > 1)
      result->setObjectName(args[1]);

    return QtLua::Value(ls, result, true, true);
  }


  QTLUA_FUNCTION(add_action, "Add a QAction to a QMenuBar, QMenu or QActionGroup.",
		 "usage: qt.menu.add_action( container, \"text\"|qaction, [ \"name\" ] )\n")
  {
    meta_call_check_args(args, 2, 3, Value::TUserData, Value::TNone, Value::TString);

    QObject *obj = args[0].to_qobject();
    QObject *result;

    switch (args[1].type())
      {
      case Value::TString: {
	String text = args[1].to_string();

	if (QMenu *menu = dynamic_cast<QMenu*>(obj))
	  result = menu->addAction(text);
	else if (QMenuBar *menubar = dynamic_cast<QMenuBar*>(obj))
	  result = menubar->addAction(text);
	else if (QActionGroup *group = dynamic_cast<QActionGroup*>(obj))
	  result = group->addAction(text);
	else if (QToolBar *tb = dynamic_cast<QToolBar*>(obj))
	  result = tb->addAction(text);
	else
	  throw String("Bad QAction container object type");
	break;
      }

      case Value::TUserData: {
	QAction *action = args[1].to_qobject_cast<QAction>();
	result = action;

	if (QMenu *menu = dynamic_cast<QMenu*>(obj))
	  menu->addAction(action);
	else if (QMenuBar *menubar = dynamic_cast<QMenuBar*>(obj))
	  menubar->addAction(action);
	else if (QActionGroup *group = dynamic_cast<QActionGroup*>(obj))
	  group->addAction(action);
	else if (QToolBar *tb = dynamic_cast<QToolBar*>(obj))
	  tb->addAction(action);
	else
	  throw String("Bad QAction container object type");
	break;
      }

      default:
	throw String("Bad QAction object");
      }

    if (args.size() > 2)
      result->setObjectName(args[2].to_string());

    return QtLua::Value(ls, result, true, true);
  }


  QTLUA_FUNCTION(new_action_group, "Create a new QActionGroup and add passed actions.",
		 "usage: qt.menu.new_action_group( action [, action ...] )\n")
  {
    QAction *a[args.size()];

    for (int i = 0; i < args.size(); i++)
      a[i] = args[i].to_qobject_cast<QAction>();

    QActionGroup *result = new QActionGroup(0);
    for (int i = 0; i < args.size(); i++)
      result->addAction(a[i]);

    return QtLua::Value(ls, result, true, true);
  }


  QTLUA_FUNCTION(remove, "Remove a QAction or QMenu action from a QWidget or QActionGroup.",
		 "usage: qt.menu.remove( qaction|qmenu [, qwidget|qactiongroup ] )\n")
  {
    meta_call_check_args(args, 1, 2, Value::TUserData, Value::TUserData);

    QObject *obj = args[0].to_qobject();
    QObject *pobj;
    QAction *action;
    QMenu *menu = 0;

    if (args.size() > 1)
      pobj = args[1].to_qobject();
    else
      pobj = obj->parent();

    if ((action = dynamic_cast<QAction*>(obj)))
      ;
    else if ((menu = dynamic_cast<QMenu*>(obj)))
      action = menu->menuAction();
    else
      throw String("Bad QAction object");

    if (QWidget *w = dynamic_cast<QWidget*>(pobj))
      w->removeAction(action);
    else if (QActionGroup *group = dynamic_cast<QActionGroup*>(pobj))
      group->removeAction(action);
    else
      throw String("Bad QWidget object to remove action from");

    return QtLua::Value(ls);
  }


  ////////////////////////////////////////////////// dialogs


  QTLUA_FUNCTION(get_existing_directory, "Wrap QFileDialog::getExistingDirectory function.",
		 "usage: qt.dialog.get_existing_directory( [ \"caption\", \"directory\", options ] )\n")
  {
    return Value(ls, QFileDialog::getExistingDirectory(QApplication::activeWindow(),
						       get_arg<QString>(args, 0, ""),
						       get_arg<QString>(args, 1, ""),
						       (QFileDialog::Option)get_arg<int>(args, 2, QFileDialog::ShowDirsOnly)
						       ));
  }


  QTLUA_FUNCTION(get_open_filename, "Wrap QFileDialog::getOpenFileName function.",
		 "usage: qt.dialog.get_open_filename( [ \"caption\", \"directory\", \"filter\", options ] )\n")
  {
    return Value(ls, QFileDialog::getOpenFileName(QApplication::activeWindow(),
						  get_arg<QString>(args, 0, ""),
						  get_arg<QString>(args, 1, ""),
						  get_arg<QString>(args, 2, ""), 0,
						  (QFileDialog::Option)get_arg<int>(args, 3, 0)
						  ));
  }

  QTLUA_FUNCTION(get_open_filenames, "Wrap QFileDialog::getOpenFileNames function.",
		 "usage: qt.dialog.get_open_filenames( [ \"caption\", \"directory\", \"filter\", options ] )\n")
  {
    return Value(ls, QFileDialog::getOpenFileNames(QApplication::activeWindow(),
						   get_arg<QString>(args, 0, ""),
						   get_arg<QString>(args, 1, ""),
						   get_arg<QString>(args, 2, ""), 0,
						   (QFileDialog::Option)get_arg<int>(args, 3, 0)
						   ));
  }


  QTLUA_FUNCTION(get_save_filename, "Wrap QFileDialog::getSaveFileName function.",
		 "usage: qt.dialog.get_save_filename( [ \"caption\", \"directory\", \"filter\", options ] )\n")
  {
    return Value(ls, QFileDialog::getSaveFileName(QApplication::activeWindow(),
						  get_arg<QString>(args, 0, ""),
						  get_arg<QString>(args, 1, ""),
						  get_arg<QString>(args, 2, ""), 0,
						  (QFileDialog::Option)get_arg<int>(args, 3, 0)
						  ));
  }


  QTLUA_FUNCTION(get_color, "Wrap QColorDialog::getColor function, returns rgb triplet in [0, 255] range.",
		 "usage: qt.dialog.get_color( [ init_red, init_green, init_blue ] )\n")	 
  {
    QColor init(Qt::white);

    if (args.count() >= 3)
      init = QColor(get_arg<int>(args, 0, 0), get_arg<int>(args, 1, 0), get_arg<int>(args, 2, 0));

    QColor c = QColorDialog::getColor(init, QApplication::activeWindow());

    return c.isValid() ? Value::List(Value(ls, c.red()), Value(ls, c.green()), Value(ls, c.blue()))
      : Value::List();
  }


  QTLUA_FUNCTION(get_double, "Wrap QInputDialog::getDouble function.",
		 "usage: qt.dialog.get_double(\"title\", \"label\", value, min, max, decimals)\n")
  {
    bool ok;
    double v = QInputDialog::getDouble(QApplication::activeWindow(),
				       get_arg<QString>(args, 0, ""),
				       get_arg<QString>(args, 1, ""),
				       get_arg<double>(args, 2, 0),
				       get_arg<double>(args, 3, -2147483647),
				       get_arg<double>(args, 4, 2147483647),
				       get_arg<int>(args, 5, 1),
				       &ok
				       );
    return ok ? Value(ls, v) : Value(ls);
  }


  QTLUA_FUNCTION(get_integer, "Wrap QInputDialog::getInteger function.",
		 "usage: qt.dialog.get_integer(\"title\", \"label\", value, min, max, step)\n")
  {
    bool ok;
    int v = QInputDialog::getInteger(QApplication::activeWindow(),
				     get_arg<QString>(args, 0, ""),
				     get_arg<QString>(args, 1, ""),
				     get_arg<int>(args, 2, 0),
				     get_arg<int>(args, 3, -2147483647),
				     get_arg<int>(args, 4, 2147483647),
				     get_arg<int>(args, 5, 1),
				     &ok
				     );
    return ok ? Value(ls, v) : Value(ls);
  }


  QTLUA_FUNCTION(get_text, "Wrap QInputDialog::getText function.",
		 "usage: qt.dialog.get_text(\"title\", \"label\", \"text\")\n")
  {
    bool ok;
    QString v = QInputDialog::getText(QApplication::activeWindow(),
				      get_arg<QString>(args, 0, ""),
				      get_arg<QString>(args, 1, ""),
				      QLineEdit::Normal,
				      get_arg<QString>(args, 2, ""),
				      &ok
				      );
    return ok ? Value(ls, v) : Value(ls);
  }


  QTLUA_FUNCTION(get_item, "Wrap QInputDialog::getItem function.",
		 "usage: qt.dialog.get_item( { \"item\", \"item\", ... }, [ default, editable, \"title\", \"label\" ] )\n")
  {
    bool ok;
    QString v = QInputDialog::getItem(QApplication::activeWindow(),
				      get_arg<QString>(args, 3, ""),
				      get_arg<QString>(args, 4, ""),
				      get_arg<QList<QString> >(args, 0),
				      get_arg<int>(args, 1, 0),
				      get_arg<Value::Bool>(args, 2, Value::False),
				      &ok
				      );
    return ok ? Value(ls, v) : Value(ls);
  }


  QTLUA_FUNCTION(msg_about, "Wrap QMessageBox::about function.",
		 "usage: qt.dialog.msg_about( \"text\" [ , \"title\" ] )\n")
  {
    QMessageBox::about(QApplication::activeWindow(),
		       get_arg<QString>(args, 1, ""),
		       get_arg<QString>(args, 0));
    return Value(ls);
  }


  QTLUA_FUNCTION(msg_critical, "Wrap QMessageBox::critical function.",
		 "usage: qt.dialog.msg_critical( \"text\" [ , \"title\", buttons, default_button ] )\n")
  {
    return Value(ls, QMessageBox::critical(QApplication::activeWindow(),
					   get_arg<QString>(args, 1, ""),
					   get_arg<QString>(args, 0),
					   (QMessageBox::StandardButtons)get_arg<int>(args, 2, QMessageBox::Ok),
					   (QMessageBox::StandardButton)get_arg<int>(args, 3, QMessageBox::NoButton)));
  }


  QTLUA_FUNCTION(msg_information, "Wrap QMessageBox::information function.",
		 "usage: qt.dialog.msg_information( \"text\" [ , \"title\", buttons, default_button ] )\n")
  {
    return Value(ls, QMessageBox::information(QApplication::activeWindow(),
					      get_arg<QString>(args, 1, ""),
					      get_arg<QString>(args, 0),
					      (QMessageBox::StandardButtons)get_arg<int>(args, 2, QMessageBox::Ok),
					      (QMessageBox::StandardButton)get_arg<int>(args, 3, QMessageBox::NoButton)));
  }


  QTLUA_FUNCTION(msg_question, "Wrap QMessageBox::question function.",
		 "usage: qt.dialog.msg_question( \"text\" [ , \"title\", buttons, default_button ] )\n")
  {
    return Value(ls, QMessageBox::question(QApplication::activeWindow(),
					   get_arg<QString>(args, 1, ""),
					   get_arg<QString>(args, 0),
					   (QMessageBox::StandardButtons)get_arg<int>(args, 2, QMessageBox::Ok),
					   (QMessageBox::StandardButton)get_arg<int>(args, 3, QMessageBox::NoButton)));
  }


  QTLUA_FUNCTION(msg_warning, "Wrap QMessageBox::warning function.",
		 "usage: qt.dialog.msg_warning( \"text\" [ , \"title\", buttons, default_button ] )\n")
  {
    return Value(ls, QMessageBox::warning(QApplication::activeWindow(),
					  get_arg<QString>(args, 1, ""),
					  get_arg<QString>(args, 0),
					  (QMessageBox::StandardButtons)get_arg<int>(args, 2, QMessageBox::Ok),
					  (QMessageBox::StandardButton)get_arg<int>(args, 3, QMessageBox::NoButton)));
  }


  QTLUA_FUNCTION(new_table_dialog, "Dynamically create a new QtLua::TableDialog.",
		 "usage: qt.dialog.new_table_dialog( table , viewtype, [ attributes ] )\n")
  {
    meta_call_check_args(args, 1, 3, Value::TNone, Value::TNumber, Value::TNumber);

    return Value(ls, new TableDialog(args[0],
				     (TableDialog::ViewType)get_arg<int>(args, 1), 0,
				     get_arg<int>(args, 2, 0), 0), true, true);
  }


  QTLUA_FUNCTION(tree_treeview, "Expose a lua table in a QTreeView.",
		 "usage: qt.dialog.tree_treeview( table [ , attributes, \"title\" ] )\n")
  {
    meta_call_check_args(args, 1, 3, Value::TNone, Value::TNumber, Value::TString);

    TableDialog::tree_tree_dialog(QApplication::activeWindow(),
				  get_arg<QString>(args, 2, ""), args[0],
				  (TableTreeModel::Attributes)get_arg<int>(args, 1, 0)
				  );

    return Value::List();
  }


  QTLUA_FUNCTION(tree_tableview, "Expose a lua table in a QTreeView.",
		 "usage: qt.dialog.tree_tableview( table [ , attributes, \"title\" ] )\n")
  {
    meta_call_check_args(args, 1, 3, Value::TNone, Value::TNumber, Value::TString);

    TableDialog::tree_table_dialog(QApplication::activeWindow(),
				   get_arg<QString>(args, 2, ""), args[0],
				   (TableTreeModel::Attributes)get_arg<int>(args, 1, 0)
				   );

    return Value::List();
  }


  QTLUA_FUNCTION(grid_tableview, "Expose 2 dimensions nested lua tables in a QTableView.",
		 "usage: qt.dialog.grid_tableview( table [ , attributes, \"title\", {column keys}, {row keys} ] )\n")
  {
    meta_call_check_args(args, 1, 5, Value::TNone, Value::TNumber,
			 Value::TString, Value::TTable, Value::TTable);
    Value::List rk, *rkptr = 0;
    Value::List ck, *ckptr = 0;

    if (args.count() >= 5)
      {
	rk = args[4].to_qlist<Value>();
	if (!rk.empty())
	  rkptr = &rk;
      }

    if (args.count() >= 4)
      {
	ck = args[3].to_qlist<Value>();
	if (!ck.empty())
	  ckptr = &ck;
      }

    TableDialog::grid_table_dialog(QApplication::activeWindow(),
				   get_arg<QString>(args, 2, ""), args[0],
				   (TableGridModel::Attributes)get_arg<int>(args, 1, 0),
				   ckptr, rkptr
				   );

    return Value::List();
  }


  //////////////////////////////////////////////////
   
  void qtluaopen_qt(State *ls)
  {
    ls->set_global("qt.meta", Value(ls, qt_meta));

    QTLUA_FUNCTION_REGISTER(ls, "qt.", connect               );
    QTLUA_FUNCTION_REGISTER(ls, "qt.", disconnect            );
    QTLUA_FUNCTION_REGISTER(ls, "qt.", load_ui               );
    QTLUA_FUNCTION_REGISTER(ls, "qt.", new_widget            );
    QTLUA_FUNCTION_REGISTER(ls, "qt.", new_qobject           );
    QTLUA_FUNCTION_REGISTER(ls, "qt.", layout_add            );
    QTLUA_FUNCTION_REGISTER(ls, "qt.", layout_spacer         );

    QTLUA_FUNCTION_REGISTER(ls, "qt.", tr                    );
    QTLUA_FUNCTION_REGISTER(ls, "qt.", translator            );

    QTLUA_FUNCTION_REGISTER(ls, "qt.menu.", add_toolbar      );
    QTLUA_FUNCTION_REGISTER(ls, "qt.menu.", add_menu         );
    QTLUA_FUNCTION_REGISTER(ls, "qt.menu.", add_separator    );
    QTLUA_FUNCTION_REGISTER(ls, "qt.menu.", add_action       );
    QTLUA_FUNCTION_REGISTER(ls, "qt.menu.", new_action_group );
    QTLUA_FUNCTION_REGISTER(ls, "qt.menu.", remove           );

    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", get_existing_directory);
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", get_open_filename     );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", get_open_filenames    );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", get_save_filename     );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", get_color             );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", get_double            );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", get_integer           );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", get_text              );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", get_item              );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", msg_about             );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", msg_critical          );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", msg_information       );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", msg_question          );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", msg_warning           );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", new_table_dialog      );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", tree_treeview         );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", tree_tableview        );
    QTLUA_FUNCTION_REGISTER(ls, "qt.dialog.", grid_tableview        );
  }

}

