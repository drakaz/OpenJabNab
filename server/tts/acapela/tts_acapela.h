#ifndef _TTSACAPELA_H_
#define _TTSACAPELA_H_

#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "ttsinterface.h"
	
class TTSAcapela : public TTSInterface
{
	Q_OBJECT
	Q_INTERFACES(TTSInterface)
	Q_PLUGIN_METADATA(IID "org.openjabnab.plugin.tts.acapela" FILE "tts_acapela.json");

public:
	TTSAcapela();
	virtual ~TTSAcapela();
	QByteArray CreateNewSound(QString, QString, bool);

private:
};

#endif
