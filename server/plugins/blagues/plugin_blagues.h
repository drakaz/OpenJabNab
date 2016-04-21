#ifndef _PLUGINBlagues_H_
#define _PLUGINBlagues_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "plugininterface.h"

class PluginBlagues : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	
private slots:
	void analyseXml();
	void analyseDone(bool, Bunny*, QByteArray);

public:
	PluginBlagues();
	virtual ~PluginBlagues();
	
	virtual bool Init();
	
	virtual bool OnClick(Bunny *, PluginInterface::ClickType);
	virtual void OnCron(Bunny * b, QVariant);
	virtual void OnBunnyConnect(Bunny *);
	virtual void OnBunnyDisconnect(Bunny *);

	// API
	virtual void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_SetFrequency);
	PLUGIN_BUNNY_API_CALL(Api_GetFrequency);

protected:
        void createCron(Bunny *);
        int GetRandomizedFrequency(unsigned int);

private:
	void getBlagues(Bunny *);
	QByteArray blaguesMessage;
};

#endif
