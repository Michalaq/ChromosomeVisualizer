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
        display.cpp\
../QtChromosomeViz_v2/VizWidget.cpp\
../QtChromosomeViz_v2/bartekm_code/PDBSimulation.cpp\
../QtChromosomeViz_v2/bartekm_code/Simulation.cpp \
    draggable.cpp \
    canvas.cpp \
    control.cpp \
    camera.cpp

HEADERS  += mainwindow.h\
        display.h\
../QtChromosomeViz_v2/VizWidget.hpp\
../QtChromosomeViz_v2/bartekm_code/PDBSimulation.h\
../QtChromosomeViz_v2/bartekm_code/Simulation.h\
../QtChromosomeViz_v2/bartekm_code/common.h \
    draggable.h \
    canvas.h \
    control.h \
    camera.h

FORMS    += mainwindow.ui\
        display.ui

CONFIG += c++11

RESOURCES += \
    icons.qrc
