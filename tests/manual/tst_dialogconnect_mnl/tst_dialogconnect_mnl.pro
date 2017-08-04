include(../../manual.pri)

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tst_dialogconnect_mnl
TEMPLATE = app

LIBS += -lsqlextension

HEADERS   = *.h
SOURCES   = *.cpp
