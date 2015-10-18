#-------------------------------------------------
#
# Project created by QtCreator 2014-10-19T20:11:39
#
#-------------------------------------------------

QT      += core gui
QT      += network
QT      += core
QT      += sensors widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HomeControl
TEMPLATE = app


SOURCES += main.cpp\
        homecontrol.cpp \
        downloader.cpp

HEADERS  += homecontrol.h \
        downloader.h

FORMS    += homecontrol.ui

CONFIG += mobility
MOBILITY += systeminfo

RESOURCES +=

DISTFILES += \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

