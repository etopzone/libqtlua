# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec, modified by Alexandre Becoulet for qtlua mainstream

# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

	PROJECT(qtlua)
	CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

	SET(INSTALL_BIN bin CACHE PATH "Where to install binaries to.")
	SET(INSTALL_LIB lib CACHE PATH "Where to install libraries to.")
	SET(INSTALL_INC include CACHE PATH "Where to install headers to.")

# Find Qt4
	SET(QT_USE_QTUITOOLS 1)
	FIND_PACKAGE(Qt4 REQUIRED)
	INCLUDE(${QT_USE_FILE})

# Find Lua 
	FIND_PACKAGE (Lua51 REQUIRED)
	INCLUDE_DIRECTORIES (${LUA_INCLUDE_DIR})	

# qtlua library
	
	###########################################################################
	# generating config.h file:
	MESSAGE("Configure: qtlua - generating config.hh:")
		INCLUDE (CheckIncludeFiles)
		INCLUDE (CheckFunctionExists)
		INCLUDE (CheckSymbolExists)
		INCLUDE(CheckLibraryExists)

		CHECK_INCLUDE_FILES (dlfcn.h HAVE_DLFCN_H)
		CHECK_INCLUDE_FILES (inttypes.h HAVE_INTTYPES_H)
		CHECK_INCLUDE_FILES (memory.h HAVE_MEMORY_H)
		CHECK_INCLUDE_FILES (stdint.h HAVE_STDINT_H)
		CHECK_INCLUDE_FILES (stdlib.h HAVE_STDLIB_H)
		CHECK_INCLUDE_FILES (strings.h HAVE_STRINGS_H)
		CHECK_INCLUDE_FILES (string.h HAVE_STRING_H)
		CHECK_INCLUDE_FILES ("sys/stat.h" HAVE_SYS_STAT_H)
		CHECK_INCLUDE_FILES ("sys/types.h" HAVE_SYS_TYPES_H)
		CHECK_INCLUDE_FILES (unistd.h HAVE_UNISTD_H)
		
		SET(CMAKE_REQUIRED_INCLUDES ${LUA_INCLUDE_DIR})
		SET(CMAKE_REQUIRED_LIBRARIES ${LUA_LIBRARIES})
		CHECK_FUNCTION_EXISTS(lua_gc HAVE_LUA_GC)
		CHECK_FUNCTION_EXISTS(luaL_openlib HAVE_LUA_OPENLIBS)
		CHECK_FUNCTION_EXISTS(luaopen_os HAVE_LUA_OSLIB)
		CHECK_FUNCTION_EXISTS(luaopen_package HAVE_LUA_PACKAGELIB)

		CHECK_INCLUDE_FILES("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)
		SET(SHLIBEXT ${CMAKE_SHARED_LIBRARY_SUFFIX})

 		# LT_OBJDIR

		SET(PACKAGE_NAME "\"libqtlua\"")
		SET(PACKAGE_TARNAME "\"libqtlua\"")
		SET(PACKAGE_VERSION "\"1.3\"")
		SET(PACKAGE_STRING "\"libqtlua 1.3\"")
		SET(PACKAGE_BUGREPORT "\"\"")
		SET(PACKAGE_URL "\"http://www.nongnu.org/libqtlua/\"")

		# all is defined, so generate config.hh
		CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.hh.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.hh)

	MESSAGE("Configure: qtlua - done.")
	# end of generating config.hh file

	ADD_DEFINITIONS(-DHAVE_CONFIG_H)
	ADD_DEFINITIONS(-DQT_NO_CAST_FROM_BYTEARRAY)
	ADD_DEFINITIONS(-DQTLUAREF_DEBUG)

	INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR})
	INCLUDE_DIRECTORIES( src )

	SET(QTLUA_LIB_SRCS src/qtluaconsole.cc src/qtluaenum.cc
		src/qtluaenumiterator.cc src/qtluafunction.cc
		src/qtluaitem.cc src/qtluaitemmodel.cc
		src/qtluaitemselectionmodel.cc src/qtlualistitem.cc
		src/qtlualistiterator.cc src/qtluamember.cc
		src/qtluametacache.cc src/qtluamethod.cc
		src/qtluaplugin.cc src/qtluaproperty.cc
		src/qtluaqmetaobjecttable.cc
		src/qtluaqmetaobjectwrapper.cc
		src/qtluaqobjectiterator.cc src/qtluaqobjectwrapper.cc
		src/qtluaqtlib.cc src/qtluastate.cc
		src/qtluatabledialog.cc src/qtluatablegridmodel.cc
		src/qtluatableiterator.cc src/qtluatabletreekeys.cc
		src/qtluatabletreemodel.cc src/qtluauserdata.cc
		src/qtluavalue.cc src/qtluavalueref.cc src/qtluadispatchproxy.cc )

	# Generate moc files
	SET(MOC_HEADERS	
		src/QtLua/qtluaconsole.hh src/QtLua/qtluaitemselectionmodel.hh 
		src/QtLua/qtluaitemmodel.hh src/QtLua/qtluatabletreemodel.hh 
		src/QtLua/qtluatabledialog.hh src/QtLua/qtluastate.hh	
		src/QtLua/qtluatablegridmodel.hh			
		src/qtluaqtlib.hh	
	)
	
	QT4_WRAP_CPP(MOC_OUTFILES ${MOC_HEADERS})
	
	ADD_LIBRARY(qtlua SHARED ${QTLUA_LIB_SRCS} ${MOC_OUTFILES})
	TARGET_LINK_LIBRARIES(qtlua ${QT_LIBRARIES} ${LUA_LIBRARIES})

# qtlua app
	ADD_EXECUTABLE(qtlua_app tools/qtlua/qtlua.cc )
	TARGET_LINK_LIBRARIES(qtlua_app qtlua)
	SET_TARGET_PROPERTIES(qtlua_app  PROPERTIES OUTPUT_NAME qtlua CLEAN_DIRECT_OUTPUT 1)

# library and app install
	INSTALL(TARGETS qtlua_app qtlua RUNTIME DESTINATION ${INSTALL_BIN}
			LIBRARY DESTINATION ${INSTALL_LIB}
			ARCHIVE DESTINATION ${INSTALL_LIB})

        INSTALL(FILES src/QtLua/Function src/QtLua/qtluafunction.hh
		      src/QtLua/qtluafunction.hxx src/QtLua/Console
		      src/QtLua/qtluaconsole.hh
		      src/QtLua/qtluaconsole.hxx src/QtLua/Item
		      src/QtLua/qtluaitem.hh src/QtLua/qtluaitem.hxx
		      src/QtLua/TableTreeModel
		      src/QtLua/qtluatabletreemodel.hh
		      src/QtLua/TableDialog
		      src/QtLua/qtluatabledialog.hh
		      src/QtLua/qtluatabledialog.hxx
		      src/QtLua/TableGridModel
		      src/QtLua/qtluatablegridmodel.hh
		      src/QtLua/ItemModel src/QtLua/qtluaitemmodel.hh
		      src/QtLua/ItemSelectionModel
		      src/QtLua/qtluaitemselectionmodel.hh
		      src/QtLua/qtluaitemselectionmodel.hxx
		      src/QtLua/Iterator src/QtLua/qtluaiterator.hh
		      src/QtLua/qtluaiterator.hxx src/QtLua/ListItem
		      src/QtLua/qtlualistitem.hh
		      src/QtLua/qtlualistitem.hxx src/QtLua/Plugin
		      src/QtLua/qtluaplugin.hh
		      src/QtLua/qtluaplugin.hxx
		      src/QtLua/PluginInterface
		      src/QtLua/qtluaplugininterface.hh
		      src/QtLua/State src/QtLua/qtluastate.hh
		      src/QtLua/qtluastate.hxx src/QtLua/UserData
		      src/QtLua/qtluauserdata.hh
		      src/QtLua/qtluauserdata.hxx src/QtLua/UserObject
		      src/QtLua/qtluauserobject.hh
		      src/QtLua/qtluauserobject.hxx src/QtLua/Value
		      src/QtLua/qtluavalue.hh src/QtLua/qtluavalue.hxx
		      src/QtLua/ValueRef src/QtLua/qtluavalueref.hh
		      src/QtLua/qtluavalueref.hxx src/QtLua/Ref
		      src/QtLua/qtluaref.hh src/QtLua/String
		      src/QtLua/qtluastring.hh
		      src/QtLua/qtluastring.hxx src/QtLua/QHashProxy
		      src/QtLua/qtluaqhashproxy.hh
		      src/QtLua/qtluaqhashproxy.hxx
		      src/QtLua/QVectorProxy
		      src/QtLua/qtluaqvectorproxy.hh
		      src/QtLua/qtluaqvectorproxy.hxx
		      src/QtLua/QListProxy
		      src/QtLua/qtluaqlistproxy.hh
		      src/QtLua/qtluaqlistproxy.hxx
		      src/QtLua/QLinkedListProxy
		      src/QtLua/qtluaqlinkedlistproxy.hh
		      src/QtLua/qtluaqlinkedlistproxy.hxx
              DESTINATION ${INSTALL_INC}/QtLua)
	
# tests
#	INCLUDE_DIRECTORIES( test )
#
#	ADD_EXECUTABLE(test_value test/test_value.cc)
#	TARGET_LINK_LIBRARIES(test_value qtlua)
#
#	ADD_EXECUTABLE(test_table   test/test_table.cc)
#	TARGET_LINK_LIBRARIES(test_table qtlua)
#
#	# Generate moc files
#	QT4_WRAP_CPP(MOC_OUTFILES_TEST  test/test_qobject_arg.hh)
#	ADD_EXECUTABLE(test_qobject_arg   test/test_qobject_arg.cc ${MOC_OUTFILES_TEST})
#	TARGET_LINK_LIBRARIES(test_qobject_arg qtlua)
#
#	ADD_EXECUTABLE(test_item test/test_item.cc)
#	TARGET_LINK_LIBRARIES(test_item qtlua)


# examples
#	# console
#	QT4_WRAP_CPP(MOC_OUTFILES_CONSOLE  examples/cpp/console/console.hh)
#	ADD_EXECUTABLE(console examples/cpp/console/console_fcn.cc ${MOC_OUTFILES_CONSOLE})
#	TARGET_LINK_LIBRARIES(console qtlua)
#
#	# mvc
#	QT4_WRAP_CPP(MOC_OUTFILES_TREEVIEW  examples/cpp/mvc/itemtreeview.hh)
#	ADD_EXECUTABLE(itemtreeview examples/cpp/mvc/itemtreeview.cc ${MOC_OUTFILES_TREEVIEW})
#	TARGET_LINK_LIBRARIES(itemtreeview qtlua)
#
#	QT4_WRAP_CPP(MOC_OUTFILES_TABLEGRIDVIEW  examples/cpp/mvc/tablegridview.hh)
#	ADD_EXECUTABLE(tablegridview examples/cpp/mvc/tablegridview.cc ${MOC_OUTFILES_TABLEGRIDVIEW})
#	TARGET_LINK_LIBRARIES(tablegridview qtlua)
#	
#	QT4_WRAP_CPP(MOC_OUTFILES_TABLETREEVIEW  examples/cpp/mvc/tabletreeview.hh)
#	ADD_EXECUTABLE(tabletreeview examples/cpp/mvc/tabletreeview.cc ${MOC_OUTFILES_TABLETREEVIEW})
#	TARGET_LINK_LIBRARIES(tabletreeview qtlua)
#
#	QT4_WRAP_CPP(MOC_OUTFILES_PLUGIN  examples/cpp/plugin/plugin.hh)
#	ADD_LIBRARY(plugin MODULE examples/cpp/plugin/plugin.cc ${MOC_OUTFILES_PLUGIN})
#	SET_TARGET_PROPERTIES ( plugin PROPERTIES PREFIX "" )
#	
#	# plugin
#	ADD_EXECUTABLE(plugin_app examples/cpp/plugin/plugin_load.cc  )
#	TARGET_LINK_LIBRARIES(plugin_app qtlua)
#	SET_TARGET_PROPERTIES ( plugin_app PROPERTIES OUTPUT_NAME plugin CLEAN_DIRECT_OUTPUT 1 )
#
#	# proxy
#	ADD_EXECUTABLE(qhashproxy_value examples/cpp/proxy/qhashproxy_value.cc)
#	TARGET_LINK_LIBRARIES(qhashproxy_value qtlua)
#	ADD_EXECUTABLE(qlinkedlistproxy_string examples/cpp/proxy/qlinkedlistproxy_string.cc)
#	TARGET_LINK_LIBRARIES(qlinkedlistproxy_string qtlua)
#	ADD_EXECUTABLE(qlistproxy_string examples/cpp/proxy/qlistproxy_string.cc)
#	TARGET_LINK_LIBRARIES(qlistproxy_string qtlua)	
#	ADD_EXECUTABLE(qmapproxy_string examples/cpp/proxy/qmapproxy_string.cc)
#	TARGET_LINK_LIBRARIES(qmapproxy_string qtlua)
#	ADD_EXECUTABLE(qvectorproxy_string examples/cpp/proxy/qvectorproxy_string.cc)
#	TARGET_LINK_LIBRARIES(qvectorproxy_string qtlua)	
#	ADD_EXECUTABLE(qvectorproxy_userdata examples/cpp/proxy/qvectorproxy_userdata.cc)
#	TARGET_LINK_LIBRARIES(qvectorproxy_userdata qtlua)
#
#	# qobject
#	ADD_EXECUTABLE(qobject_iter examples/cpp/qobject/qobject_iter.cc)
#	TARGET_LINK_LIBRARIES(qobject_iter qtlua)
#	ADD_EXECUTABLE(qobject_owner examples/cpp/qobject/qobject_owner.cc)
#	TARGET_LINK_LIBRARIES(qobject_owner  qtlua)
#
#	# userdata
#	ADD_EXECUTABLE(function examples/cpp/userdata/function.cc)
#	TARGET_LINK_LIBRARIES(function qtlua)
#	ADD_EXECUTABLE(ref examples/cpp/userdata/ref.cc)
#	TARGET_LINK_LIBRARIES(ref qtlua)
#	
#	# value
#	ADD_EXECUTABLE(global examples/cpp/value/global.cc)
#	TARGET_LINK_LIBRARIES(global qtlua)
#	ADD_EXECUTABLE(iterate examples/cpp/value/iterate.cc)
#	TARGET_LINK_LIBRARIES(iterate qtlua)
#
#	# types
#	QT4_WRAP_CPP(MOC_OUTFILES_TYPES  examples/cpp/types/myobject.hh)
#	ADD_EXECUTABLE(meta examples/cpp/types/meta.cc ${MOC_OUTFILES_TYPES})
#	TARGET_LINK_LIBRARIES(meta qtlua)

