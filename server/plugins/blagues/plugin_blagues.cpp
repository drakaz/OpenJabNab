#include <QDateTime>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QHttp>
#include <QMapIterator>
#include <QRegExp>
#include <QUrl>
#include <QTextDocument>
#include <QTextCodec>
#include <memory>
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "log.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_blagues.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_blague, PluginBlagues)

// +/- 20% - 30min => rand(24,36)
#define RANDOMIZEDRATIO 20

PluginBlagues::PluginBlagues():PluginInterface("blagues", "Blagues",BunnyZtampPlugin) {}

PluginBlagues::~PluginBlagues() {}

bool PluginBlagues::Init()
{
	QByteArray currentBlague = TTSManager::CreateNewSound("Allez, encore une petite blague", "Claire");
	if(currentBlague.isNull())
		return false;

	blaguesMessage = "MU " + currentBlague + "\nPL 3\nMW\n";
	return true;
}

void PluginBlagues::createCron(Bunny * b)
{
        // Check Frequency
        unsigned int frequency = b->GetPluginSetting(GetName(), "frequency", (uint)0).toUInt();
        if(!frequency)
        {
                // Stable. Warning Removed.
                LogDebug(QString("Bunny '%1' has invalid frequency '%2'").arg(b->GetID(), QString::number(frequency)));
                return;
        }

	if (frequency > 1)
	{
		// Register cron
		Cron::RegisterOneShot(this, GetRandomizedFrequency(frequency), b, QVariant(), NULL);
	}
}

int PluginBlagues::GetRandomizedFrequency(unsigned int freq)
{
        // 250 => ~30min, 125 => ~1h, 50 => ~2h30
        unsigned int meanTimeInSec = (250/freq) * 30;

        int deviation = 0;

        if(RANDOMIZEDRATIO > 0 && RANDOMIZEDRATIO < 100)
        {
                unsigned int maxDeviation = (meanTimeInSec * 2 * RANDOMIZEDRATIO) / 100;
                if(maxDeviation > 0)
                {
                        deviation = qrand() % (maxDeviation);
                }
                deviation -= (maxDeviation/2);
        }

        return meanTimeInSec + deviation;
}

void PluginBlagues::OnCron(Bunny * b, QVariant)
{
	getBlagues(b);
        // Restart Timer
        createCron(b);
}

bool PluginBlagues::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		getBlagues(b);
		return true;
	}
	return false;
}

void PluginBlagues::getBlagues(Bunny * b)
{
	QDir * sdir = GetResourcesFolder();
	QDir * hdir = GetLocalHTTPFolder();
	QByteArray message = blaguesMessage;
	QStringList list;
	qsrand(QDateTime::currentDateTime ().toTime_t ());
	if(sdir)
	{
		sdir->cd("sources");
		sdir->setNameFilters(QStringList("*.txt"));
		QStringList slist = sdir->entryList(QDir::Files|QDir::NoDotAndDotDot);
		foreach (const QString &tFilename, slist)
		{
			list.append(sdir->absoluteFilePath(tFilename));
		}
	}
	if (hdir)
	{
		hdir->cd("sources");
		hdir->setNameFilters(QStringList("*.txt"));
		QStringList hlist = hdir->entryList(QDir::Files|QDir::NoDotAndDotDot);
		foreach (const QString &tFilename, hlist)
		{
			list.append(hdir->absoluteFilePath(tFilename));
		}
	}
	if (list.size() > 0)
	{
		QString filepath = list.at(qrand()%list.count());
		// LogDebug("Selected source :" + filepath);
		// Load file
		QFile blagueFile (filepath);
		// Get pre message if exist
		QFile blagueFilePre (filepath + ".pre");
		if (blagueFilePre.exists())
		{
			blagueFilePre.open(QFile::ReadOnly);
			QTextStream preflux(&blagueFilePre);	
			preflux.setCodec(QTextCodec::codecForName("UTF-8"));
			QString premessage = preflux.readLine();
			// LogDebug("Premessage found : " + premessage);
			QByteArray currentBlague = TTSManager::CreateNewSound(premessage, "Claire");
			message = "MU " + currentBlague + "\nPL 3\nMW\n";
			blagueFilePre.close();
		}
				
		int line_count=0;
		if(blagueFile.open(QFile::ReadOnly))
		{
			QTextStream flux(&blagueFile);
			flux.setCodec(QTextCodec::codecForName("UTF-8"));
			// First count lines in file
			while( !flux.atEnd())
			{
				flux.readLine();
				line_count++;
			}
			// Random line
			qsrand(QDateTime::currentDateTime ().toTime_t ());
			int random = qrand() % line_count;
			// Seek to begin
			flux.seek(0);
			// Read again and stop on the chosen line
			int loop = 0;
			while( !flux.atEnd())
			{
				QString blagounette = flux.readLine();
				if (loop == random)
				{
					// LogDebug("Selected blagues : " + blagounette); 
					QRegExp sep("[. ][:][- ]+");
					QStringList splitted_blagounette = blagounette.split(sep);
					foreach (const QString &word, splitted_blagounette) {
						if (!word.trimmed().isEmpty())
						{
							QByteArray file = TTSManager::CreateNewSound(word.trimmed(), "Claire");
							message += "MU " + file + "\nPL 3\nMW\n";
						}
					}
					break;
				}
				loop++;
			}
			blagueFile.close();
			if(b->IsConnected())
			{
				QByteArray file = GetBroadcastHTTPPath("mp3/son_blague.mp3");
				message += "MU "+file+"\nPL 3\nMW\n";
				b->SendPacket(MessagePacket(message));
			}
		}
	}
}

void PluginBlagues::OnBunnyConnect(Bunny * b)
{
	createCron(b);
}

void PluginBlagues::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

/*******
 * API *
 *******/

void PluginBlagues::InitApiCalls()
{
        DECLARE_PLUGIN_BUNNY_API_CALL("setFrequency(value)", PluginBlagues, Api_SetFrequency);
        DECLARE_PLUGIN_BUNNY_API_CALL("getFrequency()", PluginBlagues, Api_GetFrequency);
}

PLUGIN_BUNNY_API_CALL(PluginBlagues::Api_SetFrequency)
{
        Q_UNUSED(account);

        bunny->SetPluginSetting(GetName(), "frequency", QVariant(hRequest.GetArg("value").toInt()));
        OnBunnyDisconnect(bunny);
        OnBunnyConnect(bunny);
        return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}

PLUGIN_BUNNY_API_CALL(PluginBlagues::Api_GetFrequency)
{
        Q_UNUSED(account);
        Q_UNUSED(hRequest);

        return new ApiManager::ApiOk(QString::number(bunny->GetPluginSetting(GetName(), "frequency", (uint)0).toInt()));
}
