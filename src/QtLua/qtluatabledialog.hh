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
   * This dialog class use the @ref TableModel class to display lua
   * tables in @ref QTreeView or @ref QTableView widgets.
   *
   * The @ref QtLib lua library provides functions to display lua
   * tables using @ref TableDialog objects from lua code.
   *
   * @see TableModel
   */

  class TableDialog : public QDialog
  {
    Q_OBJECT;

  public:

    /** Create a table dialog. */
    TableDialog(QWidget *parent, const Value &root,
		TableModel::Attributes attr, bool tableview);
    ~TableDialog();

    /**
     * Shortcut function to display a modal lua table dialog.
     *
     * @param parent parent widget
     * @param root lua table value to expose
     * @param attr model attributes, control display and edit options
     * @param tableview use a QTableView instead of a QTreeView when set
     */
    static void table_dialog(QWidget *parent, const Value &root, 
			     TableModel::Attributes attr = TableModel::Recursive,
			     const QString &title = "", bool tableview = false);

  private slots:
    void edit() const;
    void insert() const;
    void remove() const;
    void currentChanged(const QModelIndex & index) const;
    QModelIndex editable_index(const QModelIndex &index) const;

  protected:
    virtual QSize sizeHint() const;

    QAbstractItemModel *_model;
    QAbstractItemView *_view;
    QPushButton *_eb, *_rb, *_ib;
  };

}

#endif

