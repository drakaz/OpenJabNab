######################################################################
# Automatically generated by qmake (2.01a) sam. janv. 19 19:10:01 2008
######################################################################

TEMPLATE = lib
CONFIG += plugin qt
CONFIG += debug_and_release

QT += network xml
QT -= gui
INCLUDEPATH += . ../../server ../../lib
TARGET = tts_google
DESTDIR = ../../bin/tts
DEPENDPATH += . ../../server ../../lib
LIBS += -L../../bin/ -lcommon
MOC_DIR = ./tmp/moc
OBJECTS_DIR = ./tmp/obj
win32 {
	QMAKE_CXXFLAGS_WARN_ON += -WX
}
unix {
	QMAKE_LFLAGS += -Wl,-rpath,\'\$$ORIGIN\'
	QMAKE_CXXFLAGS += -Werror
}

# Input
HEADERS += tts_google.h
SOURCES += tts_google.cpp
