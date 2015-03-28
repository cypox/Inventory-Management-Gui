#-------------------------------------------------
#
# Project created by QtCreator 2013-05-06T22:11:33
#
#-------------------------------------------------


TEMPLATE = app
TARGET = Serveur
DEPENDPATH += .
INCLUDEPATH += .
QT += core gui sql console network


SOURCES += main.cpp\
        fenserveur.cpp\
		basededonnees.cpp\
    tcpsocketclient.cpp \
    formstart.cpp

HEADERS  += fenserveur.h\
		basededonnees.h\
    tcpsocketclient.h \
    formstart.h

FORMS    += fenserveur.ui \
    formstart.ui
