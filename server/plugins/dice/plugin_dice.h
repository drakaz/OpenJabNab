#ifndef _PLUGINDICE_H_
#define _PLUGINDICE_H_

#include "plugininterface.h"
	
class PluginDice : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginDice();
	virtual ~PluginDice();
	bool OnAllClick(Bunny *);

};

#endif
