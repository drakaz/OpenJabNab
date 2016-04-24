TEMPLATE = lib
QT_CONFIG -= no-pkg-config
CONFIG += qt release link_pkgconfig
PKGCONFIG += pocketsphinx sphinxbase
CONFIG -= debug
QT += network
QT -= gui
TARGET = common
INCLUDEPATH += . ../main
DEPENDPATH += . ../main
MOC_DIR = ./tmp/moc
OBJECTS_DIR = ./tmp/obj
DESTDIR = ../bin/
win32 {
	DEFINES += WIN32 OJN_MAKEDLL
	QMAKE_CXXFLAGS_WARN_ON += -WX
}
unix {
	QMAKE_CXXFLAGS += -Werror
}
# Input
HEADERS +=	httphandler.h \
			xmpphandler.h \
			httprequest.h \
			settings.h \
			log.h \
			pluginmanager.h \
			pluginapihandler.h \
			pluginauthinterface.h \
			plugininterface.h \
			plugininterface_inline.h \
			packet.h \
			ambientpacket.h \
			messagepacket.h \
			sleeppacket.h \
			choregraphy.h \
			bunnymanager.h \
			bunny.h \
			ztampmanager.h \
			ztamp.h \
			apimanager.h \
			cron.h \
			ttsmanager.h \
			ttsinterface.h \
			ttsinterface_inline.h \
			accountmanager.h \
			account.h \
			apihandler.h \
			netdump.h \
			iq.h \
			pocketSphinx.h \
			context.h

SOURCES +=	httphandler.cpp \
			xmpphandler.cpp \
			httprequest.cpp \
			settings.cpp \
			log.cpp \
			pluginmanager.cpp \
			packet.cpp \
			ambientpacket.cpp \
			messagepacket.cpp \
			sleeppacket.cpp \
			choregraphy.cpp \
			bunnymanager.cpp \
			bunny.cpp \
			ztampmanager.cpp \
			ztamp.cpp \
			apimanager.cpp \
			cron.cpp \
			ttsmanager.cpp \
			accountmanager.cpp \
			account.cpp \
			netdump.cpp \
			iq.cpp \
			pocketSphinx.cpp \
			context.cpp
