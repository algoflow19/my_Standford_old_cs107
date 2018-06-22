TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    hashset.c \
    hashsettest.c \
    streamtokenizer.c \
    thesaurus-lookup.c \
    vector.c \
    vectortest.c

HEADERS += \
    bool.h \
    hashset.h \
    streamtokenizer.h \
    vector.h
