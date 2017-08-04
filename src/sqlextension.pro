VERSION = 1.0.0
QMAKE_TARGET_COMPANY = "RTPTechGroup"
QMAKE_TARGET_PRODUCT = "SqlExtension Library"
QMAKE_TARGET_DESCRIPTION = "SqlExtension Library"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) RTPTechGroup"

include(../libs.pri)

DEFINES += SQLEXTENSION_LIBRARY
greaterThan(QT_MAJOR_VERSION, 4) {
  QT += sql widgets
}

# Input
HEADERS   = *.h
SOURCES   = *.cpp
RESOURCES = *.qrc
FORMS     = *.ui
