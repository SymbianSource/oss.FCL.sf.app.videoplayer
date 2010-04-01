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
# Description: Project file for building Videoplayer components
#
#
# Version : %version: 1 %

TEMPLATE = app
TARGET = testvideodisplayhandler
CONFIG += qtestlib hb qt
DEFINES += BUILD_VIDEOPLAYBACK_DLL

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += ../../../inc \
               ../../../../inc \
               ../../controlinc \
               $$MW_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxplaybackutility.dll \
        -lmpxcommon.dll \
        -lestor.dll \
        -lsysutil.dll        
                
# Input
HEADERS += stub/inc/mpxvideoviewwrapper.h \
           stub/inc/hbvideobaseplaybackview.h \
           stub/inc/mpxvideoplaybackviewfiledetails.h \
           stub/inc/mpxvideocontainer.h \
           stub/inc/alfcompositionutility.h \
           stub/inc/mediaclientvideodisplay.h \
           inc/testvideodisplayhandler.h \
           ../../viewinc/mpxvideoplaybackdisplayhandler.h
                                                
SOURCES += stub/src/mpxvideoviewwrapper.cpp \					 				 
           stub/src/hbvideobaseplaybackview.cpp \
           stub/src/mpxvideoplaybackviewfiledetails.cpp \
           stub/src/mpxvideocontainer.cpp \
           stub/src/alfcompositionutility.cpp \
           stub/src/mediaclientvideodisplay.cpp \
		       src/testvideodisplayhandler.cpp \
		       ../../viewsrc/mpxvideoplaybackdisplayhandler.cpp
