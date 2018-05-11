#-------------------------------------------------
#
#
# Copyright or © or Copr. Emmanuel Iarussi, David Bommes, Adrien Bousseau
# BendFields: Regularized Curvature Fields from Rough Concept Sketches (2015)

# emmanueliarussi (AT) gmail (DOT) com
# bommes (AT) aices (DOT) rwth-aachen (DOT) de
# adrien (DOT) bousseau (AT) inria (DOT) fr

# This software is a computer program whose purpose is to compute cross
# fields over sketches using the approach especified in BendFields paper.

# This software is governed by the CeCILL  license under French law and
# abiding by the rules of distribution of free software.  You can  use,
# modify and/ or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "http://www.cecill.info".

# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.

# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.

# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
#
#
#-------------------------------------------------

QT       += core
QT       += gui
QT       += xml
QT       += opengl

TARGET = BendFields_OSX
CONFIG   += console
CONFIG   -= app_bundle


TEMPLATE = app

# OpenCV includes:

INCLUDEPATH += /usr/include/opencv/ \
              ./src/ \
              /usr/local/Cellar/opencv@2/2.4.13.6/include/opencv \
              /usr/local/Cellar/opencv@2/2.4.13.6/include/

LIBS += -lopencv_core -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_video -L /usr/local/Cellar/opencv@2/2.4.13.6/lib


SOURCES += main.cpp \
    distancetransform.cpp \
    labeledmap.cpp \
    crossfield.cpp \
    tangentmap.cpp \
    vec2doperations.cpp \
    periodjumpfield.cpp \
    vec3doperations.cpp \
    mainwindow.cpp \
    glwidget.cpp \
    crossfieldgraphic.cpp \
    imageconverter.cpp \
    filterlineintegralconvolution.cpp \
    unknownsindexer.cpp \
    harmoniccrossfield.cpp \
    bendfield.cpp

HEADERS += \
    distancetransform.h \
    labeledmap.h \
    crossfield.h \
    tangentmap.h \
    vec2doperations.h \
    periodjumpfield.h \
    vec3doperations.h \
    mainwindow.h \
    glwidget.h \
    crossfieldgraphic.h \
    imageconverter.h \
    filterlineintegralconvolution.h \
    ui_mainwindow.h \
    unknownsindexer.h \
    harmoniccrossfield.h \
    bendfield.h

OTHER_FILES +=

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

# OpenCV includes for Mac
LIBS += -L/usr/local/lib \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_highgui \
    -lopencv_objdetect \
    -lopencv_calib3d

INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2
DEPENDPATH += /usr/local/include
DEPENDPATH += /usr/local/include/opencv/include
DEPENDPATH += /usr/local/include/opencv2/include

INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/lib























