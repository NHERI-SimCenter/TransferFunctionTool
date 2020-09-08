QT       += core gui webchannel
unix: QT += svg opengl concurrent
win32: QT += opengl printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

win32 {
   RC_ICONS = resources/NHERI-TFT-Icon.ico
} else {
   mac {
      ICON = resources/NHERI-TFT-Icon.icns
   }
}


macx {

    DEFINES += "_MacOSX"
    #LIBS += /Users/fmk/lib/libLapack.a
    #LIBS += /Users/fmk/lib/libBlas.a
    #LIBS += /usr/local/llvm-gcc/lib/libgfortran.a

    #LIBS += -llapack -lblas
    LIBS +=  -lmkl_core -lmkl_intel_ilp64 -lmkl_intel_lp64 -lmkl_sequential
}

linux {

    DEFINES += "_LINUX"

    LIBS += -llapack -lblas

}

win32 {

INCLUDEPATH += "%MKLROOT%"\include

#LIBS += -L"C:\Program Files (x86)\IntelSWTools\compilers_and_libraries\windows\mkl\lib\intel64" mkl_intel_lp64.lib mkl_sequential.lib mkl_core.lib
#LIBS += -L"C:\Program Files (x86)\IntelSWTools\compilers_and_libraries\windows\mkl\lib\intel64" mkl_lapack95_lp64.lib mkl_blas95_lp64.lib

LIBS +=  -lmkl_core -lmkl_intel_ilp64 -lmkl_intel_lp64 -lmkl_sequential

}


include(../SimCenterCommon/Common/Common.pri)
include(../SimCenterCommon/SimFigure/SimFigure.pri)

include($$PWD/ConanHelper.pri)

win32: include(C:\qwt-6.1.5\features\qwt.prf)
unix: include(/usr/local/qwt-6.1.4/features/qwt.prf)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


SOURCES += \
    UI/BonzaTableModel.cpp \
    UI/BonzaTableView.cpp \
    mainwindow.cpp \
    main.cpp

HEADERS += \
    GlobalConstances.h \
    UI/BonzaTableModel.h \
    UI/BonzaTableView.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    ConanHelper.pri \
    conanfile.py

RESOURCES += \
    resource.qrc

