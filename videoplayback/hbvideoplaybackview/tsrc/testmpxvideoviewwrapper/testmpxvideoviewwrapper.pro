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
TARGET = testmpxvideoviewwrapper
CONFIG += qtestlib hb qt

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += ../inc \
							 ../../inc \   
               ../../../inc \            
               ../../../../inc \                                          
               ../../controlinc \ 
               ../../viewinc \ 
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
HEADERS += stub/inc/mpxvideoplaybackuserinputhandler.h \
           stub/inc/hbvideobaseplaybackview.h  \
           inc/testmpxvideoviewwrapper.h \
           ../../viewinc/mpxvideoviewwrapper.h \
           stub/inc/mpxvideoplaybackcontrolscontroller.h \           
           stub/inc/mpxvideoplaybackviewfiledetails.h \
           stub/inc/mpxplaybackutilityimpl.h
					 
               
SOURCES += stub/src/hbvideobaseplaybackview.cpp \ 
					 src/testmpxvideoviewwrapper.cpp \
					 ../../viewsrc/mpxvideoviewwrapper.cpp \
					 stub/src/mpxvideoplaybackdisplayhandler.cpp \					 
					 stub/src/mpxvideoplaybackviewfiledetails.cpp \
					 stub/src/mpxvideoplaybackcontrolscontroller.cpp \
					 stub/src/mpxplaybackutility.cpp \
					 stub/src/mpxplaybackutilityimpl.cpp \
					 stub/src/mpxvideoplaybackuserinputhandler.cpp 
					 
           