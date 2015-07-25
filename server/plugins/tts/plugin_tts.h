#ifndef _PLUGINTTS_H_
#define _PLUGINTTS_H_

#include "apimanager.h"
#include "plugininterface.h"

class PluginTTS : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	Q_PLUGIN_METADATA(IID "org.openjabnab.plugin.tts" FILE "tts.json");

public:
	PluginTTS();
	virtual ~PluginTTS() {};

	// API
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_Say);

private:
};

#endif
