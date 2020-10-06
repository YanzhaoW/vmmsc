#####################################################
#####################################################
# IMPORTANT: FOR SRS, COMMENT OUT THE DEFINES
#####################################################
#####################################################
#DEFINES += ASSISTER=1
#####################################################
#####################################################
#####################################################


#####################################################
# directory and machine specific items here
#####################################################
linebreak="---------------------------------------------------------------"



sourcepath=""
includepath=""
imagepath=""
type=""

linux {
    sourcepath="../src"
    includepath="../include"
    imagepath="../images"
} else {
    sourcepath="../src"
    includepath="../include"
    imagepath="../images"
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

greaterThan(QT_MAJOR_VERSION, 4): QT +=widgets printsupport
CONFIG  += console
CONFIG  +=declarative_debug
CONFIG  +=c++11
CONFIG  += sanitizer sanitize_address

TARGET   = vmmdcs
TEMPLATE = app


INCLUDEPATH += $$includepath
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
           $$sourcepath/mainwindow.cpp\
           $$sourcepath/socket_handler.cpp\
           $$sourcepath/vmmsocket.cpp\
           $$sourcepath/message_handler.cpp\
           $$sourcepath/commandline.cpp \
           $$sourcepath/vmm_config_handler.cpp \
           $$sourcepath/daq_config_handler.cpp \
           $$sourcepath/fec_config_handler.cpp \
            $$sourcepath/vmm.cpp \
            $$sourcepath/daq.cpp \
            $$sourcepath/hybrid.cpp \
            $$sourcepath/hdmi.cpp \
            $$sourcepath/fec.cpp \
            $$sourcepath/daq_window.cpp \
            $$sourcepath/fec_window.cpp \
            $$sourcepath/hdmi_window.cpp \
            $$sourcepath/hybrid_window.cpp \
            $$sourcepath/vmm_window.cpp \
            $$sourcepath/hybrid_config_handler.cpp \
            $$sourcepath/calibration_module.cpp \
            $$sourcepath/qcustomplot.cpp \
            $$sourcepath/fec_config_module.cpp

HEADERS  += $$includepath/mainwindow.h\
            $$includepath/socket_handler.h\
            $$includepath/vmmsocket.h\
            $$includepath/message_handler.h\
            $$includepath/vmm_config_handler.h\
            $$includepath/calibration_module.h\
            $$includepath/daq_config_handler.h\
            $$includepath/commandline.h \
            $$includepath/globparameter.h \
            $$includepath/qcustomplot.h \
            $$includepath/vmm.h \
            $$includepath/daq.h \
            $$includepath/hybrid.h \
            $$includepath/hdmi.h \
            $$includepath/fec.h \
            $$includepath/daq_window.h \
            $$includepath/fec_window.h \
            $$includepath/hdmi_window.h \
            $$includepath/hybrid_window.h \
            $$includepath/vmm_window.h \
            $$includepath/hybrid_config_handler.h \
            $$includepath/fec_config_handler.h \
            $$includepath/fec_config_module.h




FORMS    += $$sourcepath/daq_window.ui \
            $$sourcepath/vmm_window.ui \
            $$sourcepath/hybrid_window.ui \
            $$sourcepath/fec_window.ui \
            $$sourcepath/hdmi_window.ui

RESOURCES += \
    $$imagepath/icons.qrc

DISTFILES +=
