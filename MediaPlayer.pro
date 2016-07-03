#-------------------------------------------------
#
# Project created by QtCreator 2014-03-15T16:49:47
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets winextras

TARGET = MediaPlayer
TEMPLATE = app

SOURCES += main.cpp\
    frmaddmark.cpp \
    frmmain.cpp \
    gstmanager.cpp \
    markerdb.cpp \
    videoarea.cpp \
    seekbar.cpp \
    wsearchresultitem.cpp \
    filehash.cpp \
    playlist.cpp

HEADERS  += \
    mman.h \
    frmaddmark.h \
    frmmain.h \
    gstmanager.h \
    markerdb.h \
    videoarea.h \
    seekbar.h \
    wsearchresultitem.h \
    filehash.h \
    playlist.h

FORMS    += \
    frmaddmark.ui \
    frmmain.ui \
    wsearchresultitem.ui


RESOURCES += \
    resources.qrc
RC_FILE = resource.rc


win32: LIBS += -L"C:/Program Files (x86)/VideoLAN/VLC/sdk/lib"/ -llibvlccore
win32: LIBS += -L"C:/Program Files (x86)/VideoLAN/VLC/sdk/lib"/ -llibvlc

INCLUDEPATH += "C:/Program Files (x86)/VideoLAN/VLC/sdk/include"
DEPENDPATH += "C:/Program Files (x86)/VideoLAN/VLC/sdk/include"
