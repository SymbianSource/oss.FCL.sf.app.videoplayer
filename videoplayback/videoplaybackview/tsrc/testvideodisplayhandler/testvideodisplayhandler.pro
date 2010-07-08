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
# Version : %version: 4 %

TEMPLATE = app
TARGET = testvideodisplayhandler
CONFIG += qtestlib hb qt
DEFINES += BUILD_VIDEOPLAYBACK_DLL

DEPENDPATH += inc src stub/inc stub/src

INCLUDEPATH += ../../../inc \
               ../../../../inc \
               ../../controlinc \
               $$MW_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxplaybackutility.dll \
        -lmpxcommon.dll \
        -lestor.dll \
        -lsysutil.dll \
        -lcone.dll \
        -lefsrv.dll \
        -lws32.dll \
        -lgdi.dll
                
# Input
HEADERS += mpxvideoviewwrapper.h \
           videobaseplaybackview.h \
           videoplaybackviewfiledetails.h \
           mpxvideocontainer.h \
           alfcompositionutility.h \
           mediaclientvideodisplay.h \
           testvideodisplayhandler.h \
           ../../viewinc/mpxvideoplaybackdisplayhandler.h
                                                
SOURCES += mpxvideoviewwrapper.cpp \					 				
           videobaseplaybackview.cpp \
           videoplaybackviewfiledetails.cpp \
           mpxvideocontainer.cpp \
           alfcompositionutility.cpp \
           mediaclientvideodisplay.cpp \
           testvideodisplayhandler.cpp \
           ../../viewsrc/mpxvideoplaybackdisplayhandler.cpp
