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

// __moc_flags__ -fQtLua/LuaModel

#ifndef QTLUA_CALLMODEL_HH_
#define QTLUA_CALLMODEL_HH_

#include <QAbstractItemModel>

#include "qtluavalue.hh"

namespace QtLua {

  /**
@code
    get(nil,
	item_id)     -- numerical id of queried item
	child_row,   -- row of child_id under item specified by item_id
	child_col)   -- column of child_id under item specified by item_id

item_rows,        -- number of rows under the parent specified by item_id
item_cols,        -- number of columns under the parent specified by item_id
child_id,         -- numerical id of children item at [child_row, child_col] in item_id
parent_id,        -- numerical id of parent of item specified by item_id
item_row,         -- row position of item specified by item_id in its parent starting at 1
item_col,         -- column position of item specified by item_id in its parent starting at 1
flags,            -- flags of item specified by item_id, default flags are used if not present

    get(data_role,     -- data role of requested child (LuaModel::ItemDataRole enum)
	item_id)       -- numerical id of queried item

item_data,     -- data of item specified by item_id, data is QVariant() if not present
data_type,     -- Qt type numeric handle of data, use simple Value::to_qvariant function if not present.


    set(role, item_id, value)
return true if ok.

    edit(check, parent_id, pos, count)
when check is true this function must return a boolean to indicate if an action is possible.

@end code

     The @tt item_id, @tt child_id and @tt parent_id values are
     integer numbers provided by the lua code used to identify items
     in the model. A value of @tt 0 is equivalent to an invalid @ref
     QModelIndex object.

     Valid column and row number start at 1 on lua side.

     When the @tt data_role value is @tt nil, the lua code may not
     return a valid @tt child_data value.

     For non-tree models, the lua code may only return 4 values.
  */
  class LuaModel : public QAbstractItemModel
  {
    Q_OBJECT;
    Q_ENUMS(ItemDataRole);

  public:

    LuaModel(const Value &get_func,
	      const Value &set_func = Value(),
	      const Value &insert_rows_func = Value(),
	      const Value &remove_rows_func = Value(),
	      const Value &insert_cols_func = Value(),
	      const Value &remove_cols_func = Value(),
	      QObject *parent = 0);

    enum ItemDataRole
      {
	DisplayRole =               ::Qt::DisplayRole,
	DecorationRole =	    ::Qt::DecorationRole,
	EditRole =		    ::Qt::EditRole,
	ToolTipRole =		    ::Qt::ToolTipRole,
	StatusTipRole =	            ::Qt::StatusTipRole,
	WhatsThisRole =	            ::Qt::WhatsThisRole,

	// Metadata
	FontRole =		    ::Qt::FontRole,
	TextAlignmentRole =	    ::Qt::TextAlignmentRole,
	BackgroundColorRole =	    ::Qt::BackgroundColorRole,
	BackgroundRole =	    ::Qt::BackgroundRole,
	TextColorRole =	            ::Qt::TextColorRole,
	ForegroundRole =	    ::Qt::ForegroundRole,
	CheckStateRole =	    ::Qt::CheckStateRole,

	// Accessibility
	AccessibleTextRole =	    ::Qt::AccessibleTextRole,
	AccessibleDescriptionRole = ::Qt::AccessibleDescriptionRole,

	// More general purpose
	SizeHintRole =	            ::Qt::SizeHintRole,
	InitialSortOrderRole =      ::Qt::InitialSortOrderRole,

	UserRole =                  ::Qt::UserRole
      };

  private:

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int row, int count, const QModelIndex& parent);
    bool removeRows(int row, int count, const QModelIndex& parent);
    bool insertColumns(int col, int count, const QModelIndex& parent);
    bool removeColumns(int col, int count, const QModelIndex& parent);
    void error(const String &err) const;

    Value _get;
    Value _set;
    Value _insert_rows;
    Value _remove_rows;
    Value _insert_cols;
    Value _remove_cols;
  };

}

#endif
