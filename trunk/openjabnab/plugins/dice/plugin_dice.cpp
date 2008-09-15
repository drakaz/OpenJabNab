#include <QDateTime>
#include <time.h>
#include <stdlib.h>
#include "plugin_dice.h"
#include "bunny.h"
#include "httprequest.h"
#include "log.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_dice, PluginDice)

PluginDice::PluginDice():PluginInterface("dice", "Dice roll")
{
	// Initialize the randomizer
	srand(time(NULL));
}

bool PluginDice::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		// Language
		QByteArray Language = b->GetPluginSetting("dice", "PluginConfiguration/Language", "fr").toByteArray();
		// Get a random value and create ID
		quint8 value = rand() % 6 + 1;
		Log::Debug(" -- Language : " + Language + " -- Roll dice : " + QString::number(value));
		// Send packet to bunny with mp3 to be played
		b->SendPacket(MessagePacket("MU broadcast/ojn_local/plugins/dice/" + Language + "/get.mp3\nMW\nMU broadcast/ojn_local/plugins/dice/" + Language + "/" + QByteArray::number(value) + ".mp3\nMW\n"));
	}
	else
	{
		// Shutdown of the plugin on Dbl click
		// To be done when activation / deactivation is OK
	}
	return true;
}

PluginDice::~PluginDice()
{
	QIODevice * d = diceStream.device();
	if (d)
		d->close();
}