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
    VCSBPPSolvers/vcsbppsolver.cpp \
    VCSBPPSolvers/abdf.cpp \
    VCSBPPSolvers/mipvscbppsolver.cpp \
    vcsbpsibooker.cpp \
    VCSBPSIBookers/mppbooker.cpp \
    VCSBPSIBookers/tspbooker.cpp \
    src/binutility.cpp \
    VCSBPSIBookers/averagebooker.cpp \
    VCSBPSIBookers/evpbooker.cpp \
    VCSBPSIBookers/vcsbpsibooker.cpp



HEADERS += \
    include/EBM.h \
    include/structDef.h \
    VCSBPPSolvers/vcsbppsolver.h \
    VCSBPPSolvers/abdf.h \
    VCSBPPSolvers/mipvscbppsolver.h \
    VCSBPSIBookers/vcsbpsibooker.h \
    VCSBPSIBookers/mppbooker.h \
    VCSBPSIBookers/tspbooker.h \
    include/binutility.h \
    VCSBPSIBookers/averagebooker.h \
    VCSBPSIBookers/evpbooker.h


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
