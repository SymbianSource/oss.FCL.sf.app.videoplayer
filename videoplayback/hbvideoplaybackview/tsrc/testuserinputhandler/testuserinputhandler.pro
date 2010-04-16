#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
# 
# Description: Project file for building testuserinputhandler
#
#
# Version : %version: 1 %

TEMPLATE = app
TARGET = testuserinputhandler
CONFIG += qtestlib hb qt
DEFINES += BUILD_VIDEOPLAYBACK_DLL

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += ../../../inc \
               ../../../../inc \
               ../../controlinc \
               

LIBS += -lremconcoreapi.dll \
        -lremconInterfacebase.dll 

                
# Input
HEADERS += stub/inc/mpxvideoviewwrapper.h \
           stub/inc/hbvideobaseplaybackview.h \
           stub/inc/mpxvideoplaybackviewfiledetails.h \
           stub/inc/hal.h \
           stub/inc/centralrepository.h \
           inc/testuserinputhandler.h \
           ../../viewinc/mpxvideoplaybackuserinputhandler.h
                                                
SOURCES += stub/src/mpxvideoviewwrapper.cpp \					 				 
           stub/src/hbvideobaseplaybackview.cpp \
           stub/src/mpxvideoplaybackviewfiledetails.cpp \
           stub/src/hal.cpp \
           stub/src/centralrepository.cpp \
		       src/testuserinputhandler.cpp \		       
		       ../../viewsrc/mpxvideoplaybackuserinputhandler.cpp
