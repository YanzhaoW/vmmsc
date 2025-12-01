#####################################################
# Define default value for clock source
# 0 = ESS readout, clock via ring (44.02625 MHz)
# 1 = ESS readout, clock from KC705 (44.444 MHz)
# 2 = SRS, ESS firmware with 44.4444 MHz
# 3 = SRS, RD51 firmware with 40 MHz
DEFINES += CLOCK_SOURCE=2
#####################################################
# Define default value for slow control (does not matter for SRS FEC)
# only relevant for ESS assister
# 0 = ESS over master
# 1 = ESS over assister
# 2 = SRS
DEFINES += SLOW_CONTROL=2
#####################################################
# Define board type
# 0 = SRS
# 1 = KC705
# 2 = uROC
DEFINES += BOARD=0
#####################################################
# directory and machine specific items here
#####################################################
linebreak="---------------------------------------------------------------"



sourcepath=""
includepath=""
alglibpath=""
imagepath=""
type=""

linux {
    sourcepath="../src"
    includepath="../include"
    imagepath="../images"
    alglibpath="../alglib"
} else {
    sourcepath="../src"
    includepath="../include"
    imagepath="../images"
    alglibpath="../alglib"
}

message($$linebreak)
message("qmake sourcepath:      $$sourcepath")
message("qmake includepath:     $$includepath")
message($$linebreak)

#####################################################

QT      += core gui
QT      += network
QT      += widgets
QT      += xml


greaterThan(QT_MAJOR_VERSION, 4): QT +=widgets printsupport
CONFIG  += console
CONFIG  +=declarative_debug
CONFIG  +=c++11
#CONFIG  += sanitizer sanitize_address

TARGET   = vmmdcs
TEMPLATE = app


INCLUDEPATH += $$includepath
INCLUDEPATH += $$alglibpath
DEPENDPATH  += $$includepath

OBJECTS_DIR += ./objects/
MOC_DIR     += ./moc/
RCC_DIR     += ./rcc/
UI_DIR      += ./ui/

linux {
    QMAKE_CXXFLAGS += -std=c++17
    QMAKE_CXXFLAGS += -fmax-errors=3
    QMAKE_CXXFLAGS += -isystem /usr/include/x86_64-linux-gnu/qt6
}
else:win32 {
    LIBS += -lws2_32
}
else {
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_CXXFLAGS += -std=c++17
    QMAKE_LFLAGS   += -stdlib=libc++
}


SOURCES += $$sourcepath/main.cpp\
           $$sourcepath/socket_handler.cpp\
           $$sourcepath/vmmsocket.cpp\
           $$sourcepath/message_handler.cpp\
            $$sourcepath/vmm.cpp \
            $$sourcepath/daq.cpp \
            $$sourcepath/hybrid.cpp \
            $$sourcepath/fec.cpp \
            $$sourcepath/daq_window.cpp \
            $$sourcepath/daq_config.cpp \
            $$sourcepath/calibration_module.cpp \
            $$sourcepath/qcustomplot.cpp \
            $$sourcepath/fec_config_module.cpp \
            $$alglibpath/alglibinternal.cpp \
            $$alglibpath/alglibmisc.cpp \
            $$alglibpath/ap.cpp \
            $$alglibpath/integration.cpp \
            $$alglibpath/interpolation.cpp \
            $$alglibpath/linalg.cpp \
            $$alglibpath/optimization.cpp \
            $$alglibpath/solvers.cpp \
            $$alglibpath/specialfunctions.cpp

HEADERS  += $$includepath/socket_handler.h\
            $$includepath/vmmsocket.h\
            $$includepath/message_handler.h\
            $$includepath/calibration_module.h\
            $$includepath/daq_config.h\
            $$includepath/globparameter.h \
            $$includepath/qcustomplot.h \
            $$includepath/vmm.h \
            $$includepath/daq.h \
            $$includepath/hybrid.h \
            $$includepath/fec.h \
            $$includepath/daq_window.h \
            $$includepath/fec_config_module.h \
            $$includepath/vmm_sys_regs_map.h \
            $$alglibpath/alglibinternal.h \
            $$alglibpath/alglibmisc.h \
            $$alglibpath/ap.h \
            $$alglibpath/integration.h \
            $$alglibpath/interpolation.h \
            $$alglibpath/linalg.h \
            $$alglibpath/optimization.h \
            $$alglibpath/solvers.h \
            $$alglibpath/specialfunctions.h \
            $$alglibpath/stdafx.h




FORMS    += $$sourcepath/daq_window.ui

RESOURCES += \
    $$imagepath/icons.qrc

DISTFILES +=
