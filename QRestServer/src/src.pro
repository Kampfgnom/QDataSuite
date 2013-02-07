QDATASUITE_PATH = ../../QDataSuite
include($$QDATASUITE_PATH/QDataSuite.pri)

QRESTSERVER_PATH = ..
include($$QRESTSERVER_PATH/QRestServer.pri)

QHAL_PATH = ../lib/QHal
include($$QHAL_PATH/QHal.pri)

### General config ###

TARGET          = $$QRESTSERVER_TARGET
VERSION         = $$QRESTSERVER_VERSION
TEMPLATE        = lib
QT              += network
QT              -= gui
CONFIG          += static libc++11
QMAKE_CXXFLAGS  += $$QDATASUITE_COMMON_QMAKE_CXXFLAGS
INCLUDEPATH     += $$QRESTSERVER_INCLUDEPATH


### QDataSuite ###

INCLUDEPATH     += $$QDATASUITE_INCLUDEPATH
LIBS            += $$QDATASUITE_LIBS


### QHttpServer ###

INCLUDEPATH     += $$QHTTPSERVER_INCLUDEPATH
LIBS            += $$QHTTPSERVER_LIBS

### QHal ###

INCLUDEPATH     += $$QHAL_INCLUDEPATH
LIBS            += $$QHAL_LIBS

### Files ###

HEADERS += \
    server.h \
    linkhelper.h \
    responder.h \
    serializer.h \
    parser.h \
    haljsonserializer.h

SOURCES += \
    server.cpp \
    linkhelper.cpp \
    responder.cpp \
    serializer.cpp \
    parser.cpp \
    haljsonserializer.cpp
