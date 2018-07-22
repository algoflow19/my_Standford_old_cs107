TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS +=-lpthread

SOURCES += \
    ../../Music/assn-4-rss-news-search/queryHashSet.c \
    ../../Music/assn-4-rss-news-search/rss-news-search.c

HEADERS += \
    ../../Music/assn-4-rss-news-search/bool.h \
    ../../Music/assn-4-rss-news-search/hashset.h \
    ../../Music/assn-4-rss-news-search/html-utils.h \
    ../../Music/assn-4-rss-news-search/queryHashSet.h \
    ../../Music/assn-4-rss-news-search/stirngHashSet.h \
    ../../Music/assn-4-rss-news-search/streamtokenizer.h \
    ../../Music/assn-4-rss-news-search/url.h \
    ../../Music/assn-4-rss-news-search/urlconnection.h \
    ../../Music/assn-4-rss-news-search/vector.h

DISTFILES += \
    ../assn-4/assn-4-rss-news-search-lib/librssnews.a \
    ../assn-4/assn-4-rss-news-search-lib/librssnews_pure_p9_c0_112312015_32.a \
    ../../Music/assn-4-rss-news-search/librssnews.a \
    ../../Music/assn-4-rss-news-search/librssnews_pure_p9_c0_112312015_32.a
