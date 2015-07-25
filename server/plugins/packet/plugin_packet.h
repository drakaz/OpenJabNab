#ifndef _PLUGINPACKET_H_
#define _PLUGINPACKET_H_

#include "plugininterface.h"
	
class PluginPacket : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	Q_PLUGIN_METADATA(IID "org.openjabnab.plugin.packet" FILE "packet.json");

public:
	PluginPacket();
	virtual ~PluginPacket() {};

	// API
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_SendPacket);
	PLUGIN_BUNNY_API_CALL(Api_SendMessage);
};

#endif
