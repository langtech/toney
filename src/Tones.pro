#-------------------------------------------------
#
# Project created by QtCreator 2011-11-02T16:55:18
#
#-------------------------------------------------

QT       += core gui xml multimedia
CONFIG   += static

TARGET = Tones
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    cluster.cpp \
    clusterbox.cpp \
    pool.cpp \
    item.cpp \
    audiofile.cpp \
    annotation.cpp \
    annotationset.cpp \
    textgrid.cpp \
    playmenu.cpp \
    targetlist.cpp \
    listplayer.cpp \
    player.cpp \
    f0contour.cpp \
    f0display.cpp

HEADERS  += mainwindow.h \
    cluster.h \
    clusterbox.h \
    pool.h \
    item.h \
    audiofile.h \
    annotation.h \
    annotationset.h \
    tonesexception.h \
    textgrid.h \
    playmenu.h \
    targetlist.h \
    listplayer.h \
    config.h \
    player.h \
    f0contour.h \
    f0display.h

FORMS    += mainwindow.ui \
    cluster.ui \
    clusterbox.ui

unix {
    LIBS += -lsndfile -L../../get_f0 -lgetf0
    INCLUDEPATH += ../../get_f0
    SOURCES += soxplayer.cpp
    HEADERS += soxplayer.h
    DEFINES += SOXPLAYER
}

macx {
    # other stuff is inherited from the "unix" section
    LIBS += -L/opt/local/lib
    INCLUDEPATH += /opt/local/include
    CONFIG += app_bundle
}

win32 {
    LIBS += "C:/Program Files/Mega-Nerd/libsndfile/lib/libsndfile-1.lib"
    INCLUDEPATH += ../../get_f0 "C:/Program Files/Mega-Nerd/libsndfile/include"
    SOURCES += qtplayer.cpp
    HEADERS += qtplayer.h
    DEFINES += QTPLAYER
    SOURCES += \
        ../../get_f0/get_f0.c \
        ../../get_f0/dp_f0.c \
        ../../get_f0/get_cands.c \
        ../../get_f0/sigproc.c
    HEADERS += \
        ../../get_f0/f0.h \
        ../../get_f0/f0_structs.h
}

RESOURCES +=
