#-------------------------------------------------
#
# Project created by QtCreator 2015-12-20T12:51:34
#
#-------------------------------------------------

QT       += core gui opengl svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QChromosome
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
../QtChromosomeViz_v2/VizWidget.cpp\
../QtChromosomeViz_v2/bartekm_code/Simulation.cpp \
    draggable.cpp \
    camera.cpp \
    plot.cpp \
    mediacontrol.cpp \
    dockwidget.cpp \
    spinbox.cpp \
    ../QtChromosomeViz_v2/LabelRenderer.cpp \
    rangeslider.cpp \
    axis.cpp \
    ../QtChromosomeViz_v2/SelectionRectWidget.cpp \
    legend.cpp \
    toolbar.cpp \
    rendersettings.cpp \
    blind.cpp \
    projectsettings.cpp \
    slider.cpp \
    softslider.cpp \
    ../QtChromosomeViz_v2/bartekm_code/SimulationLayer.cpp \
    ../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.cpp \
    ../QtChromosomeViz_v2/bartekm_code/ProtobufSimulationlayer.cpp \
    ../QtChromosomeViz_v2/bartekm_code/message_format/message.pb.cc \
    ../QtChromosomeViz_v2/bartekm_code/NullSimulationLayer.cpp \
    treeitem.cpp \
    treemodel.cpp \
    ../QtChromosomeViz_v2/bartekm_code/SimulationLayerConcatenation.cpp \
    attributes.cpp \
    picker.cpp \
    layerattributes.cpp \
    viewport.cpp \
    multimap.cpp \
    camerasettings.cpp \
    doublespinbox.cpp \
    combobox.cpp \
    lineedit.cpp

HEADERS  += mainwindow.h\
../QtChromosomeViz_v2/VizWidget.hpp\
../QtChromosomeViz_v2/bartekm_code/Simulation.h\
../QtChromosomeViz_v2/bartekm_code/common.h \
    draggable.h \
    camera.h \
    plot.h \
    mediacontrol.h \
    dockwidget.h \
    spinbox.h \
    ../QtChromosomeViz_v2/LabelRenderer.hpp \
    rangeslider.h \
    axis.h \
    ../QtChromosomeViz_v2/SelectionRectWidget.hpp \
    legend.h \
    toolbar.h \
    rendersettings.h \
    moviemaker.h \
    blind.h \
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
    ../QtChromosomeViz_v2/bartekm_code/message_format/message.pb.h \
    ../QtChromosomeViz_v2/bartekm_code/NullSimulationLayer.h \
    treeitem.h \
    treemodel.h \
    ../QtChromosomeViz_v2/bartekm_code/SimulationLayerConcatenation.h \
    attributes.h \
    picker.h \
    ../QtChromosomeViz_v2/bartekm_code/TimeTransformation.h \
    layerattributes.h \
    viewport.h \
    multimap.h \
    camerasettings.h \
    doublespinbox.h \
    combobox.h \
    lineedit.h

FORMS    += mainwindow.ui \
    dockwidget.ui \
    projectsettings.ui \
    rendersettings.ui \
    attributes.ui \
    layerattributes.ui \
    viewport.ui \
    rswidget.ui \
    camerasettings.ui

CONFIG += c++14

RESOURCES += \
    icons.qrc \
    ../QtChromosomeViz_v2/shaders.qrc \
    misc.qrc

OTHER_FILES +=

unix|win32: LIBS += -lprotobuf
