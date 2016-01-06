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
    control.cpp \
    camera.cpp \
    plot.cpp \
    ../QtChromosomeViz_v2/bartekm_code/NullSimulation.cpp \
    ../QtChromosomeViz_v2/SelectionOperationsWidget.cpp

HEADERS  += mainwindow.h\
../QtChromosomeViz_v2/VizWidget.hpp\
../QtChromosomeViz_v2/bartekm_code/PDBSimulation.h\
../QtChromosomeViz_v2/bartekm_code/Simulation.h\
../QtChromosomeViz_v2/bartekm_code/common.h \
    draggable.h \
    control.h \
    camera.h \
    plot.h \
    ../QtChromosomeViz_v2/bartekm_code/NullSimulation.h \
    ../QtChromosomeViz_v2/SelectionOperationsWidget.hpp

FORMS    += mainwindow.ui

CONFIG += c++11

RESOURCES += \
    icons.qrc
