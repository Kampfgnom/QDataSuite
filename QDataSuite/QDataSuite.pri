isEmpty(QDATASUITE_PATH) {
    error(You have to set QDATASUITE_PATH to the path of QDataSuite)
}

QDATASUITE_TARGET           = qdatasuite
QDATASUITE_VERSION          = 0.0.0
QDATASUITE_INCLUDEPATH      = $$PWD/include
QDATASUITE_LIBS             = -L$$QDATASUITE_PATH/src -l$$QDATASUITE_TARGET

QDATASUITE_COMMON_QMAKE_CXXFLAGS = -Wall -ansi -pedantic \ # Gives us more compiler warnings
                                    -Wno-long-long  # When using -pedantic, the qglobal.h does not
                                                    # compile because illegal usage of long long
