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
# Description: Project file for building testmpxvideoviewwrapper
#
#
# Version : %version: 5 %


TEMPLATE = app
TARGET = testmpxvideoviewwrapper
CONFIG += qtestlib hb qt

DEPENDPATH += inc src stub/src stub/inc

INCLUDEPATH += ../inc \
               ../../inc \   
               ../../../inc \            
               ../../../../inc \                                          
               ../../controlinc \ 
               ../../viewinc \ 
               $$MW_LAYER_SYSTEMINCLUDE                                            

LIBS += -lmpxcommon.dll \
        -lflogger.dll \
        -lmpxcollectionutility.dll \
        -lcone.dll \
        -lws32.dll

# Input
HEADERS += testmpxvideoviewwrapper.h \
           stub/inc/hbvideobaseplaybackview.h  \
           mpxvideoplaybackuserinputhandler.h \
           mpxvideoplaybackcontrolscontroller.h \           
           mpxvideoplaybackviewfiledetails.h \
           mpxplaybackutilityimpl.h \
           ../../viewinc/mpxvideoviewwrapper.h
					                
SOURCES += testmpxvideoviewwrapper.cpp \
            stub/src/hbvideobaseplaybackview.cpp \
            mpxvideoplaybackdisplayhandler.cpp \					 
            mpxvideoplaybackviewfiledetails.cpp \
            mpxvideoplaybackcontrolscontroller.cpp \
            mpxplaybackutility.cpp \
            mpxplaybackutilityimpl.cpp \
            mpxvideoplaybackuserinputhandler.cpp \
            ../../viewsrc/mpxvideoviewwrapper.cpp
