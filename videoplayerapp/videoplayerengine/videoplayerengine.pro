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
# Version : %version: da1mmcf#24 %


TEMPLATE = lib
TARGET = videoplayerengine
CONFIG += hb qt dll
DEFINES += BUILD_VIDEOPLAYERAPP_DLL

symbian: 
{ 
    TARGET.CAPABILITY = ALL -DRM -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20024337
    TARGET.EPOCHEAPSIZE = 0x20000 0x1600000
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE \
                   $$APP_LAYER_SYSTEMINCLUDE SYSTEMINCLUDE
    BLD_INF_RULES.prj_exports += "rom/videoplayerengine.iby CORE_APP_LAYER_IBY_EXPORT_PATH(videoplayerengine.iby)"
}

INCLUDEPATH += ../../inc \
               ../inc

LIBS += -lmpxviewframeworkqt.dll \
        -lxqplugins.dll \
        -lmpxplaybackutility.dll \
        -lmpxcollectionutility.dll \
        -lmpxcollectionhelper.dll \    
        -lmpxcommon.dll \
        -lplaybackhelper.dll \
        -lvideoplaylistutility.dll \
        -lservicehandler.dll \
        -lxqservice.dll \
        -lxqserviceutil.dll \
        -lflogger.dll \
        -lefsrv.dll

DEPENDPATH += ../../inc ../inc inc
VPATH += src

HEADERS += videoplayerengine.h \
          mpxvideoplaybackwrapper.h \
          mpxvideoplayerappuiengine.h \
          videoservices.h \
          videoserviceplay.h \
          videoserviceurifetch.h \
          videoserviceview.h \
          videoservicebrowse.h

SOURCES += videoplayerengine.cpp \
           mpxvideoplaybackwrapper.cpp \
           mpxvideoplayerappuiengine.cpp \
           videoservices.cpp \
           videoserviceplay.cpp \
           videoserviceurifetch.cpp \
           videoserviceview.cpp \
           videoservicebrowse.cpp

