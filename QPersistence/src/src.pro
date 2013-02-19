QDATASUITE_PATH = ../../QDataSuite
include($$QDATASUITE_PATH/QDataSuite.pri)

QPERSISTENCE_PATH = ..
include($$QPERSISTENCE_PATH/QPersistence.pri)

### General config ###

TARGET          = $$QPERSISTENCE_TARGET
VERSION         = $$QPERSISTENCE_VERSION
TEMPLATE        = lib
QT              += sql
QT              -= gui
CONFIG          += static c++11
QMAKE_CXXFLAGS  += $$QDATASUITE_COMMON_QMAKE_CXXFLAGS
INCLUDEPATH     += $$QPERSISTENCE_INCLUDEPATH


### QDataSuite ###

INCLUDEPATH     += $$QDATASUITE_INCLUDEPATH
LIBS            += $$QDATASUITE_LIBS


### Files ###

HEADERS += \
    databaseschema.h \
    sqldataaccessobjecthelper.h \
    persistentdataaccessobject.h \
    sqlquery.h \
    sqlcondition.h

SOURCES += \
    databaseschema.cpp \
    sqldataaccessobjecthelper.cpp \
    persistentdataaccessobject.cpp \
    sqlquery.cpp \
    sqlcondition.cpp
