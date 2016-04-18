#include <QDateTime>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QHttp>
#include <QMapIterator>
#include <QRegExp>
#include <QUrl>
#include <QTextDocument>
#include <memory>
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "log.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_tv.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_tv, PluginTV)

PluginTV::PluginTV():PluginInterface("tv", "Programme TV",BunnyZtampPlugin) {}

PluginTV::~PluginTV()
{
	Cron::UnregisterAll(this);
}

bool PluginTV::Init()
{
	QByteArray ceSoir = TTSManager::CreateNewSound("Programme tele de ce soir", "Claire");
	if(ceSoir.isNull())
		return false;

	ceSoirMessage = "MU " + ceSoir + "\nPL 3\nMW\n";
	return true;
}

void PluginTV::OnCron(Bunny * b, QVariant)
{
	getTVPage(b);
}

bool PluginTV::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		getTVPage(b);
		return true;
	}
	return false;
}

void PluginTV::getTVPage(Bunny * b)
{
	QHttp* http = new QHttp(this);
	http->setProperty("BunnyID", b->GetID());
	connect(http, SIGNAL(done(bool)), this, SLOT(analyseXml()));
	http->setHost("static.programme-tv.net");
	http->get("/rss/ns_tnt.xml");
}

void PluginTV::analyseXml()
{
	std::auto_ptr<QHttp> http(qobject_cast<QHttp *>(sender()));
	Bunny * bunny = BunnyManager::GetBunny(this, http->property("BunnyID").toByteArray());
	if(!bunny)
		return;

	PluginTV_Worker * p = new PluginTV_Worker(this, bunny, http->readAll());
	connect(p, SIGNAL(done(bool,Bunny*,QByteArray)), this, SLOT(analyseDone(bool,Bunny*,QByteArray)));
	connect(p, SIGNAL(finished()), p, SLOT(deleteLater()));
	p->start();
}

void PluginTV::analyseDone(bool ret, Bunny * b, QByteArray message)
{
	if(ret && b->IsConnected())
		 b->SendPacket(MessagePacket(message));
}

void PluginTV::OnBunnyConnect(Bunny * b)
{
	QStringList webcasts = b->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
	foreach(QString webcast, webcasts)
	{
		Cron::RegisterDaily(this, QTime::fromString(webcast, "hh:mm"), b);
	}
	QStringList channels = b->GetPluginSetting(GetName(), "Channels", QStringList()).toStringList();
	if(channels.count() == 0)
	{
		channels << "TF1" << "France 2" << "France 3" << "Arte" << "M6"; 
		b->SetPluginSetting(GetName(), "Channels", channels);
	}
}

void PluginTV::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

/*******
 * API *
 *******/

void PluginTV::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(time)", PluginTV, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(time)", PluginTV, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("listwebcast()", PluginTV, Api_ListWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("setchannel(list)", PluginTV, Api_SetChannel);
	DECLARE_PLUGIN_BUNNY_API_CALL("listchannel()", PluginTV, Api_ListChannel);
}

PLUGIN_BUNNY_API_CALL(PluginTV::Api_AddWebcast)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QString hTime = hRequest.GetArg("time");
	if(!bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList().contains(hTime))
	{
		Cron::RegisterDaily(this, QTime::fromString(hTime, "hh:mm"), bunny);
		QStringList bunnyWebcastList = bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList() << hTime;
		bunnyWebcastList.sort();
		bunny->SetPluginSetting(GetName(), "Webcast/List", bunnyWebcastList);
		return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
	}
	return new ApiManager::ApiError(QString("Webcast at '%1' already exists for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginTV::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(QString(bunny->GetID())));

	QStringList bunnyWebcastList = bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
	QString time = hRequest.GetArg("time");
	if(bunnyWebcastList.contains(time))
	{
		bunnyWebcastList.removeAll(time);
		bunnyWebcastList.sort();
		bunny->SetPluginSetting(GetName(), "Webcast/List", bunnyWebcastList);

		// Recreate crons
		OnBunnyDisconnect(bunny);
		OnBunnyConnect(bunny);

		return new ApiManager::ApiOk(QString("Remove webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
	}
	return new ApiManager::ApiError(QString("No webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginTV::Api_ListWebcast)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiList(bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList());
}

PLUGIN_BUNNY_API_CALL(PluginTV::Api_ListChannel)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiList(bunny->GetPluginSetting(GetName(), "Channels", QStringList()).toStringList());
}

PLUGIN_BUNNY_API_CALL(PluginTV::Api_SetChannel)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QStringList hList = hRequest.GetArg("list").split(",");
	bunny->SetPluginSetting(GetName(), "Channels", hList);
	return new ApiManager::ApiOk(QString("Channels setup is OK"));
}

/* WORKER THREAD */
PluginTV_Worker::PluginTV_Worker(PluginTV * p, Bunny * bu, QByteArray b):plugin(p),bunny(bu),buffer(b.replace("&amp;", "and"))
{
}

void PluginTV_Worker::run()
{
	QXmlStreamReader xml;
	xml.clear();
	xml.addData(buffer);
	xml.setNamespaceProcessing(true);

	QString currentTag;
	QString channel;
	QString title;
	QByteArray message = plugin->ceSoirMessage;
	QStringList liste = bunny->GetPluginSetting(plugin->GetName(), "Channels", QStringList()).toStringList();
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement())
		{
			currentTag = xml.name().toString();
		}
		else if (xml.isCharacters() && !xml.isWhitespace())
		{
			if (currentTag == "title")
			{
				title = xml.text().toString();
				// LogDebug("Found title : " + title);
			}
			if (currentTag == "channel")
			{
				QString encoded = xml.text().toString();
				QTextDocument html;
				html.setHtml(encoded);
				QString newchannel = html.toPlainText();
				// Remove unwanted char from some channel
				QString spokechannel = newchannel;
				spokechannel.remove(QRegExp("[-`~!@#$%^&*()_—+=|:;<>«»,.?/{}]"));
				// LogDebug("Found channel : " + newchannel );
				if (liste.contains("Extra : include late night program for selection") and newchannel != channel)
				{
					channel = newchannel;
					if (liste.contains(channel))
					{
						QByteArray file = TTSManager::CreateNewSound(spokechannel.trimmed() + ", prime time, " + title.trimmed(), "Claire");
						message += "MU " + file + "\nPL 3\nMW\n";
					}
				}
				else if (liste.contains("Extra : include late night program for selection") and newchannel == channel)
				{
					if (liste.contains(channel))
					{
						QByteArray file = TTSManager::CreateNewSound("puis, " + title.trimmed(), "Claire");
						message += "MU " + file + "\nPL 3\nMW\n";
					}
				}
				else if (newchannel != channel)
				{
					channel = newchannel;
					if (liste.contains(channel))
					{
						QByteArray file = TTSManager::CreateNewSound(spokechannel.trimmed() + ", " + title.trimmed(), "Claire");
						message += "MU " + file + "\nPL 3\nMW\n";
					}
				}
			}
		}
	}

	if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
		emit done(false, bunny, QByteArray());
	else
		emit done(true, bunny, message);
}
