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

    Additional Terms 7.b of GPLv3 applies to this file: Requiring
    preservation of specified reasonable legal notices or author
    attributions in that material or in the Appropriate Legal Notices
    displayed by works containing it;

    Copyright (C) 2012, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/

#include <stdio.h>

#include <QtXml>
#include <QTextStream>

#include "config.hh"

#define QTLUA_COPYRIGHT "QtLua user interface compiler " PACKAGE_VERSION " Copyright (C) 2012, Alexandre Becoulet"

QTextStream out(stdout);
QTextStream err(stderr);
QString input_filename;

static QTextStream & warning(const QDomNode &node)
{
  err << input_filename << ":" << node.lineNumber() << ":warning: ";
  return err;
}

static QString write_widget(QDomElement w, const QString &parent);

static void write_spacer(QDomElement s, const QString &parent)
{
  QString size = "0, 0";
  QString orientation = "Qt::Vertical";
  QString type = "QSizePolicy.Expanding";

  for (QDomNode n = s.firstChild(); !n.isNull(); n = n.nextSibling())
    {
      QDomElement s = n.toElement();

      if (s.isNull())
	continue;

      QString tagname(s.tagName());

      if (tagname == "property")
	{
	  QString prop(s.attribute("name"));
	  QDomElement v = s.firstChild().toElement();
	  QString tagname2(v.tagName());

	  if (prop == "orientation" && tagname2 == "enum")
	    {
	      orientation = v.text();
	    }
	  else if (prop == "sizeType" && tagname2 == "enum")
	    {
	      type = v.text().replace("::", ".");
	    }
	  else if (prop == "sizeHint" && tagname2 == "size")
	    {
	      size = v.elementsByTagName("width").at(0).toElement().text() + ", "
		+ v.elementsByTagName("height").at(0).toElement().text();
	    }
	  else
	    {
	      warning(v) << "skipped unsupported spacer property `" << prop << "'\n";
	    }
	}
    }

  if (orientation == "Qt::Vertical")
    out << "qt.layout_spacer(" << parent << ", " << size << ", qt.meta.QSizePolicy.Minimum, qt.meta." << type << ");\n";
  else if (orientation == "Qt::Horizontal")
    out << "qt.layout_spacer(" << parent << ", " << size << ", qt.meta." << type << ", qt.meta.QSizePolicy.Minimum);\n";
  else
    warning(s) << "bad spacer orientation `" << orientation << "'\n";
}

static void write_property(QDomElement s, const QString &parent)
{
  QDomElement v = s.firstChild().toElement();
  QString prop(s.attribute("name"));

  if (!v.isNull() && !prop.isEmpty())
    {
      QString tagname2(v.tagName());

      if (tagname2 == "bool" || tagname2 == "number")
	{
	  out << parent << "." << prop << " = " << v.text() << ";\n";
	}
      else if (tagname2 == "string")
	{
	  out << parent << "." << prop << " = \"" << v.text() << "\";\n";
	}
      else if (tagname2 == "rect")
	{
	  out << parent << "." << prop << " = { "
	      << v.elementsByTagName("x").at(0).toElement().text() << ", "
	      << v.elementsByTagName("y").at(0).toElement().text() << ", "
	      << v.elementsByTagName("width").at(0).toElement().text() << ", "
	      << v.elementsByTagName("height").at(0).toElement().text()<< " };\n";
	}
      else if (tagname2 == "size")
	{
	  out << parent << "." << prop << " = { "
	      << v.elementsByTagName("width").at(0).toElement().text() << ", "
	      << v.elementsByTagName("height").at(0).toElement().text()<< " };\n";
	}
      else if (tagname2 == "sizepolicy")
	{
	  out << parent << "." << prop << " = { "
	      << v.elementsByTagName("horstretch").at(0).toElement().text() << ", "
	      << v.elementsByTagName("verstretch").at(0).toElement().text()<< ", "
	      << "qt.meta.QSizePolicy." << v.attribute("hsizetype") << ", "
	      << "qt.meta.QSizePolicy." << v.attribute("vsizetype") << " };\n";
	}
      else if (tagname2 == "enum")
	{
	  out << parent << "." << prop << " = qt.meta." << v.text().replace("::", ".") << ";\n";
	}
      else if (tagname2 == "set")
	{
	  out << parent << "." << prop << " = qt.meta." << v.text().replace("::", ".").replace("|", " + qt.meta.") << ";\n";
	}
      else
	{
	  warning(v) << "skipped property `" << prop
		     << "' with unsupported type `" << tagname2 << "'\n";
	}
    }
}

static QString write_layout(QDomElement l, const QString &parent, bool add_layout)
{
  QString lclass(l.attribute("class"));

  static unsigned int layout_id = 0;
  QString lname = QString("_layout_") + QString::number(layout_id++);

  out << "\nlocal " << lname << " = qt.new_qobject(qt.meta." << lclass << ");" << "\n";

  if (add_layout)
    out << "qt.layout_add(" << parent << ", " << lname << ");\n";

  for (QDomNode n = l.firstChild(); !n.isNull(); n = n.nextSibling())
    {
      QDomElement s = n.toElement();

      if (s.isNull())
	continue;

      QString tagname(s.tagName());

      if (tagname == "item")
	{
	  QString row(s.attribute("row"));
	  QString col(s.attribute("column"));
	  QString row_span(s.attribute("rowspan", "1"));
	  QString col_span(s.attribute("colspan", "1"));
	  QString extra;
	  if (lclass == "QGridLayout")
	    extra = QString(", ") + row + ", " + col + ", " + row_span + ", " + col_span;
	  else if (lclass == "QFormLayout")
	    extra = QString(", ") + row + ", " + col + ", " + col_span;

	  for (QDomNode n = s.firstChild(); !n.isNull(); n = n.nextSibling())
	    {
	      QDomElement s = n.toElement();

	      if (s.isNull())
		continue;

	      QString tagname(s.tagName());

	      if (tagname == "widget")
		{
		  QString n = write_widget(s, parent);
		  out << "qt.layout_add(" << lname << ", " << n << extra << ");\n";
		}
	      else if (tagname == "layout")
		{
		  QString n = write_layout(s, parent, false);
		  out << "qt.layout_add(" << lname << ", " << n << extra << ");\n";
		}
	      else if (tagname == "spacer")
		{
		  write_spacer(s, lname);
		}
	      else
		{
		  warning(s) << "skipped unsupported layout item node `" << tagname << "'\n";
		}
	    }
	}
      else if (tagname == "property")
	{
	  write_property(s, lname);
	}
      else
	{
	  warning(s) << "skipped unsupported layout node `" << tagname << "'\n";
	}
    }

  return lname;
}

static QString write_widget(QDomElement w, const QString &parent)
{
  QString wclass(w.attribute("class").replace("::", "__"));
  QString wname(w.attribute("name"));

  out << "\nlocal " << wname << " = qt.new_qobject(qt.meta." << wclass << ");" << "\n";

  if (!parent.isEmpty())
    out << parent << "." << wname << " = " << wname << ";\n";

  for (QDomNode n = w.firstChild(); !n.isNull(); n = n.nextSibling())
    {
      QDomElement s = n.toElement();

      if (s.isNull())
	continue;

      QString tagname(s.tagName());

      if (tagname == "property")
	{
	  write_property(s, wname);
	}
      else if (tagname == "widget")
	{
	  write_widget(s, wname);
	}
      else if (tagname == "layout")
	{
	  write_layout(s, wname, true);
	}
      else
	{
	  warning(s) << "skipped unsupported widget node `" << tagname << "'\n";
	}
    }

  return wname;
}

static void write_connections(QDomElement c)
{
  for (QDomNode n = c.firstChild(); !n.isNull(); n = n.nextSibling())
    {
      QDomElement s = n.toElement();

      if (s.isNull())
	continue;

      QString tagname(s.tagName());

      if (tagname == "connection")
	{
	  out << "qt.connect("
	      << s.elementsByTagName("sender").at(0).toElement().text() << ", \""
	      << s.elementsByTagName("signal").at(0).toElement().text() << "\", "
	      << s.elementsByTagName("receiver").at(0).toElement().text() << ", \""
	      << s.elementsByTagName("slot").at(0).toElement().text() << "\");\n";
	}
      else
	{
	  warning(s) << "skipped unsupported connection node `" << tagname << "'\n";
	}
    }
}

int main(int argc, char *argv[])
{
  //  out.setDevice(stdout);
  out.setCodec("UTF-8");

  //  err.setDevice(stderr);
  err.setCodec("UTF-8");

  err << QTLUA_COPYRIGHT << "\n";

  //  QApplication app(argc, argv);

  if (argc < 2)
    {
      err << "usage: " << argv[0] << " file.ui" << "\n";
      return 1;
    }

  input_filename = argv[1];
  QFile f(input_filename);

  if (!f.open(QIODevice::ReadOnly))
    {
      err << "Can not open `" << argv[1] << "' ui file." << "\n";
      return 1;
    }

  QDomDocument dom;

  if (!dom.setContent(&f))
    {
      err << "Can not read `" << argv[1] << "' xml file." << "\n";
      return 1;
    }

  f.close();

  QString root;
  QDomElement e = dom.documentElement();

  out <<
    "-- -----------------------------------------------------------------------------\n"
    "-- Generated from UI file `" << input_filename << "'\n"
    "-- \n"
    "-- Created by QtLua user interface compiler version " PACKAGE_VERSION " \n"
    "-- \n"
    "-- WARNING! All changes made in this file will be lost when recompiling UI file!\n"
    "-- -----------------------------------------------------------------------------\n";

  for (QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling())
    {
      QDomElement s = n.toElement();

      if (s.isNull())
	continue;

      QString tagname(s.tagName());

      if (tagname == "class")
	{
	}
      else if (tagname == "widget")
	{
	  root = write_widget(s, "");
	}
      else if (tagname == "connections")
	{
	  write_connections(s);
	}
      else
	{
	  warning(s) << "skipped unsupported ui file section `" << tagname << "'" << "\n";
	}
    }

  out << "\nreturn " << root << ";\n";

  return 0;
}
