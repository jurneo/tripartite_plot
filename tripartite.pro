#-------------------------------------------------
#
# Project created by QtCreator 2013-04-10T23:46:33
#
#-------------------------------------------------

QT       += core gui
CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tripartite
TEMPLATE = app
CONFIG += console

INCLUDEPATH += ./include

Debug:LIBS += -L./lib -lqwtd
Release:LIBS += -L./lib -lqwt

SOURCES += main.cpp\
    mainwindow.cpp \
    tripartitegrid.cpp \
    reader.cpp

HEADERS  += mainwindow.h \
    tripartitegrid.h \
    reader.h

INCLUDEPATH += ./src
