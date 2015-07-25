#ifndef _PLUGINEARS_H_
#define _PLUGINEARS_H_

#include "plugininterface.h"
	
class PluginEars : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	Q_PLUGIN_METADATA(IID "org.openjabnab.plugin.ears" FILE "ears.json");

public:
	PluginEars();
	virtual ~PluginEars();
	bool OnEarsMove(Bunny *, int, int);

	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_getFriend);
	PLUGIN_BUNNY_API_CALL(Api_setFriend);
	
};

#endif
