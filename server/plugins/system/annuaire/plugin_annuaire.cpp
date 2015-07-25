#include <QDebug>
#include <QUrl>
#include <QUrlQuery>
#include <QXmlStreamReader>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QObject>
#include "bunny.h"
#include "account.h"
#include "plugin_annuaire.h"

PluginAnnuaire::PluginAnnuaire():PluginInterface("annuaire", "Register the bunny on the central directory", SystemPlugin)
{
}

PluginAnnuaire::~PluginAnnuaire() {}

void PluginAnnuaire::OnBunnyConnect(Bunny * b)
{
	QString server = GetSettings("global/URL", "").toString();
	if(server != "") {
    	QNetworkAccessManager *connection = new QNetworkAccessManager();
   
		QString api = b->GetGlobalSetting("VApiEnable", false).toBool() ? "1" : "0";
		QString pub = b->GetGlobalSetting("VApiPublic", false).toBool() ? "1" : "0";

		QUrlQuery url = QUrlQuery(server + "/nabconnection.php");
		url.addQueryItem("m", b->GetID());
		url.addQueryItem("n", b->GetBunnyName());
		url.addQueryItem("s", GlobalSettings::GetString("OpenJabNabServers/PingServer"));
		url.addQueryItem("ip", b->GetGlobalSetting("LastIP", QString("")).toString());
		url.addQueryItem("api", api);
		url.addQueryItem("public", pub);

		QUrl qurl = QUrl(url.query(QUrl::FullyEncoded).toUtf8());

	    QNetworkRequest requete(qurl);
	    QNetworkReply *http = NULL;
	    Q_UNUSED(http);
	    http = connection->get(requete);    
	}
}

QList<BunnyInfos> PluginAnnuaire::SearchBunnyByName(QString name)
{
	QEventLoop loop;
    QNetworkAccessManager *connection = new QNetworkAccessManager();

	QString server = GetSettings("global/URL").toString();
	QUrlQuery url = QUrlQuery(server + "/whois.php");
	url.addQueryItem("n", QUrl::toPercentEncoding(name));

	QUrl qurl = QUrl(url.query(QUrl::FullyEncoded).toUtf8());

	QNetworkRequest requete(qurl);
	QNetworkReply *http = NULL;

	http = connection->get(requete);

	QObject::connect(http, SIGNAL(done(bool)), &loop, SLOT(quit()));
	loop.exec();

	QXmlStreamReader xml;
	xml.clear();
	xml.addData(http->readAll());

	QString currentTag;
	QList<BunnyInfos> whois = QList<BunnyInfos>();
	BunnyInfos currentBunny;
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement() || xml.isEndElement())
		{
			currentTag = xml.name().toString();
		}
		else if (xml.isCharacters() && !xml.isWhitespace())
		{
			if(currentTag == "name")
			{
				currentBunny.name = xml.text().toString();
			}
			else if(currentTag == "macaddress")
			{
				currentBunny.ID = xml.text().toString().toLatin1();
			}
			else if(currentTag == "server")
			{
				currentBunny.server = xml.text().toString();
			}
		}
		if(xml.isEndElement() && currentTag == "bunny")
		{
			whois.append(currentBunny);
		}
	}

	return whois;
}

QList<BunnyInfos> PluginAnnuaire::SearchBunnyByMac(QByteArray ID)
{
	QEventLoop loop;
    QNetworkAccessManager *connection = new QNetworkAccessManager();

	QString server = GetSettings("global/URL").toString();
	QUrlQuery url = QUrlQuery(server + "/whois.php");
	url.addQueryItem("nm", QUrl::toPercentEncoding(ID));

	QUrl qurl = QUrl(url.query(QUrl::FullyEncoded).toUtf8());

	QNetworkRequest requete(qurl);
	QNetworkReply *http = NULL;

	http = connection->get(requete);

	QObject::connect(http, SIGNAL(done(bool)), &loop, SLOT(quit()));
	loop.exec();

	QXmlStreamReader xml;
	xml.clear();
	xml.addData(http->readAll());

	QString currentTag;
	QList<BunnyInfos> whois = QList<BunnyInfos>();
	BunnyInfos currentBunny;
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement() || xml.isEndElement())
		{
			currentTag = xml.name().toString();
		}
		else if (xml.isCharacters() && !xml.isWhitespace())
		{
			if(currentTag == "name")
			{
				currentBunny.name = xml.text().toString();
			}
			else if(currentTag == "macaddress")
			{
				currentBunny.ID = xml.text().toString().toLatin1();
			}
			else if(currentTag == "server")
			{
				currentBunny.server = xml.text().toString();
			}
		}
		if(xml.isEndElement() && currentTag == "bunny")
		{
			whois.append(currentBunny);
		}
	}

	return whois;
}
/*******/
/* API */
/*******/
void PluginAnnuaire::InitApiCalls()
{
	DECLARE_PLUGIN_API_CALL("setURL(url)", PluginAnnuaire, Api_setURL);
	DECLARE_PLUGIN_API_CALL("getURL()", PluginAnnuaire, Api_getURL);
	DECLARE_PLUGIN_API_CALL("searchbunnybymac(mac)", PluginAnnuaire, Api_SearchBunnyByMac);
	DECLARE_PLUGIN_API_CALL("searchbunnybyname(name)", PluginAnnuaire, Api_SearchBunnyByName);
	DECLARE_PLUGIN_API_CALL("verifymactoken(mac,reqtoken)", PluginAnnuaire, Api_VerifyMacToken);
}

PLUGIN_API_CALL(PluginAnnuaire::Api_setURL)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QString url = hRequest.GetArg("url");
	SetSettings("global/URL", url);

	return new ApiManager::ApiOk(QString("URL set to '%1'.").arg(url));
}

PLUGIN_API_CALL(PluginAnnuaire::Api_getURL)
{
	Q_UNUSED(hRequest);

	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	return new ApiManager::ApiString(QString("%1").arg(GetSettings("global/URL", "").toString()));
}

PLUGIN_API_CALL(PluginAnnuaire::Api_SearchBunnyByMac)
{
	Q_UNUSED(account);

	QList<BunnyInfos> whois = SearchBunnyByMac(hRequest.GetArg("mac").toLatin1());
	QString xml = "";
	foreach(BunnyInfos b, whois)
	{
		xml += "<bunny>";
		xml += "<name>" + b.name + "</name>";
		xml += "<ID>" + b.ID + "</ID>";
		xml += "<server>" + b.server + "</server>";
		xml += "</bunny>\n";
	}
	return new ApiManager::ApiXml(xml);
}

PLUGIN_API_CALL(PluginAnnuaire::Api_SearchBunnyByName)
{
	Q_UNUSED(account);

	QList<BunnyInfos> whois = SearchBunnyByName(hRequest.GetArg("name"));
	QString xml = "";
	foreach(BunnyInfos b, whois)
	{
		xml += "<bunny>";
		xml += "<name>" + b.name + "</name>";
		xml += "<ID>" + b.ID + "</ID>";
		xml += "<server>" + b.server + "</server>";
		xml += "</bunny>\n";
	}
	return new ApiManager::ApiXml(xml);
}

PLUGIN_API_CALL(PluginAnnuaire::Api_VerifyMacToken)
{
	Q_UNUSED(account);
	Bunny * b = BunnyManager::GetBunny(hRequest.GetArg("mac").toLatin1());
	QString xml = "";
		
	if (hRequest.GetArg("reqtoken").toLatin1()==b->GetGlobalSetting("VApiToken","FAILED").toString() ) 
		xml += "<verify>true</verify>\n";
	else 
		xml += "<verify>false</verify>\n";
	
	return new ApiManager::ApiXml(xml);

}
