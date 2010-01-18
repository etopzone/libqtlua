
include $(top_srcdir)/build/autotroll.mk

noinst_PROGRAMS = itemtreeview tabletreeview

itemtreeview_SOURCES = itemtreeview.cc itemtreeview.moc.cc itemtreeview.hh
itemtreeview_CXXFLAGS = $(QT_CXXFLAGS) $(AM_CXXFLAGS)
itemtreeview_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS) -I$(top_srcdir)/src
itemtreeview_LDFLAGS  = $(QT_LDFLAGS) $(LDFLAGS) $(libtool_flags)
itemtreeview_LDADD   = $(QT_LIBS) $(LDADD) $(top_builddir)/src/libqtlua.la

BUILT_SOURCES = itemtreeview.moc.cc

tabletreeview_SOURCES = tabletreeview.cc tabletreeview.moc.cc tabletreeview.hh
tabletreeview_CXXFLAGS = $(QT_CXXFLAGS) $(AM_CXXFLAGS)
tabletreeview_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS) -I$(top_srcdir)/src
tabletreeview_LDFLAGS  = $(QT_LDFLAGS) $(LDFLAGS) $(libtool_flags)
tabletreeview_LDADD   = $(QT_LIBS) $(LDADD) $(top_builddir)/src/libqtlua.la

BUILT_SOURCES = tabletreeview.moc.cc

