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
# Version : %version: 8 %


TEMPLATE = app
TARGET = testvideoplayerengine
DEFINES += BUILD_VIDEOPLAYERAPP_DLL

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += . \
              stub/inc \
              /epoc32/include \
              /epoc32/include/stdapis \
              /epoc32/include/stdapis/sys \
              ../../inc \
              ../../../../inc

CONFIG += qtestlib hb qt

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll \
    -lxqserviceutil.dll


# Input
HEADERS += stub/inc/mpxvideoplaybackwrapper.h \
           stub/inc/mpxviewpluginqt.h \
           stub/inc/testviewplugin.h \
           stub/inc/xqpluginloader.h \
           stub/inc/xqplugininfo.h \
           stub/inc/hbinstance.h \
           inc/testvideoplayerengine.h \
           stub/inc/videoservices.h \
           stub/inc/videoserviceurifetch.h \
           stub/inc/videoserviceplay.h \
           ../../../../inc/videoplayerengine.h

SOURCES += stub/src/mpxvideoplaybackwrapper.cpp \
           stub/src/testviewplugin.cpp \
           stub/src/xqpluginloader.cpp \
           stub/src/hbinstance.cpp \
           src/testvideoplayerengine.cpp \
           stub/src/videoservices.cpp \
           stub/src/videoserviceurifetch.cpp \
           stub/src/videoserviceplay.cpp \
           ../../src/videoplayerengine.cpp
