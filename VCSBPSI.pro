#-------------------------------------------------
#
# Project created by QtCreator 2015-03-16T15:42:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = VCSBPSI
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++11


TEMPLATE = app


SOURCES += main.cpp \
    src/EBM.cpp \
    vcsbppsolver.cpp \
    abdf.cpp \
    mipvscbppsolver.cpp \
    vcsbpsibooker.cpp \
    mppbooker.cpp \
    tspbooker.cpp \
    binutility.cpp \
    averagebooker.cpp \
    evpbooker.cpp



HEADERS += \
    include/EBM.h \
    include/structDef.h \
    vcsbppsolver.h \
    abdf.h \
    mipvscbppsolver.h \
    vcsbpsibooker.h \
    mppbooker.h \
    tspbooker.h \
    binutility.h \
    averagebooker.h \
    evpbooker.h


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../opt/gurobi602/linux64/lib/release/ -lgurobi_c++
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../opt/gurobi602/linux64/lib/debug/ -lgurobi_c++
else:unix: LIBS += -L$$PWD/../../../opt/gurobi602/linux64/lib/ -lgurobi_c++

INCLUDEPATH += $$PWD/../../../opt/gurobi602/linux64/include
DEPENDPATH += $$PWD/../../../opt/gurobi602/linux64/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../opt/gurobi602/linux64/lib/release/libgurobi_c++.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../opt/gurobi602/linux64/lib/debug/libgurobi_c++.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../opt/gurobi602/linux64/lib/release/gurobi_c++.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../opt/gurobi602/linux64/lib/debug/gurobi_c++.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../opt/gurobi602/linux64/lib/libgurobi_c++.a


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../opt/gurobi602/linux64/lib/release/ -lgurobi60
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../opt/gurobi602/linux64/lib/debug/ -lgurobi60
else:unix: LIBS += -L$$PWD/../../../opt/gurobi602/linux64/lib/ -lgurobi60

INCLUDEPATH += $$PWD/../../../opt/gurobi602/linux64/include
DEPENDPATH += $$PWD/../../../opt/gurobi602/linux64/include
