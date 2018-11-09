QT       += core gui opengl svg charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qcstudio
TEMPLATE = app


SOURCES += \
    main.cpp \
    mainwindow.cpp \
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
    timeline.cpp \
    softslider.cpp \
    treeitem.cpp \
    treemodel.cpp \
    atomattributes.cpp \
    picker.cpp \
    layerattributes.cpp \
    viewport.cpp \
    doublespinbox.cpp \
    combobox.cpp \
    lineedit.cpp \
    fileedit.cpp \
    label.cpp \
    keyframe.cpp \
    interpolator.cpp \
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
    jpegdialog.cpp \
    slider.cpp \
    targadialog.cpp \
    spline.cpp \
    simulation/simulationlayer.cpp \
    simulation/pdbsimulationlayer.cpp \
    simulation/simulation.cpp \
    opengl/vizwidget.cpp

HEADERS += \
    mainwindow.h \
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
    timeline.h \
    softslider.h \
    treeitem.h \
    treemodel.h \
    atomattributes.h \
    picker.h \
    layerattributes.h \
    viewport.h \
    doublespinbox.h \
    combobox.h \
    lineedit.h \
    fileedit.h \
    label.h \
    spline.h \
    keyframe.h \
    interpolator.h \
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
    jpegdialog.h \
    slider.h \
    targadialog.h \
    simulation/simulationlayer.h \
    simulation/pdbsimulationlayer.h \
    simulation/simulation.h \
    opengl/vizwidget.h

FORMS += mainwindow.ui \
    dockwidget.ui \
    projectsettings.ui \
    rendersettings.ui \
    atomattributes.ui \
    layerattributes.ui \
    viewport.ui \
    keyframe.ui \
    preferences.ui \
    materialattributes.ui \
    materialbrowser.ui \
    attributes.ui \
    cameraattributes.ui \
    importdialog.ui \
    mediapanel.ui \
    tabwidget.ui \
    jpegdialog.ui \
    targadialog.ui

CONFIG += c++14
CONFIG -= app_bundle

RESOURCES += \
    icons.qrc \
    opengl/shaders.qrc \
    misc.qrc

DISTFILES += \
    opengl/camera/fragment.glsl \
    opengl/camera/geometry.glsl \
    opengl/camera/vertex.glsl \
    opengl/cylinder/fragment.glsl \
    opengl/cylinder/geometry.glsl \
    opengl/cylinder/vertex.glsl \
    opengl/labels/fragment.glsl \
    opengl/labels/geometry.glsl \
    opengl/labels/vertex.glsl \
    opengl/picking/fragment.glsl \
    opengl/sphere/fragment.glsl \
    opengl/sphere/geometry.glsl \
    opengl/sphere/vertex.glsl

target.path = /usr/local/bin/
INSTALLS += target
