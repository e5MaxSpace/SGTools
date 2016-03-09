#-------------------------------------------------
#
# Project created by QtCreator 2016-03-09T14:30:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = seisprofile
TEMPLATE = app

include(../../common.pri)
include(../../libs/qwt/qwt.pri)
include(../../libs/colortemplate/colortemplate.pri)
include(../../libs/seis/seis.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    polylineitem.cpp \
    profilemainwindow.cpp \
    profileseishorizonlayer.cpp \
    profilewellcolumnlayer.cpp \
    qiglayer.cpp \
    qigscene.cpp \
    qremotefile.cpp \
    segyread.cpp \
    seis3dsurvey.cpp \
    seishorizon.cpp \
    seishorizonlineitem.cpp \
    seistest.cpp \
    seisvolumn.cpp \
    surveymatrixdata.cpp \
    surveypoint.cpp

HEADERS  += mainwindow.h \
    polylineitem.h \
    profilemainwindow.h \
    profileseishorizonlayer.h \
    profilewellcolumnlayer.h \
    qiglayer.h \
    qigscene.h \
    qremotefile.h \
    segyread.h \
    seis3dsurvey.h \
    seishorizon.h \
    seishorizonlineitem.h \
    seistest.h \
    seisvolumn.h \
    surveymatrixdata.h \
    surveypoint.h

FORMS    += mainwindow.ui \
    profilemainwindow.ui
