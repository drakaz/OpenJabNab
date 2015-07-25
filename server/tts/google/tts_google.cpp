#include <QDateTime>
#include <QUrl>
#include <QCryptographicHash>
#include <QMapIterator>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include "tts_google.h"
#include "log.h"

TTSGoogle::TTSGoogle():TTSInterface("google", "Google")
{
	voiceList.insert("fr", "fr");
}

TTSGoogle::~TTSGoogle()
{
}

QByteArray TTSGoogle::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{
	QEventLoop loop;

	if(!voiceList.contains(voice))
		voice = "fr";

	// Check (and create if needed) output folder
	QDir outputFolder = ttsFolder;
	if(!outputFolder.exists(voice))
		outputFolder.mkdir(voice);
	
	if(!outputFolder.cd(voice))
	{
		LogError(QString("Cant create TTS Folder : %1").arg(ttsFolder.absoluteFilePath(voice)));
		return QByteArray();
	}
	
	// Compute fileName
	QString fileName = QCryptographicHash::hash(text.toLatin1(), QCryptographicHash::Md5).toHex().append(".mp3");
	QString filePath = outputFolder.absoluteFilePath(fileName);

	if(!forceOverwrite && QFile::exists(filePath))
		return ttsHTTPUrl.arg(voice, fileName).toLatin1();

	// Fetch MP3
	QNetworkAccessManager *connection = new QNetworkAccessManager;
	QUrl qurl = QUrl("translate.google.com/translate_tts");
    QNetworkRequest requete(qurl);
    QNetworkReply *http = NULL;

	QObject::connect(http, SIGNAL(done(bool)), &loop, SLOT(quit()));

	QUrlQuery query;
	query.addQueryItem("ie", "UTF-8");
	query.addQueryItem("q", QUrl::toPercentEncoding(text));
	query.addQueryItem("tl", voice);

	QUrl post_data;
	post_data.setQuery(query);

	requete.setRawHeader("Host", "translate.google.com");

	http = connection->post(requete, post_data.toEncoded());

	loop.exec();

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		LogError("Cannot open sound file for writing");
		return QByteArray();
	}
	file.write(http->readAll());
	file.close();
	return ttsHTTPUrl.arg(voice, fileName).toLatin1();
}

