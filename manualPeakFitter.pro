#-------------------------------------------------
#
# Project created by QtCreator 2016-01-13T17:13:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = manualPeakFitter
win32:TARGET_EXT = .exe
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../libs/qcustomplot/qcustomplot.cpp \
    masslibrary.cpp

HEADERS  += mainwindow.h \
    ../libs/qcustomplot/qcustomplot.h \
    masslibrary.h

FORMS    += mainwindow.ui

#INCLUDEPATH += ../libs/eigen-eigen-b30b87236a1b/Eigen
INCLUDEPATH += ../libs/eigen-eigen-323c052e1731/Eigen


#win32:INCLUDEPATH += ../libs/CMake-hdf5-1.8.16/CMake-hdf5-1.8.16/hdf5-1.8.16-build
#win32:INCLUDEPATH += ../libs/CMake-hdf5-1.8.16/CMake-hdf5-1.8.16/hdf5-1.8.16/src
#win32:INCLUDEPATH += ../libs/CMake-hdf5-1.8.16/CMake-hdf5-1.8.16/hdf5-1.8.16/c++/src
win32:INCLUDEPATH += C:\msys64\mingw32\include
#win32:LIBS += -L../libs/hdf5-1.8.16-bin/bin
win32:LIBS += -LC:\msys64\mingw32\lib

linux:INCLUDEPATH += /usr/include/hdf5/serial
linux:LIBS +=  -L/usr/lib/x86_64-linux-gnu/hdf5/serial  -lhdf5_cpp -lhdf5 #-lhdf5-serial #-lhdf5
#win32:LIBS += \
#    -lhdf5_cpp-static \
#    -lhdf5-static\
#    -lhdf5_hl-static \
#    -lhdf5_hl_cpp-static \
#    -lhdf5_tools-static
win32:LIBS += \
    -lhdf5_cpp \
    -lhdf5_cpp \
    -lhdf5 \
    -lhdf5_hl \
    -lhdf5_hl_cpp \
    -lhdf5_tools \
    -lszip \
    -lz

win32:QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static # statically link libGCC, STDlib, winpthread, ...

win32 {
    DEPLOY_COMMAND = windeployqt # collects dll dependencies

CONFIG( debug, debug|release ) {
    # debug
    # DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/debug/$${TARGET}$${TARGET_EXT}))
    DEPLOY_TARGET = $${OUT_PWD}/debug/$${TARGET}$${TARGET_EXT}
    # PKG_DESTINATION = $$shell_quote($$shell_path($${OUT_PWD}/debug/pkg/))
} else {
    # release
    # DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/release/$${TARGET}$${TARGET_EXT}))
    DEPLOY_TARGET = $${OUT_PWD}/release/$${TARGET}$${TARGET_EXT}
    # PKG_DESTINATION = $$shell_quote($$shell_path($${OUT_PWD}/release/pkg/))
}

#  # Uncomment the following line to help debug the deploy command when running qmake
warning($${DEPLOY_COMMAND} $${DEPLOY_TARGET})

QMAKE_POST_LINK = $${DEPLOY_COMMAND} $${DEPLOY_TARGET} #--dir $${DLLDESTDIR} --no-translations
}
