#include <QDateTime>
#include <QUrl>
#include <QCryptographicHash>
#include <QMapIterator>
#include "tts_acapela.h"
#include "log.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QObject>
#include <QUrlQuery>

TTSAcapela::TTSAcapela():TTSInterface("acapela", "Acapela")
{
	// Arabic
	voiceList.insert("Leila", "sonid0");
	voiceList.insert("Mehdi", "sonid0");
	voiceList.insert("Nizar", "sonid0");
	voiceList.insert("Salma", "sonid0");
	// English (AU)
	voiceList.insert("Liam", "sonid6");
	voiceList.insert("Lisa", "sonid6");
	voiceList.insert("Olivia", "sonid6");
	voiceList.insert("Tyler", "sonid6");
	// English (India)
	voiceList.insert("Deepa", "sonid7");
	// English (UK)
	voiceList.insert("Graham", "sonid8");
	voiceList.insert("Harry", "sonid8");
	voiceList.insert("Lucy", "sonid8");
	voiceList.insert("Nizareng", "sonid8");
	voiceList.insert("Peter", "sonid8");
	voiceList.insert("PeterHappy", "sonid8");
	voiceList.insert("PeterSad", "sonid8");
	voiceList.insert("QueenElizabeth", "sonid8");
	voiceList.insert("Rachel", "sonid8");
	voiceList.insert("Rosie", "sonid8");
	// French (Belgium)
	voiceList.insert("Justine", "sonid11");
	// French (Canada)
	voiceList.insert("Louise", "sonid12");
	// French (France)
	voiceList.insert("Alice", "sonid13");
	voiceList.insert("Antoine", "sonid13");
	voiceList.insert("AntoineFromAfar", "sonid13");
	voiceList.insert("AntoineHappy", "sonid13");
	voiceList.insert("AntoineSad", "sonid13");
	voiceList.insert("AntoineUpClose", "sonid13");
	voiceList.insert("Bruno", "sonid13");
	voiceList.insert("Claire", "sonid13");
	voiceList.insert("Julie", "sonid13");
	voiceList.insert("Manon", "sonid13");
	voiceList.insert("Margaux", "sonid13");
	voiceList.insert("MargauxHappy", "sonid13");
	voiceList.insert("MargauxSad", "sonid13");
	// German
	voiceList.insert("Andreas", "sonid14");
	voiceList.insert("Jonas", "sonid14");
	voiceList.insert("Julia", "sonid14");
	voiceList.insert("Klaus", "sonid14");
	voiceList.insert("Lea", "sonid14");
	voiceList.insert("Sarah", "sonid14");
	// Italian
	voiceList.insert("chiara", "sonid16");
	voiceList.insert("fabiana", "sonid16");
	voiceList.insert("vittorio", "sonid16");
	// Russian
	voiceList.insert("Alyona", "sonid24");
	// Spanish (Spain)
	voiceList.insert("Antonio", "sonid25");
	voiceList.insert("Ines", "sonid25");
	voiceList.insert("Maria", "sonid25");
	// Spanish (US)
	voiceList.insert("Rodrigo", "sonid26");
	voiceList.insert("Rosa", "sonid26");
}

TTSAcapela::~TTSAcapela()
{
}

QByteArray TTSAcapela::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{
	QEventLoop loop;

	if(!voiceList.contains(voice))
		voice = "Claire";

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
	QUrl qurl = QUrl("http://www.acapela-group.com/demo-tts/DemoHTML5Form_V2_fr.php?langdemo=Vocalis%C3%A9+par+%3Ca+href%3D%22http%3A%2F%2Fwww.acapela-vaas.com%22%3EAcapela+Voice+as+a+Service%3C%2Fa%3E.+Pour+d%C3%A9monstration+et+%C3%A9valuation+uniquement%2C+pour+toute+utilisation+commerciale+des+fichiers+sons+g%C3%A9n%C3%A9r%C3%A9s%2C+consultez+%3Ca+href%3D%22http%3A%2F%2Fwww.acapela-box.com%22%3Ewww.acapela-box.com%3C%2Fa%3E");
    QNetworkRequest requete(qurl);
    QNetworkReply *http = NULL;

	QString langId = voiceList.value(voice);
	QString langName = voice;

	QUrlQuery query;
	query.addQueryItem("MyLanguages", langId);
	query.addQueryItem("0", "Leila");
	query.addQueryItem("1", "Laia");
	query.addQueryItem("2", "Eliska");
	query.addQueryItem("3", "Mette");
	query.addQueryItem("4", "Jeroen");
	query.addQueryItem("5", "Daan");
	query.addQueryItem("6", "Liam");
	query.addQueryItem("7", "Deepa");
	query.addQueryItem("8", "Graham");
	query.addQueryItem("9", "Heather");
	query.addQueryItem("10", "Sanna");
	query.addQueryItem("11", "Justine");
	query.addQueryItem("12", "Louise");
	query.addQueryItem("MySelectedVoice", langName);
	query.addQueryItem("14", "Andreas");
	query.addQueryItem("15", "Dimitris");
	query.addQueryItem("16", "chiara");
	query.addQueryItem("17", "Sakura");
	query.addQueryItem("18", "Minji");
	query.addQueryItem("19", "Lulu");
	query.addQueryItem("20", "Bente");
	query.addQueryItem("21", "Ania");
	query.addQueryItem("22", "Marcia");
	query.addQueryItem("23", "Celia");
	query.addQueryItem("24", "Alyona");
	query.addQueryItem("25", "Antonio");
	query.addQueryItem("26", "Rodrigo");
	query.addQueryItem("27", "Elin");
	query.addQueryItem("28", "Samuel");
	query.addQueryItem("29", "Kal");
	query.addQueryItem("30", "Mia");
	query.addQueryItem("31", "Ipek");
	query.addQueryItem("MyTextForTTS", QUrl::toPercentEncoding(text));
	query.addQueryItem("t", "1");
	query.addQueryItem("SendToVaaS", "");

	QUrl post_data;
	post_data.setQuery(query);

	requete.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	requete.setRawHeader("Accept-Encoding", "gzip, deflate");
	requete.setRawHeader("Accept-Language", "fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
	requete.setRawHeader("Connection", "keep-alive");
	requete.setRawHeader("Host", "www.acapela-group.com");
	requete.setRawHeader("Referer", "http://www.acapela-group.com/demo-tts/DemoHTML5Form_V2.php?langdemo=Powered+by+%3Ca+href%3D%22http%3A%2F%2Fwww.acapela-vaas.com%22%3EAcapela+Voice+as+a+Service%3C%2Fa%3E.+For+demo+and+evaluation+purpose+only%2C+for+commercial+use+of+generated+sound+files+please+go+to+%3Ca+href%3D%22http%3A%2F%2Fwww.acapela-box.com%22%3Ewww.acapela-box.com%3C%2Fa%3E");
	requete.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:28.0) Gecko/20100101 Firefox/28.0");
	requete.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

	http = connection->post(requete, post_data.toEncoded());

	QObject::connect(http, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();
	
	QByteArray reponse = http->readAll();

	QRegExp rx("myPhpVar = '(http://[^']*mp3)';");
	rx.setMinimal(true);
	int pos = 0;
	if((pos = rx.indexIn(reponse, pos)) != -1 )
        {
                QString acapelaFile = rx.cap(1);
                QUrl urlfile(acapelaFile);
                QNetworkAccessManager * manager = new QNetworkAccessManager(this);
                QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
                QNetworkReply * reply = manager->get(QNetworkRequest(urlfile));
                loop.exec();
                QFile file(filePath);
                if (!file.open(QIODevice::WriteOnly))
                {
                    LogError("Cannot open sound file for writing");
                    return QByteArray();
                }
                file.write(reply->readAll());
                file.close();
                return ttsHTTPUrl.arg(voice, fileName).toLatin1();
        }
 	LogError("Acapela demo did not return a sound file");
	LogDebug(QString("Acapela answer : %1").arg(QString(reponse)));
	return QByteArray();

}

