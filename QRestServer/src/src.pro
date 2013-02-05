QDATASUITE_PATH = ../../QDataSuite
include($$QDATASUITE_PATH/QDataSuite.pri)

QRESTSERVER_PATH = ..
include($$QRESTSERVER_PATH/QRestServer.pri)

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


### Files ###

HEADERS += \
    collection.h \
    server.h \
    global.h \
    linkhelper.h \
    collectionresponder.h

SOURCES += \
    server.cpp \
    global.cpp \
    linkhelper.cpp \
    collection.cpp \
    collectionresponder.cpp
