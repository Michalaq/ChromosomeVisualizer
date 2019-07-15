QT       += opengl svg charts

TARGET = qcstudio
TEMPLATE = app
INCLUDEPATH += ./src/

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/draggable.cpp \
    src/camera.cpp \
    src/plot.cpp \
    src/mediacontrol.cpp \
    src/dockwidget.cpp \
    src/seriesattributes.cpp \
    src/spinbox.cpp \
    src/rangeslider.cpp \
    src/legend.cpp \
    src/toolbar.cpp \
    src/rendersettings.cpp \
    src/projectsettings.cpp \
    src/timeline.cpp \
    src/softslider.cpp \
    src/treeitem.cpp \
    src/treemodel.cpp \
    src/atomattributes.cpp \
    src/picker.cpp \
    src/layerattributes.cpp \
    src/viewport.cpp \
    src/doublespinbox.cpp \
    src/combobox.cpp \
    src/lineedit.cpp \
    src/fileedit.cpp \
    src/label.cpp \
    src/keyframe.cpp \
    src/interpolator.cpp \
    src/selection.cpp \
    src/visibilitydelegate.cpp \
    src/treeview.cpp \
    src/textedit.cpp \
    src/preferences.cpp \
    src/namedelegate.cpp \
    src/material.cpp \
    src/tagsdelegate.cpp \
    src/flowlayout.cpp \
    src/materialattributes.cpp \
    src/materialbrowser.cpp \
    src/moviemaker.cpp \
    src/attributes.cpp \
    src/titlelabel.cpp \
    src/pickwidget.cpp \
    src/cameraattributes.cpp \
    src/importdialog.cpp \
    src/labelatlas.cpp \
    src/statusbar.cpp \
    src/fontcombobox.cpp \
    src/session.cpp \
    src/mediapanel.cpp \
    src/tabwidget.cpp \
    src/jpegdialog.cpp \
    src/slider.cpp \
    src/targadialog.cpp \
    src/spline.cpp \
    src/simulation/simulationlayer.cpp \
    src/simulation/pdbsimulationlayer.cpp \
    src/simulation/simulation.cpp \
    src/opengl/vizwidget.cpp \
    src/simulation/iodevice.cpp \
    src/myproxystyle.cpp \
    src/listview.cpp \
    src/opengl/defaultlight.cpp \
    src/messagehandler.cpp \
    src/simulation/simulationseries.cpp \
    src/simulation/xvgsimulationseries.cpp \
    src/simulation/simulationitem.cpp

HEADERS += \
    src/mainwindow.h \
    src/draggable.h \
    src/camera.h \
    src/plot.h \
    src/mediacontrol.h \
    src/dockwidget.h \
    src/seriesattributes.h \
    src/spinbox.h \
    src/rangeslider.h \
    src/legend.h \
    src/toolbar.h \
    src/rendersettings.h \
    src/moviemaker.h \
    src/projectsettings.h \
    src/timeline.h \
    src/softslider.h \
    src/treeitem.h \
    src/treemodel.h \
    src/atomattributes.h \
    src/picker.h \
    src/layerattributes.h \
    src/viewport.h \
    src/doublespinbox.h \
    src/combobox.h \
    src/lineedit.h \
    src/fileedit.h \
    src/label.h \
    src/spline.h \
    src/keyframe.h \
    src/interpolator.h \
    src/selection.h \
    src/visibilitydelegate.h \
    src/treeview.h \
    src/textedit.h \
    src/preferences.h \
    src/namedelegate.h \
    src/material.h \
    src/tagsdelegate.h \
    src/flowlayout.h \
    src/materialattributes.h \
    src/materialbrowser.h \
    src/attributes.h \
    src/titlelabel.h \
    src/pickwidget.h \
    src/cameraattributes.h \
    src/importdialog.h \
    src/labelatlas.h \
    src/statusbar.h \
    src/fontcombobox.h \
    src/session.h \
    src/glbuffer.h \
    src/mediapanel.h \
    src/tabwidget.h \
    src/jpegdialog.h \
    src/slider.h \
    src/targadialog.h \
    src/simulation/simulationlayer.h \
    src/simulation/pdbsimulationlayer.h \
    src/simulation/simulation.h \
    src/opengl/vizwidget.h \
    src/simulation/iodevice.h \
    src/myproxystyle.h \
    src/listview.h \
    src/opengl/defaultlight.h \
    src/messagehandler.h \
    src/simulation/simulationseries.h \
    src/simulation/xvgsimulationseries.h \
    src/simulation/simulationitem.h

FORMS += \
    src/mainwindow.ui \
    src/dockwidget.ui \
    src/projectsettings.ui \
    src/rendersettings.ui \
    src/atomattributes.ui \
    src/layerattributes.ui \
    src/seriesattributes.ui \
    src/viewport.ui \
    src/keyframe.ui \
    src/preferences.ui \
    src/materialattributes.ui \
    src/materialbrowser.ui \
    src/attributes.ui \
    src/cameraattributes.ui \
    src/importdialog.ui \
    src/mediapanel.ui \
    src/tabwidget.ui \
    src/jpegdialog.ui \
    src/targadialog.ui

CONFIG -= app_bundle

RESOURCES += \
    src/icons.qrc \
    src/opengl/shaders.qrc \
    src/misc.qrc

DISTFILES += \
    src/opengl/camera/fragment.glsl \
    src/opengl/camera/geometry.glsl \
    src/opengl/camera/vertex.glsl \
    src/opengl/cylinder/fragment.glsl \
    src/opengl/cylinder/geometry.glsl \
    src/opengl/cylinder/vertex.glsl \
    src/opengl/glow/fragment.glsl \
    src/opengl/glow/geometry.glsl \
    src/opengl/glow/vertex.glsl \
    src/opengl/index/fragment.glsl \
    src/opengl/label/fragment.glsl \
    src/opengl/label/geometry.glsl \
    src/opengl/label/vertex.glsl \
    src/opengl/select/fragment.glsl \
    src/opengl/select/geometry.glsl \
    src/opengl/sphere/fragment.glsl \
    src/opengl/sphere/geometry.glsl \
    src/opengl/sphere/vertex.glsl

LIBS += -lz

unix: target.path = "/usr/local/bin/"
win32: target.path = "C:/Program Files/Regulomics/QChromosome 4D Studio/"

INSTALLS += target
