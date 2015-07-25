#ifndef _PLUGINSTATS_H_
#define _PLUGINSTATS_H_

#include "plugininterface.h"
	
class PluginStats : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	Q_PLUGIN_METADATA(IID "org.openjabnab.plugin.system.stats" FILE "stats.json");

public:
	PluginStats();
	virtual ~PluginStats();

	virtual void InitApiCalls();
	PLUGIN_API_CALL(Api_GetColors);
	PLUGIN_API_CALL(Api_GetPlugins);
	PLUGIN_API_CALL(Api_GetBunniesIP);
	PLUGIN_API_CALL(Api_GetBunniesName);
	PLUGIN_API_CALL(Api_GetBunniesStatus);
        PLUGIN_API_CALL(Api_GetBunniesInformation);
	
};

#endif
