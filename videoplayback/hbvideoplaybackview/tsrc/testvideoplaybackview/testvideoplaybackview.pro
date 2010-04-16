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
# Version : %version: 6 %

TEMPLATE = app
TARGET = testvideoplaybackview
CONFIG += qtestlib hb qt
DEFINES += BUILD_VIDEOPLAYBACK_DLL

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += ../../inc \
               ../../../inc \
               ../../../../inc \                          
               ../../controlinc \
               $$MW_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxplaybackutility.dll \
        -lmpxcommon.dll \
        -lmpxviewplugin.dll \
        -lcone.dll \
        -lws32.dll \
        -lestor.dll \
        -leuser.dll \
        -lcommonengine.dll \
        -lavkon.dll \
        -lefsrv.dll \
        -lflogger.dll \
        -lsysutil.dll \
        -lmpxcollectionutility.dll \
        -lremconcoreapi.dll \
        -lremconInterfacebase.dll \
        -lhal.dll \
        -lcentralrepository.dll
        
        
# Input
HEADERS += stub/inc/mpxvideoviewwrapper.h \					 
           inc/testvideoplaybackview.h \
           ../../../inc/hbvideobaseplaybackview.h \
           ../../../inc/hbvideoplaybackview.h \
           ../../inc/mpxvideoplaybackviewfiledetails.h
           
           
           
               
SOURCES += stub/src/mpxvideoviewwrapper.cpp \					 				 
					 src/testvideoplaybackview.cpp \
					 ../../viewsrc/hbvideobaseplaybackview.cpp \
					 ../../viewsrc/hbvideoplaybackview.cpp \
			     ../../viewsrc/mpxvideoplaybackviewfiledetails.cpp