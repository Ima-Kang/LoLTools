QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = res/ChampionSquare.ico

LIBS += -luser32

SOURCES += \
    src/accountinfo.cpp \
    src/adddialog.cpp \
    src/editdialog.cpp \
    src/main.cpp \
    src/accountmanager.cpp \
    src/profile.cpp \
    src/removedialog.cpp \
    src/script.cpp \
    src/hotkey.cpp \
    src/settings.cpp \

HEADERS += \
    headers/accountinfo.h \
    headers/accountmanager.h \
    headers/adddialog.h \
    headers/editdialog.h \
    headers/removedialog.h \
    headers/script.h \
    headers/hotkey.h \
    headers/settings.h \
    headers/profile.h

FORMS += \
    forms/accountmanager.ui \
    forms/adddialog.ui \
    forms/editdialog.ui \
    forms/removedialog.ui \
    forms/settings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    res/qss.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../opencv-4.5.4/opencv/build/x64/vc15/lib/ -lopencv_world454
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../opencv-4.5.4/opencv/build/x64/vc15/lib/ -lopencv_world454d

INCLUDEPATH += $$PWD/../../../../../opencv-4.5.4/opencv/build/include
DEPENDPATH += $$PWD/../../../../../opencv-4.5.4/opencv/build/include

win32: LIBS += -lGdi32
