QDATASUITE_PATH = ..
include($$QDATASUITE_PATH/QDataSuite.pri)

### General config ###

TARGET          = $$QDATASUITE_TARGET
VERSION         = $$QDATASUITE_VERSION
TEMPLATE        = lib
QT              += sql
QT              -= gui
CONFIG          += static libc++11
QMAKE_CXXFLAGS  += $$QDATASUITE_COMMON_QMAKE_CXXFLAGS
INCLUDEPATH     += $$QDATASUITE_INCLUDEPATH

### Files ###

HEADERS += \
    metaproperty.h \
    error.h \
    metaobject.h \
    serializer.h \
    parser.h \
    jsonserializer.h \
    jsonparser.h \
    abstractdataaccessobject.h \
    simpledataaccessobject.h
SOURCES += \
    metaproperty.cpp \
    error.cpp \
    metaobject.cpp \
    serializer.cpp \
    parser.cpp \
    jsonserializer.cpp \
    jsonparser.cpp \
    abstractdataaccessobject.cpp \
    simpledataaccessobject.cpp
