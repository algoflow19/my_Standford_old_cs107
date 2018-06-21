TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../../assn-2-six-degrees/imdb.cc \
    ../../assn-2-six-degrees/imdb-test.cc \
    ../../assn-2-six-degrees/path.cc \
    ../../assn-2-six-degrees/six-degrees.cc

DISTFILES += \
    ../../assn-2-six-degrees/imdb-test-linux \
    ../../assn-2-six-degrees/Makefile

HEADERS += \
    ../../assn-2-six-degrees/imdb.h \
    ../../assn-2-six-degrees/imdb-utils.h \
    ../../assn-2-six-degrees/path.h
