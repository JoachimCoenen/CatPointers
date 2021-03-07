CONFIG -= qt

TEMPLATE = lib

CONFIG += c++17

HEADERS += \
    src/cat_owningPtr.h \
	src/cat_sharedPtr.h \
	src/cat_weakPtr.h

INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src
