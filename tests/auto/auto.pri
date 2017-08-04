include(../tests.pri)
INCLUDEPATH += $$PWD
QT += testlib
CONFIG += qt warn_on console depend_includepath testcase
CONFIG -= app_bundle
DEFINES -= QT_NO_CAST_FROM_ASCII
