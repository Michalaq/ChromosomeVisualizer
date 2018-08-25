QT       += core gui opengl svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qcstudio
TEMPLATE = app


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ../QtChromosomeViz_v2/VizWidget.cpp \
    ../QtChromosomeViz_v2/bartekm_code/Simulation.cpp \
    draggable.cpp \
    camera.cpp \
    plot.cpp \
    mediacontrol.cpp \
    dockwidget.cpp \
    spinbox.cpp \
    rangeslider.cpp \
    legend.cpp \
    toolbar.cpp \
    rendersettings.cpp \
    projectsettings.cpp \
    slider.cpp \
    softslider.cpp \
    ../QtChromosomeViz_v2/bartekm_code/SimulationLayer.cpp \
    ../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.cpp \
    ../QtChromosomeViz_v2/bartekm_code/ProtobufSimulationlayer.cpp \
    treeitem.cpp \
    treemodel.cpp \
    ../QtChromosomeViz_v2/bartekm_code/SimulationLayerConcatenation.cpp \
    atomattributes.cpp \
    picker.cpp \
    layerattributes.cpp \
    viewport.cpp \
    multimap.cpp \
    doublespinbox.cpp \
    combobox.cpp \
    lineedit.cpp \
    fileedit.cpp \
    label.cpp \
    keyframe.cpp \
    interpolator.cpp \
    defaults.cpp \
    selection.cpp \
    visibilitydelegate.cpp \
    treeview.cpp \
    textedit.cpp \
    preferences.cpp \
    namedelegate.cpp \
    material.cpp \
    tagsdelegate.cpp \
    flowlayout.cpp \
    materialattributes.cpp \
    materialbrowser.cpp \
    tablemodel.cpp \
    tableview.cpp \
    moviemaker.cpp \
    attributes.cpp \
    titlelabel.cpp \
    pickwidget.cpp \
    cameraattributes.cpp \
    importdialog.cpp \
    labelatlas.cpp \
    statusbar.cpp \
    fontcombobox.cpp \
    session.cpp \
    mediapanel.cpp \
    customstyle.cpp \
    tabwidget.cpp \
    jpegdialog.cpp

HEADERS += \
    mainwindow.h \
    ../QtChromosomeViz_v2/VizWidget.hpp \
    ../QtChromosomeViz_v2/bartekm_code/Simulation.h \
    ../QtChromosomeViz_v2/bartekm_code/common.h \
    draggable.h \
    camera.h \
    plot.h \
    mediacontrol.h \
    dockwidget.h \
    spinbox.h \
    rangeslider.h \
    legend.h \
    toolbar.h \
    rendersettings.h \
    moviemaker.h \
    projectsettings.h \
    slider.h \
    softslider.h \
    ../QtChromosomeViz_v2/bartekm_code/SimulationLayer.h \
    ../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.h \
    ../QtChromosomeViz_v2/bartekm_code/ProtobufSimulationlayer.h \
    ../QtChromosomeViz_v2/bartekm_code/include/header/generic_header.h \
    ../QtChromosomeViz_v2/bartekm_code/include/header/magic_value.h \
    ../QtChromosomeViz_v2/bartekm_code/include/header/placeholder.h \
    ../QtChromosomeViz_v2/bartekm_code/include/header/with_offset.h \
    ../QtChromosomeViz_v2/bartekm_code/include/cache.h \
    ../QtChromosomeViz_v2/bartekm_code/include/common.h \
    ../QtChromosomeViz_v2/bartekm_code/include/config.h \
    ../QtChromosomeViz_v2/bartekm_code/include/file_backend.h \
    ../QtChromosomeViz_v2/bartekm_code/include/header.h \
    ../QtChromosomeViz_v2/bartekm_code/include/mmap_backend.h \
    ../QtChromosomeViz_v2/bartekm_code/include/mmap_guard.h \
    ../QtChromosomeViz_v2/bartekm_code/include/posix_file_backend.h \
    ../QtChromosomeViz_v2/bartekm_code/include/posix_file_handler.h \
    ../QtChromosomeViz_v2/bartekm_code/include/stream.h \
    ../QtChromosomeViz_v2/bartekm_code/include/utils.h \
    treeitem.h \
    treemodel.h \
    ../QtChromosomeViz_v2/bartekm_code/SimulationLayerConcatenation.h \
    atomattributes.h \
    picker.h \
    layerattributes.h \
    viewport.h \
    multimap.h \
    doublespinbox.h \
    combobox.h \
    lineedit.h \
    fileedit.h \
    label.h \
    spline.h \
    keyframe.h \
    interpolator.h \
    defaults.h \
    selection.h \
    visibilitydelegate.h \
    treeview.h \
    textedit.h \
    preferences.h \
    namedelegate.h \
    material.h \
    tagsdelegate.h \
    flowlayout.h \
    materialattributes.h \
    materialbrowser.h \
    tablemodel.h \
    tableview.h \
    attributes.h \
    titlelabel.h \
    pickwidget.h \
    cameraattributes.h \
    importdialog.h \
    labelatlas.h \
    statusbar.h \
    fontcombobox.h \
    session.h \
    glbuffer.h \
    mediapanel.h \
    customstyle.h \
    tabwidget.h \
    jpegdialog.h

FORMS += mainwindow.ui \
    dockwidget.ui \
    projectsettings.ui \
    rendersettings.ui \
    atomattributes.ui \
    layerattributes.ui \
    viewport.ui \
    keyframe.ui \
    defaults.ui \
    preferences.ui \
    materialattributes.ui \
    materialbrowser.ui \
    attributes.ui \
    cameraattributes.ui \
    importdialog.ui \
    mediapanel.ui \
    tabwidget.ui \
    jpegdialog.ui

CONFIG += c++14
CONFIG -= app_bundle

RESOURCES += \
    icons.qrc \
    ../QtChromosomeViz_v2/shaders.qrc \
    misc.qrc

exists( /usr/lib/x86_64-linux-gnu/libprotobuf.a ) {
    LIBS += /usr/lib/x86_64-linux-gnu/libprotobuf.a
}

exists( /usr/local/lib/libprotobuf.a ) {
    LIBS += /usr/local/lib/libprotobuf.a
}

INCLUDEPATH += /usr/local/include

PROTOS = ../QtChromosomeViz_v2/bartekm_code/message_format/message.proto
include(protobuf.pri)

DISTFILES += \
    ../QtChromosomeViz_v2/camera/fragment.glsl \
    ../QtChromosomeViz_v2/camera/geometry.glsl \
    ../QtChromosomeViz_v2/camera/vertex.glsl \
    ../QtChromosomeViz_v2/cylinder/fragment.glsl \
    ../QtChromosomeViz_v2/cylinder/geometry.glsl \
    ../QtChromosomeViz_v2/cylinder/vertex.glsl \
    ../QtChromosomeViz_v2/labels/fragment.glsl \
    ../QtChromosomeViz_v2/labels/geometry.glsl \
    ../QtChromosomeViz_v2/labels/vertex.glsl \
    ../QtChromosomeViz_v2/picking/fragment.glsl \
    ../QtChromosomeViz_v2/sphere/fragment.glsl \
    ../QtChromosomeViz_v2/sphere/geometry.glsl \
    ../QtChromosomeViz_v2/sphere/vertex.glsl

target.path = /usr/local/bin/
INSTALLS += target
