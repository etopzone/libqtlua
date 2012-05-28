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

    Copyright (C) 2008, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/


#ifndef QTLUAFUNCTION_HH_
#define QTLUAFUNCTION_HH_

#include "qtluauserdata.hh"
#include "qtluavalue.hh"
#include "qtluaplugin.hh"

namespace QtLua {

  class State;

  /** 
   * @short Functions like objects base class
   * @header QtLua/Function
   * @module {Functions export}
   *
   * This class is a convenient base class for exposing functions like
   * objects to lua scripts. It's based on the @ref UserData class and
   * is handled by lua as an userdata value with redefined call operation.
   *
   * Basic argument checking can be done using the @ref
   * QtLua::UserData::meta_call_check_args function. More argument
   * checking and conversion can be performed with the @ref __get_arg1__
   * function. See @xref{Qt/Lua types conversion} for supported
   * types and conversion operations.
   *
   * This class allows definition of function description and help
   * strings which are useful to the @ref QtLua::Console user.
   *
   * Defining a new function object is done by inheriting from this
   * class and implementing the @ref meta_call function:
   *
   * @example examples/cpp/userdata/function.cc:1|6|3
   *
   * @ref Function objects can be exposed as a lua values or
   * registered on a @ref Plugin object. The @ref __register_1__
   * function and @ref #QTLUA_FUNCTION macro offer convenient
   * ways to register a @ref Function object in a lua table or on a
   * @ref Plugin object.
   */

  class Function : public UserData
  {
  public:
    QTLUA_REFTYPE(Function);

    /**
     * @alias register_1
     * This function is provided for convenience and may be used to
     * register the @ref Function object in lua global table or in
     * package subtables. All intermediate tables in path will be
     * created as needed.
     *
     * @param ls QtLua state where function must be registered.
     * @param path table path to function value.
     *
     * Example:
     * @example examples/cpp/userdata/function.cc:4
     */
    void register_(State *ls, const String &path);

    /** @internal @see Plugin */
    void register_(Plugin &plugin, const String &name);

    /** This function may be reimplemented to return a short
	description of the function. */
    virtual String get_description() const;

    /** This function may be reimplemented to return a function usage
	help string. */
    virtual String get_help() const;

    /** This macro declare a new a @ref Function class named
	@tt{QtLua_Function_}@em{name} with given description, help and
	code for reimplementation of the @ref UserData::meta_call function.

	@code
QTLUA_FUNCTION(foo, "The foo function", "No help available")
{
  Q_UNUSED(args);
  return QtLua::Value(ls, "foo");
}
	@end code
    */
#define QTLUA_FUNCTION(name, description, help)				\
class QtLua_Function_##name : public QtLua::Function			\
  {									\
    QtLua::Value::List meta_call(QtLua::State *ls, const QtLua::Value::List &args); \
    QtLua::String get_description() const { return description; }	\
    QtLua::String get_help() const { return help; }			\
  };									\
									\
 QtLua::Value::List QtLua_Function_##name				\
   ::meta_call(QtLua::State *ls, const QtLua::Value::List &args)

  protected:

    virtual Value::List meta_call(State *ls, const Value::List &args) = 0;

  public:

    /**
     * This function may be called from the @ref meta_call function to
     * perform lua to C++ argument conversion and checking.
     *
     * It checks if the argument is available and tries to convert
     * argument to @tt X type and throw if conversion fails. A default
     * value is returned if no argument exists at specified index.
     *
     * @param args arguments list
     * @param n argument index in list
     * @param default_ default value to return if no argument available
     * @returns C++ converted value
     *
     * Example:
     * @example examples/cpp/userdata/function.cc:6|5
     *
     * @xsee{Qt/Lua types conversion}
     * @see __get_arg2__
     * @alias get_arg1
     */
    template <class X>
    static inline X get_arg(const Value::List &args, int n, const X & default_);

    /**
     * This function does the same as the @ref __get_arg1__ function
     * but throws if argument is not available instead of returning a
     * default value.
     *
     * @see __get_arg1__
     * @see get_arg_ud
     * @alias get_arg2
     */
    template <class X>
    static inline X get_arg(const Value::List &args, int n);

    /**
     * This function may be called from the @ref meta_call function to
     * perform lua to C++ argument conversion and checking.
     *
     * It checks if the argument is available and if it is an @ref
     * UserData object and tries to cast it using the
     * Value::to_userdata_cast function.
     *
     * @param args arguments list
     * @param n argument index in list
     * @returns @ref Ref pointer to @tt X type.
     *
     * @xsee{Qt/Lua types conversion}
     * @see __get_arg2__
     */
    template <class X>
    static inline Ref<X> get_arg_ud(const Value::List &args, int n);

  private:
    String get_value_str() const;
    String get_type_name() const;
    bool support(Value::Operation c) const;
    void completion_patch(String &path, String &entry, int &offset);
  };

}

#endif

