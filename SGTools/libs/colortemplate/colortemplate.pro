TEMPLATE = lib
TARGET = ColorTemplate

DEFINES += COLORTEMPLATE_LIBRARY

include(../../common.pri)
include(../qwt/qwt.pri)

SOURCES += \
    colordata.cpp \
    colormanager.cpp \
    colortemplateplot.cpp \
    colortemplatewidget.cpp \
    transcanvaspicker.cpp \
    transtemplateplot.cpp \
    colortemplatedialog.cpp \
    valuedistributioncounter.cpp \
    colorcanvaspicker.cpp

HEADERS += \
    colortemplate_global.h \
    colordata.h \
    colormanager.h \
    colortemplateplot.h \
    colortemplatewidget.h \
    transcanvaspicker.h \
    transtemplateplot.h \
    colortemplatedialog.h \
    valuedistributioncounter.h \
    colorcanvaspicker.h

FORMS += \
    colortemplatedialog.ui

