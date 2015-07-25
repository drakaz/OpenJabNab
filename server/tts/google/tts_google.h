#ifndef _TTSGOOGLE_H_
#define _TTSGOOGLE_H_

#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "ttsinterface.h"
	
class TTSGoogle : public TTSInterface
{
	Q_OBJECT
	Q_INTERFACES(TTSInterface)
	Q_PLUGIN_METADATA(IID "org.openjabnab.plugin.tts.google" FILE "tts_google.json");

public:
	TTSGoogle();
	virtual ~TTSGoogle();
	QByteArray CreateNewSound(QString, QString, bool);

private:
};

#endif
