
ACLOCAL_AMFLAGS = -I build

SUBDIRS = src tools test doc

if EXAMPLES
SUBDIRS += examples
endif

EXTRA_DIST = bootstrap COPYING.LESSER CMakeLists.txt config.hh.cmake

doc: manual internals

manual:
	mkdoc -I src qt_4.5.mkdoclib --conf-file manual.conf src/QtLua/*.hh src/internal/*.hh

internals:
	mkdoc -I src qt_4.5.mkdoclib --conf-file internals.conf src/QtLua/*.hh src/internal/*.hh

www:
	mkdoc --conf-file web.conf

.PHONY: doc

