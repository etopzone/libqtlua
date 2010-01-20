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

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTreeView>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>

#include <QtLua/TableDialog>

namespace QtLua {

  TableDialog::TableDialog(QWidget *parent, const Value &root,
			   TableModel::Attributes attr, bool tableview)
    : QDialog(parent),
      _model(new TableModel(root, 0, attr))
  {
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    if (tableview)
      {
	QTableView *view = new QTableView();
	view->verticalHeader()->hide();
	_view = view;
      }
    else
      {
	QTreeView *view = new QTreeView();
	view->setRootIsDecorated(attr & TableModel::Recursive);
	_view = view;
	connect(view, SIGNAL(expanded(const QModelIndex&)),
		this, SLOT(expanded()));
      }

    if (attr & TableModel::Editable)
      {
	_eb = buttonBox->addButton("Edit", QDialogButtonBox::ActionRole);
	_eb->setEnabled(false);
	connect(_eb, SIGNAL(clicked()), this, SLOT(edit()));
	connect(_view, SIGNAL(clicked(const QModelIndex&)), this, SLOT(clicked(const QModelIndex&)));
      }

    _view->setModel(_model);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_view);
    layout->addWidget(buttonBox);

    setLayout(layout);
  }

  TableDialog::~TableDialog()
  {
    delete _model;
  }

  void TableDialog::expanded() const
  {
    for (int i = 0; i < 3; i++)
      static_cast<QTreeView*>(_view)->resizeColumnToContents(i);
  }

  void TableDialog::edit() const
  {
    if (_view->currentIndex().isValid())
      _view->edit(_view->currentIndex());
  }

  void TableDialog::clicked(const QModelIndex & index) const
  {
    _eb->setEnabled(_model->flags(index) & Qt::ItemIsEditable);
    // FIXME try to set current index to editable column in row
  }

  void TableDialog::table_dialog(QWidget *parent, const Value &root,
				 const QString &title, TableModel::Attributes attr,
				 bool tableview)
  {
    TableDialog d(parent, root, attr, tableview);
    d.setWindowTitle(title);
    d.exec();
  }

}

