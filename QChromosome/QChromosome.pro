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
../QtChromosomeViz_v2/bartekm_code/PDBSimulation.cpp\
../QtChromosomeViz_v2/bartekm_code/Simulation.cpp \
    draggable.cpp \
    camera.cpp \
    plot.cpp \
    ../QtChromosomeViz_v2/bartekm_code/NullSimulation.cpp \
    ../QtChromosomeViz_v2/SelectionOperationsWidget.cpp \
    mediacontrol.cpp \
    mousecontrol.cpp

HEADERS  += mainwindow.h\
../QtChromosomeViz_v2/VizWidget.hpp\
../QtChromosomeViz_v2/bartekm_code/PDBSimulation.h\
../QtChromosomeViz_v2/bartekm_code/Simulation.h\
../QtChromosomeViz_v2/bartekm_code/common.h \
    draggable.h \
    camera.h \
    plot.h \
    ../QtChromosomeViz_v2/bartekm_code/NullSimulation.h \
    ../QtChromosomeViz_v2/SelectionOperationsWidget.hpp \
    mediacontrol.h \
    mousecontrol.h

FORMS    += mainwindow.ui \
    dockwidget.ui

CONFIG += c++11

RESOURCES += \
    icons.qrc \
    ../QtChromosomeViz_v2/shaders.qrc

OTHER_FILES +=
