#-------------------------------------------------
#
# Project created by QtCreator 2013-11-27T09:57:14
#
#-------------------------------------------------

TEMPLATE = lib
TARGET = $$qtLibraryTarget(Seis)

#DEFINES += SEIS_LIBRARY

CONFIG  += staticlib

include(../../common.pri)
include(../qwt/qwt.pri)

SOURCES += \
    seisprofileview.cpp \
    seisprofileitem.cpp \
    hintmousescalewidget.cpp \
    linecdpscaledraw.cpp \
    seisprofiledata.cpp

    
HEADERS += \
    seisprofileview.h \
    seisprofileitem.h \
    hintmousescalewidget.h \
    linecdpscaledraw.h \
    seisprofiledata.h
        
FORMS += \
    seisprofilewidget.ui
