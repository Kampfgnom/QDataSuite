isEmpty(QRESTSERVER_PATH) {
    error(You have to set QRESTSERVER_PATH to the path of QDataSuite relative to your project)
}

QRESTSERVER_TARGET          = qrestserver
QRESTSERVER_VERSION         = 0.0.0
QRESTSERVER_INCLUDEPATH     = $$PWD/include
QRESTSERVER_LIBS            = -L$$QRESTSERVER_PATH/src -l$$QRESTSERVER_TARGET

QHTTPSERVER_INCLUDEPATH     = $$PWD/lib/qhttpserver/src/src
QHTTPSERVER_LIBS            = -L$$PWD/lib/qhttpserver/lib -lqhttpserver
