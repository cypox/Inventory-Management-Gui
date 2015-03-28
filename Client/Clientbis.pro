TEMPLATE = app
TARGET = Client
DEPENDPATH += .
INCLUDEPATH += .
QT += core gui console network webkit


# Input
HEADERS += authform.h \
    widgetaffichage.h \
    intersocketclient.h \
    formtype.h \
    fengenerale.h \
    tabresults.h
FORMS += authform.ui \
    widgetaffichage.ui \
    formtype.ui \
    fengenerale.ui \
    tabresults.ui
SOURCES += authform.cpp \
    widgetaffichage.cpp \
    main.cpp \
    intersocketclient.cpp \
    formtype.cpp \
    fengenerale.cpp \
    tabresults.cpp