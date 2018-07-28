TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS +=-lpthread

SOURCES += \
        main.c \
    vector.c

HEADERS += \
    vector.h
