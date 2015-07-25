#ifndef _PLUGINDICE_H_
#define _PLUGINDICE_H_

#include "plugininterface.h"
	
class PluginDice : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	Q_PLUGIN_METADATA(IID "org.openjabnab.plugin.dice" FILE "dice.json");

public:
	PluginDice();
	virtual ~PluginDice();
	bool OnClick(Bunny *, PluginInterface::ClickType);

};

#endif
