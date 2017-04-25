QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

CONFIG += c++11

TARGET = Renderer

TEMPLATE = app

LIBS += -pthread
CONFIG += link_pkgconfig
PKGCONFIG += x11 opencv

SOURCES += main.cpp \
    glwindow.cpp \
    mesh.cpp \
    shader.cpp \
    scene.cpp \
    camera.cpp \
    shapefs.cpp \
    fastm.cpp \
    facedet.cpp \
    ../dlib-18.18/dlib/all/source.cpp \
    recorder.cpp \
    facebuilder.cpp \
    mainwindow.cpp \
    glwidget.cpp \
    webcamwidget.cpp \
    mainmenu.cpp \
    facedatabase.cpp \
    qvecoperator.cpp \
    pstereowidget.cpp \
    fastmarchingwidget.cpp \
    facedatabasewidget.cpp \
    meshbuilder.cpp

HEADERS += \
    glwindow.h \
    mesh.h \
    shader.h \
    scene.h \
    camera.h \
    shapefs.h \
    fastm.h \
    recorder.h \
    facebuilder.h \
    mainwindow.h \
    glwidget.h \
    webcamwidget.h \
    mainmenu.h \
    facedatabase.h \
    qvecoperator.h \
    pstereowidget.h \
    fastmarchingwidget.h \
    facedatabasewidget.h \
    meshbuilder.h \
    facedetector.h

DISTFILES += \
    shader.frag \
    shader.vert

INCLUDEPATH += ../dlib-18.18
INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs
