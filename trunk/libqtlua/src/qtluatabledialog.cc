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
#include <QtLua/TableDialog>

namespace QtLua {

  TableDialog::TableDialog(QWidget *parent, const Value &root, bool recursive, bool editable)
    : QDialog(parent),
      _model(new TableModel(root, 0, recursive))
  {
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    _view = new QTreeView();
    _view->setRootIsDecorated(recursive);
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

  void TableDialog::table_dialog(QWidget *parent, const Value &root,
				 const QString &title, bool recursive,
				 bool editable, ColumnIds hide)
  {
    TableDialog d(parent, root, recursive, editable);
    d.setWindowTitle(title);

    for (int i = 0; i < 3; i++)
      if (hide & (1 << i))
	d._view->hideColumn(i);

    d.exec();
  }

}

