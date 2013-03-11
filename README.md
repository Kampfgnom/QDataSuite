QSerienJunkies 0.0
========

QSerienJunkies is a Qt wrapper for serienjunkies.org.


Project setup
=============

QSerienJunkies comes with some helpers to include it in you project.

You can simply add the whole project to your qmake based project and add the following lines to your application's project file:

````
QSERIENJUNKIES_PATH = relative/or/absolute/path/to/QSerienJunkies
include($$QSERIENJUNKIES_PATH/QSerienJunkies.pri)
LIBS            += $$QSERIENJUNKIES_LIBS
INCLUDEPATH     += $$QSERIENJUNKIES_INCLUDEPATH
`````

You may of course setup everything as you like :wink:


Usage
=====

Please have a look at the example for further details.

Requirements and building
=========================

QSerienJunkies has been developed and tested only on Qt 5.0 on a Mac. I don't think that it will ever be compatible to older versions of Qt.

The project can be build in Qt Creator or by issuing the following commands.

````
qmake
make
````

Contributing
============
Since this library is currently not in use, it is not tested well.

If you find errors, simply file an issue in GitHub, or even better:

If you want to contribute to QSerienJunkies, you can do the usual Fork-Patch-FilePullRequest dance. I might even tranfser the project in someone else's hands!

License
=======

QSerienJunkies is licensed under the LGPLv3.0. See LICENSE for details.
