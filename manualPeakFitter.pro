#-------------------------------------------------
#
# Project created by QtCreator 2016-01-13T17:13:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = manualPeakFitter
TARGET_EXT = .exe
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../libs/qcustomplot/qcustomplot.cpp \
    masslibrary.cpp

HEADERS  += mainwindow.h \
    ../libs/qcustomplot/qcustomplot.h \
    masslibrary.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../libs/eigen-eigen-b30b87236a1b/Eigen

win32:INCLUDEPATH += ../libs/HDF5-1.8.16-win32-mingw/HDF5-1.8.16-win32/include

win32:LIBS += -L../libs/HDF5-1.8.16-win32-mingw/HDF5-1.8.16-win32/lib

linux:LIBS += -lhdf5 -lhdf5_cpp
win32:LIBS += \
    -lhdf5_cpp-static \
    -lhdf5-static\
    -lhdf5_hl-static \
    -lhdf5_hl_cpp-static \
    -lhdf5_tools-static

win32 {
    DEPLOY_COMMAND = windeployqt # collects dll dependencies

CONFIG( debug, debug|release ) {
    # debug
    DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/debug/$${TARGET}$${TARGET_EXT}))
    # PKG_DESTINATION = $$shell_quote($$shell_path($${OUT_PWD}/debug/pkg/))
} else {
    # release
    DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/release/$${TARGET}$${TARGET_EXT}))
    # PKG_DESTINATION = $$shell_quote($$shell_path($${OUT_PWD}/release/pkg/))
}

#  # Uncomment the following line to help debug the deploy command when running qmake
warning($${DEPLOY_COMMAND} $${DEPLOY_TARGET})

QMAKE_POST_LINK = $${DEPLOY_COMMAND} $${DEPLOY_TARGET} #--dir $${DLLDESTDIR} --no-translations
}
