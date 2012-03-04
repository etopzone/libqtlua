
#ifndef EXAMPLE_PLUGIN_HH_
#define EXAMPLE_PLUGIN_HH_

/* anchor 1 */
#include <QObject>
#include <QtLua/Plugin>

class ExamplePlugin : public QObject, public QtLua::PluginInterface
{
  Q_OBJECT
  Q_INTERFACES(QtLua::PluginInterface)

public:

  void register_members(QtLua::Plugin &plugin);
};
/* anchor end */

#endif

