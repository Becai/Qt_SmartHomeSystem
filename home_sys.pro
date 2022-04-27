#-------------------------------------------------
#
# Project created by QtCreator 2022-04-21T10:08:48
#
#-------------------------------------------------

QT       += core gui network multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = home_sys
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    logger.cpp \
        main.cpp \
    manager.cpp \
    opertion.cpp \
    request.cpp \
    verify.cpp \
    voice_table.cpp \
        widget.cpp

HEADERS += \
    logger.h \
    manager.h \
    operation.h \
    request.h \
    verify.h \
    voice_table.h \
        widget.h

FORMS += \
        verify.ui \
        widget.ui

RESOURCES += \
    assets.qrc
