#-------------------------------------------------
#
# Project created by QtCreator 2018-03-24T17:39:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DetailedExample
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

DEFINES += ELPP_FEATURE_ALL \ # Optional: For stack trace
    ELPP_MULTI_LOGGER_SUPPORT \
    ELPP_THREAD_SAFE \ # This is absolutely required
    ELPP_QT_LOGGING # We also log qt classes

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h \
        log.h

FORMS += \
        mainwindow.ui

LIBS += -L"/usr/local/lib" -lresidue

## Following has nothing to do with residue, it's just setup issue with Qt
## http://stackoverflow.com/questions/38131011/qt-application-throws-dyld-symbol-not-found-cg-jpeg-resync-to-restart
LIBS += -L"/System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/"

INCLUDEPATH += /usr/local/include/
