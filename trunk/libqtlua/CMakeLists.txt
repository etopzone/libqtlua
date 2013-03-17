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
#	SET(QT_USE_QTUITOOLS 1)
#	SET(QT_USE_QTXML 1)
#	FIND_PACKAGE(Qt4 REQUIRED)
#	INCLUDE(${QT_USE_FILE})

# Find Qt5
	SET(CMAKE_INCLUDE_CURRENT_DIR ON)
	FIND_PACKAGE(Qt5Widgets)
	FIND_PACKAGE(Qt5Core)
	FIND_PACKAGE(Qt5Xml)
	SET(CMAKE_AUTOMOC ON)

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
		CHECK_FUNCTION_EXISTS(luaopen_jit HAVE_LUA_JITLIB)
		CHECK_FUNCTION_EXISTS(luaopen_ffi HAVE_LUA_FFILIB)

		CHECK_INCLUDE_FILES("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)
		SET(SHLIBEXT ${CMAKE_SHARED_LIBRARY_SUFFIX})

 		# LT_OBJDIR

		SET(PACKAGE_NAME "\"libqtlua\"")
		SET(PACKAGE_TARNAME "\"libqtlua\"")
		SET(PACKAGE_VERSION "\"2.0\"")
		SET(PACKAGE_STRING "\"libqtlua 2.0\"")
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
		src/qtluauseritem.cc src/qtluauseritemmodel.cc
		src/qtluauseritemselectionmodel.cc src/qtluauserlistitem.cc
		src/qtlualistiterator.cc src/qtluamember.cc
		src/qtluametacache.cc src/qtluamethod.cc
		src/qtluaplugin.cc src/qtluaproperty.cc
		src/qtluaqmetaobjecttable.cc src/qtluaqmetavalue.cc
		src/qtluaqmetaobjectwrapper.cc src/qtlualuamodel.cc
		src/qtluaqobjectiterator.cc src/qtluaqobjectwrapper.cc
		src/qtluaqtlib.cc src/qtluastate.cc
		src/qtluaitemviewdialog.cc src/qtluatablegridmodel.cc
		src/qtluatableiterator.cc src/qtluatabletreekeys.cc
		src/qtluatabletreemodel.cc src/qtluauserdata.cc
		src/qtluavaluebase.cc src/qtluavalue.cc src/qtluavalueref.cc
		src/qtluadispatchproxy.cc )

	# Generate moc files
 
	QT5_WRAP_CPP(MOC_OUTFILES src/QtLua/qtluaconsole.hh        OPTIONS -fQtLua/Console)
	QT5_WRAP_CPP(MOC_OUTFILES src/QtLua/qtluauseritemselectionmodel.hh OPTIONS -fQtLua/UserItemSelectionModel)
	QT5_WRAP_CPP(MOC_OUTFILES src/QtLua/qtluauseritemmodel.hh  OPTIONS -fQtLua/UserItemModel)
	QT5_WRAP_CPP(MOC_OUTFILES src/QtLua/qtlualuamodel.hh      OPTIONS  -fQtLua/LuaModel)
	QT5_WRAP_CPP(MOC_OUTFILES src/QtLua/qtluatabletreemodel.hh OPTIONS -fQtLua/TableTreeModel)
	QT5_WRAP_CPP(MOC_OUTFILES src/QtLua/qtluaitemviewdialog.hh	   OPTIONS -fQtLua/ItemViewDialog)
	QT5_WRAP_CPP(MOC_OUTFILES src/QtLua/qtluatablegridmodel.hh OPTIONS -fQtLua/TableGridModel)
	QT5_WRAP_CPP(MOC_OUTFILES src/QtLua/qtluastate.hh          OPTIONS -fQtLua/State)
	QT5_WRAP_CPP(MOC_OUTFILES src/qtluaqtlib.hh)
	
	ADD_LIBRARY(qtlua SHARED ${QTLUA_LIB_SRCS} ${MOC_OUTFILES})
	QT5_USE_MODULES(qtlua Widgets Core)

	TARGET_LINK_LIBRARIES(qtlua ${QT_LIBRARIES} ${LUA_LIBRARIES})

# qtlua app
	ADD_EXECUTABLE(qtlua_app tools/qtlua/qtlua.cc )
	QT5_USE_MODULES(qtlua_app Widgets Core)

	TARGET_LINK_LIBRARIES(qtlua_app qtlua)
	SET_TARGET_PROPERTIES(qtlua_app  PROPERTIES OUTPUT_NAME qtlua CLEAN_DIRECT_OUTPUT 1)

# qtlua_uic app
	ADD_EXECUTABLE(qtlua_uic_app tools/uic/qtlua_uic.cc )
	QT5_USE_MODULES(qtlua_uic_app Widgets Core Xml)

	TARGET_LINK_LIBRARIES(qtlua_uic_app qtlua)
	SET_TARGET_PROPERTIES(qtlua_uic_app  PROPERTIES OUTPUT_NAME qtlua_uic CLEAN_DIRECT_OUTPUT 1)

# library and app install
	INSTALL(TARGETS qtlua_app qtlua_uic_app qtlua RUNTIME DESTINATION ${INSTALL_BIN}
			LIBRARY DESTINATION ${INSTALL_LIB}
			ARCHIVE DESTINATION ${INSTALL_LIB})

        INSTALL(FILES
                src/QtLua/Function           src/QtLua/qtluafunction.hh        src/QtLua/qtluafunction.hxx 
                src/QtLua/Console            src/QtLua/qtluaconsole.hh         src/QtLua/qtluaconsole.hxx 
                src/QtLua/TableTreeModel     src/QtLua/qtluatabletreemodel.hh  src/QtLua/qtluatabletreemodel.hxx
                src/QtLua/ItemViewDialog     src/QtLua/qtluaitemviewdialog.hh  src/QtLua/qtluaitemviewdialog.hxx
                src/QtLua/TableGridModel     src/QtLua/qtluatablegridmodel.hh  src/QtLua/qtluatablegridmodel.hxx
                src/QtLua/LuaModel           src/QtLua/qtlualuamodel.hh        src/QtLua/qtlualuamodel.hxx
                src/QtLua/UserItem           src/QtLua/qtluauseritem.hh        src/QtLua/qtluauseritem.hxx
                src/QtLua/UserListItem       src/QtLua/qtluauserlistitem.hh    src/QtLua/qtluauserlistitem.hxx 
                src/QtLua/UserItemModel      src/QtLua/qtluauseritemmodel.hh   src/QtLua/qtluauseritemmodel.hxx
                src/QtLua/UserItemSelectionModel src/QtLua/qtluauseritemselectionmodel.hh src/QtLua/qtluauseritemselectionmodel.hxx 
                src/QtLua/Iterator           src/QtLua/qtluaiterator.hh        src/QtLua/qtluaiterator.hxx 
                src/QtLua/Plugin             src/QtLua/qtluaplugin.hh          src/QtLua/qtluaplugin.hxx 
                src/QtLua/State              src/QtLua/qtluastate.hh           src/QtLua/qtluastate.hxx 
                src/QtLua/UserData           src/QtLua/qtluauserdata.hh        src/QtLua/qtluauserdata.hxx 
                src/QtLua/UserObject         src/QtLua/qtluauserobject.hh      src/QtLua/qtluauserobject.hxx 
                src/QtLua/ValueBase          src/QtLua/qtluavaluebase.hh       src/QtLua/qtluavaluebase.hxx
                src/QtLua/Value              src/QtLua/qtluavalue.hh           src/QtLua/qtluavalue.hxx 
                src/QtLua/ValueRef           src/QtLua/qtluavalueref.hh        src/QtLua/qtluavalueref.hxx 
                src/QtLua/Ref                src/QtLua/qtluaref.hh 
                src/QtLua/String             src/QtLua/qtluastring.hh          src/QtLua/qtluastring.hxx 
                src/QtLua/QHashProxy         src/QtLua/qtluaqhashproxy.hh      src/QtLua/qtluaqhashproxy.hxx 
                src/QtLua/QVectorProxy       src/QtLua/qtluaqvectorproxy.hh    src/QtLua/qtluaqvectorproxy.hxx 
                src/QtLua/QListProxy         src/QtLua/qtluaqlistproxy.hh      src/QtLua/qtluaqlistproxy.hxx 
                src/QtLua/QLinkedListProxy   src/QtLua/qtluaqlinkedlistproxy.hh src/QtLua/qtluaqlinkedlistproxy.hxx 
                src/QtLua/ArrayProxy         src/QtLua/qtluaarrayproxy.hh      src/QtLua/qtluaarrayproxy.hxx 
                src/QtLua/MetaType           src/QtLua/qtluametatype.hh        src/QtLua/qtluametatype.hxx 
                src/QtLua/DispatchProxy      src/QtLua/qtluadispatchproxy.hh   src/QtLua/qtluadispatchproxy.hxx
        DESTINATION ${INSTALL_INC}/QtLua)

