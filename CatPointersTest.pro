QT += testlib
QT -= gui

CONFIG += c++17
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
	test/weakPtrTest.cpp \
	test/owningPtrTest.cpp \
	test/sharedPtrTest.cpp \
	test/autoTest.cpp

HEADERS += \
	test/autoTest.h

INCLUDEPATH += $$PWD/src
