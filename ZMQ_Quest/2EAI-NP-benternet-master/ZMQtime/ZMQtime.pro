TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
QT += concurrent

DEFINES += ZMQ_STATIC
LIBS += -L$$PWD/../lib -lnzmqt -lzmq -lws2_32 -lIphlpapi
INCLUDEPATH += $$PWD/../include

SOURCES += main.cpp \

HEADERS += \
#    $$PWD/../include/nzmqt/nzmqt.hpp \
#    $$PWD/../include/nzmqt/global.hpp \
#    $$PWD/../include/nzmqt/impl.hpp
