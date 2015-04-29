#-------------------------------------------------
#
# Project created by QtCreator 2015-04-29T14:04:41
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
    VCSBPSIBookers/vcsbpsibooker.cpp \
    VCSBPSIBookers/tspbooker.cpp \
    VCSBPSIBookers/mppbooker.cpp \
    VCSBPSIBookers/evpbooker.cpp \
    VCSBPSIBookers/averagebooker.cpp \
    VCSBPPSolvers/vcsbppsolver.cpp \
    VCSBPPSolvers/mipvscbppsolver.cpp \
    VCSBPPSolvers/abdf.cpp \
    src/EBM.cpp \
    src/binutility.cpp

HEADERS += \
    VCSBPSIBookers/vcsbpsibooker.h \
    VCSBPSIBookers/tspbooker.h \
    VCSBPSIBookers/mppbooker.h \
    VCSBPSIBookers/evpbooker.h \
    VCSBPSIBookers/averagebooker.h \
    VCSBPPSolvers/vcsbppsolver.h \
    VCSBPPSolvers/mipvscbppsolver.h \
    VCSBPPSolvers/abdf.h \
    include/structDef.h \
    include/EBM.h \
    include/binutility.h

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
