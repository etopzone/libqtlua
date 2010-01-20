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

    Copyright (C) 2010, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/


#ifndef QTLUA_TABLEDIALOG_HH_
#define QTLUA_TABLEDIALOG_HH_

#include <QAbstractItemView>
#include <QDialog>

#include <QtLua/TableModel>

namespace QtLua {

  /**
   * @short Qt Model/View lua table view dialog
   * @header QtLua/TableDialog
   * @module {Model/View}
   *
   * This dialog class can be used to display lua tables easily in a
   * @ref QTreeView .
   *
   * The @ref QtLib lua library provides functions to display lua
   * tables using @ref TableDialog objects from lua code. 
   *
   * @see TableModel
   */

  class TableDialog : public QDialog
  {
    Q_OBJECT;
    Q_ENUMS(ColumnId);
    Q_FLAGS(ColumnIds);

  public:

    /** Specify lua table view columns */
    enum ColumnId
      {
	ColKey = 1,     //< Lua table key column
	ColType = 2,    //< Lua table value type name
	ColValue = 4,   //< Lua table value string representation using @ref Value::to_string_p
      };

    Q_DECLARE_FLAGS(ColumnIds, ColumnId);

    /** Create a table dialog. */
    TableDialog(QWidget *parent, const Value &root,
		TableModel::Attributes attr, bool tableview);
    ~TableDialog();

    /** Shortcut function to display a modal lua table dialog. */
    static void table_dialog(QWidget *parent, const Value &root, const QString &title,
			     TableModel::Attributes attr, ColumnIds hide = 0, bool tableview = false);

  private slots:
    void expanded() const;
    void edit() const;
    void clicked(const QModelIndex & index) const;

  private:

    QAbstractItemModel *_model;
    QAbstractItemView *_view;
    QPushButton *_eb;
  };

  Q_DECLARE_OPERATORS_FOR_FLAGS(TableDialog::ColumnIds);

}

#endif

