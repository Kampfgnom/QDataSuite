TEMPLATE = subdirs

CONFIG += ordered
SUBDIRS = QDataSuite QRestServer QPersistence examples

QRestServer.subdir      = QRestServer
QRestServer.depends     = QDataSuite
QPersistence.subdir     = QPersistence
QPersistence.depends    = QDataSuite
examples.subdir     = examples
examples.depends    = QDataSuite QRestServer QPersistence
