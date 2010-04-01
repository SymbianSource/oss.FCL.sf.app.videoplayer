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


TEMPLATE = app
TARGET =
DEFINES += BUILD_VIDEOPLAYERAPP_DLL

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += . \
              stub/inc \
              ../../../inc \
              ../../../../inc \
              /epoc32/include \
              /epoc32/include/stdapis \
              /epoc32/include/stdapis/sys \


CONFIG += qtestlib hb qt

LIBS += -lestor.dll \
       -lfbscli.dll \
       -lbitgdi.dll \
       -lgdi.dll \
       -lxqservice.dll


# Input
HEADERS += stub/inc/videoplayerengine.h \
           inc/testvideoservices.h \
           ../../inc/videoservices.h \
           ../../inc/videoserviceurifetch.h \
           ../../inc/videoserviceplay.h \
           ../../../inc/videoplayerappexport.h

SOURCES += stub/src/videoplayerengine.cpp \
           #stub/src/xqserviceprovider.cpp \
           src/testvideoservices.cpp \
           ../../src/videoservices.cpp \
           ../../src/videoserviceurifetch.cpp \
           ../../src/videoserviceplay.cpp

