QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = res/ChampionSquare.ico
QT += network

SOURCES += \
    src/accountinfo.cpp \
    src/adddialog.cpp \
    src/editdialog.cpp \
    src/main.cpp \
    src/accountmanager.cpp \
    src/removedialog.cpp

HEADERS += \
    headers/accountinfo.h \
    headers/accountmanager.h \
    headers/adddialog.h \
    headers/editdialog.h \
    headers/removedialog.h

FORMS += \
    forms/accountmanager.ui \
    forms/adddialog.ui \
    forms/editdialog.ui \
    forms/removedialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .key

RESOURCES += \
    res/qss.qrc
