QT       += opengl svg charts

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
    tabwidget.cpp \
    jpegdialog.cpp \
    slider.cpp \
    targadialog.cpp \
    spline.cpp \
    simulation/simulationlayer.cpp \
    simulation/pdbsimulationlayer.cpp \
    simulation/simulation.cpp \
    opengl/vizwidget.cpp \
    simulation/iodevice.cpp \
    myproxystyle.cpp \
    listview.cpp \
    opengl/defaultlight.cpp \
    messagehandler.cpp \
    simulation/simulationseries.cpp \
    simulation/xvgsimulationseries.cpp

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
    tabwidget.h \
    jpegdialog.h \
    slider.h \
    targadialog.h \
    simulation/simulationlayer.h \
    simulation/pdbsimulationlayer.h \
    simulation/simulation.h \
    opengl/vizwidget.h \
    simulation/iodevice.h \
    myproxystyle.h \
    listview.h \
    opengl/defaultlight.h \
    messagehandler.h \
    simulation/simulationseries.h \
    simulation/xvgsimulationseries.h

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
    opengl/glow/fragment.glsl \
    opengl/glow/geometry.glsl \
    opengl/glow/vertex.glsl \
    opengl/index/fragment.glsl \
    opengl/label/fragment.glsl \
    opengl/label/geometry.glsl \
    opengl/label/vertex.glsl \
    opengl/select/fragment.glsl \
    opengl/select/geometry.glsl \
    opengl/sphere/fragment.glsl \
    opengl/sphere/geometry.glsl \
    opengl/sphere/vertex.glsl

LIBS += -lz

target.path = /usr/local/bin/
INSTALLS += target
