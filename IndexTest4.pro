QT += core
QT += gui
QT += widgets
CONFIG += c++11

TARGET = IndexTest4
CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    HashTable/hashtable.cpp \
    Index/avlindex.cpp \
    Index/hashindex.cpp \
    Parser/parser.cpp \
    Word/word.cpp \
    stemmer.cpp \
    QueryProcessor/queryprocessor.cpp \
    Word/docdetails.cpp \
    gui.cpp

HEADERS += \
    AVLTree/avltree.h \
    AVLTree/node.h \
    HashTable/hashtable.h \
    Index/avlindex.h \
    Index/hashindex.h \
    Index/indexinterface.h \
    Parser/parser.h \
    Word/word.h \
    stemmer.h \
    QueryProcessor/queryprocessor.h \
    Word/docdetails.h \
    gui.h


LIBS += -lpoppler-cpp \
        -lstdc++fs \
        -lncurses

