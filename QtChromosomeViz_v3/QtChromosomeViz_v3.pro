QT          += widgets opengl svg

HEADERS     = VizWidget.hpp \
    mainwidget.h \
    control.h \
    bartekm_code/common.h \
    bartekm_code/PDBSimulation.h \
    bartekm_code/Simulation.h

SOURCES     = VizWidget.cpp \
              main.cpp \
    mainwidget.cpp \
    control.cpp \
    bartekm_code/PDBSimulation.cpp \
    bartekm_code/Simulation.cpp

CONFIG      += c++11

FORMS += \
    form.ui
