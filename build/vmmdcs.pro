#####################################################
# IMPORTANT: FOR SRS, COMMENT OUT THE DEFINE BELOW
#####################################################
#####################################################
#DEFINES += ASSISTER=1
#####################################################

#####################################################
# COMMENT OUT, if you do not want config check
#####################################################
#####################################################
#DEFINES += CONFIG_CHECK=1
#####################################################


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
QT      += concurrent
QT      += serialport
QT      += sql

greaterThan(QT_MAJOR_VERSION, 4): QT +=widgets printsupport
CONFIG  += console
CONFIG  +=declarative_debug
CONFIG  +=c++11
CONFIG  += sanitizer sanitize_address

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
    QMAKE_CXXFLAGS += -std=c++11
}
else:win32 {
    LIBS += -lws2_32
}
else {
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_LFLAGS   += -stdlib=libc++
}


SOURCES += $$sourcepath/main.cpp\
           $$sourcepath/socket_handler.cpp\
           $$sourcepath/vmmsocket.cpp\
           $$sourcepath/message_handler.cpp\
           $$sourcepath/vmm_config_handler.cpp \
           $$sourcepath/daq_config_handler.cpp \
           $$sourcepath/fec_config_handler.cpp \
            $$sourcepath/vmm.cpp \
            $$sourcepath/daq.cpp \
            $$sourcepath/hybrid.cpp \
            $$sourcepath/fec.cpp \
            $$sourcepath/daq_window.cpp \
            $$sourcepath/fec_window.cpp \
            $$sourcepath/hybrid_window.cpp \
            $$sourcepath/vmm_window.cpp \
            $$sourcepath/hybrid_config_handler.cpp \
            $$sourcepath/calibration_module.cpp \
            $$sourcepath/qcustomplot.cpp \
            $$sourcepath/fec_config_module.cpp \
            $$sourcepath/test_module.cpp \
            $$sourcepath/currentmonitor.cpp \
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
            $$includepath/vmm_config_handler.h\
            $$includepath/calibration_module.h\
            $$includepath/daq_config_handler.h\
            $$includepath/globparameter.h \
            $$includepath/qcustomplot.h \
            $$includepath/vmm.h \
            $$includepath/daq.h \
            $$includepath/hybrid.h \
            $$includepath/fec.h \
            $$includepath/daq_window.h \
            $$includepath/fec_window.h \
            $$includepath/hybrid_window.h \
            $$includepath/vmm_window.h \
            $$includepath/hybrid_config_handler.h \
            $$includepath/fec_config_handler.h \
            $$includepath/fec_config_module.h \
            $$includepath/test_module.h \
            $$includepath/currentmonitor.h \
            $$includepath/PolynomialRegression.h \
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




FORMS    += $$sourcepath/daq_window.ui \
            $$sourcepath/vmm_window.ui \
            $$sourcepath/fec_window.ui \
            $$sourcepath/hybrid_window.ui

RESOURCES += \
    $$imagepath/icons.qrc

DISTFILES +=
