#include <QDateTime>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QMapIterator>
#include <QRegExp>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <memory>
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "log.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_ephemeride.h"
#include "settings.h"
#include "ttsmanager.h"

Q_PLUGIN_METADATA(IID "org.openjabnab.plugin.ephemeride" FILE "ephemeride.json");

PluginEphemeride::PluginEphemeride():PluginInterface("ephemeride", "Ephemeride",BunnyPlugin) {}

PluginEphemeride::~PluginEphemeride()
{
	Cron::UnregisterAll(this);
}

bool PluginEphemeride::Init()
{
	QByteArray aujourdhui = TTSManager::CreateNewSound("Aujourd'hui nous fetons les", "Claire");
	if(aujourdhui.isNull())
		return false;

	aujourdhuiMessage = "MU " + aujourdhui + "\nPL 3\nMW\n";
	return true;
}

void PluginEphemeride::OnCron(Bunny * b, QVariant)
{
	getEphemeridePage(b);
}

bool PluginEphemeride::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		getEphemeridePage(b);
		return true;
	}
	return false;
}

void PluginEphemeride::getEphemeridePage(Bunny * b)
{
	QEventLoop waitLoop;
    QNetworkAccessManager *connection = new QNetworkAccessManager();
    QUrl url = QUrl("www.net-pratique.fr/services/saintdujour.php");
    QNetworkRequest requete(url);
    QNetworkReply *http = NULL;

    connect(http, SIGNAL(done(bool)), this, SLOT(analyseXml()));

    http->setProperty("BunnyID", b->GetID());

    http = connection->get(requete);
}

void PluginEphemeride::analyseXml()
{
	std::auto_ptr<QNetworkReply> http(qobject_cast<QNetworkReply *>(sender()));
	Bunny * bunny = BunnyManager::GetBunny(this, http->property("BunnyID").toByteArray());
	if(!bunny)
		return;

	PluginEphemeride_Worker * p = new PluginEphemeride_Worker(this, bunny, http->readAll());
	connect(p, SIGNAL(done(bool,Bunny*,QByteArray)), this, SLOT(analyseDone(bool,Bunny*,QByteArray)));
	connect(p, SIGNAL(finished()), p, SLOT(deleteLater()));
	p->start();
}

void PluginEphemeride::analyseDone(bool ret, Bunny * b, QByteArray message)
{
	if(ret && b->IsConnected())
		 b->SendPacket(MessagePacket(message));
}

void PluginEphemeride::OnBunnyConnect(Bunny * b)
{
	QStringList webcasts = b->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
	foreach(QString webcast, webcasts)
	{
		Cron::RegisterDaily(this, QTime::fromString(webcast, "hh:mm"), b);
	}
}

void PluginEphemeride::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

void PluginEphemeride::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(time)", PluginEphemeride, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(time)", PluginEphemeride, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("getwebcastslist()", PluginEphemeride, Api_GetWebcastsList);
}

PLUGIN_BUNNY_API_CALL(PluginEphemeride::Api_GetWebcastsList)
{
	        Q_UNUSED(account);
		        Q_UNUSED(hRequest);
			        return new ApiManager::ApiList(bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList());
}

PLUGIN_BUNNY_API_CALL(PluginEphemeride::Api_AddWebcast)
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

PLUGIN_BUNNY_API_CALL(PluginEphemeride::Api_RemoveWebcast)
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

/* WORKER THREAD */
PluginEphemeride_Worker::PluginEphemeride_Worker(PluginEphemeride * p, Bunny * bu, QByteArray b):plugin(p),bunny(bu),buffer(b.replace("&amp;", "and"))
{
}

void PluginEphemeride_Worker::run()
{
	QByteArray message = plugin->aujourdhuiMessage;
	QRegExp rx(">(.*)</span>");
	if(rx.indexIn(buffer) != -1)
	{
		QByteArray prenomFile = TTSManager::CreateNewSound(rx.cap(1).trimmed(), "Claire");
		message += "MU " + prenomFile + "\nPL 3\nMW\n";
		emit done(true, bunny, message);
	}
	else
		emit done(false, bunny, QByteArray());
}
