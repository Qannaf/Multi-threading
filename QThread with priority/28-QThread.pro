QT += core
QT -= gui

CONFIG += c++11

TARGET = 28-QThread
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    mythread.cpp

HEADERS += \
    mythread.h
