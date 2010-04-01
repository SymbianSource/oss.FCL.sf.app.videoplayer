#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Project file for building teststatuspanecontrol
#
#
# Version : %version: 2 %


TEMPLATE = app
TARGET = teststatuspanecontrol
CONFIG += qtestlib hb qt

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += stub/inc \
               ../inc \
               ../../inc \   
               ../../../inc \            
               ../../../../inc \                                          

DEPENDPATH += stub/inc stub/src inc src 
        
# Input
HEADERS += ../../../controlinc/mpxvideoplaybackstatuspanecontrol.h \       
           inc/teststatuspanecontrol.h \            
           mpxvideoplaybackcontrolscontroller.h \
           mpxvideoplaybackviewfiledetails.h \
           hbvideobaseplaybackview.h \
           mpxvideoplaybackdocumentloader.h \
           mpxvideoplaybackfullscreencontrol.h \
           hblabel.h

SOURCES += ../../../controlsrc/mpxvideoplaybackstatuspanecontrol.cpp \
           src/teststatuspanecontrol.cpp \				 
           mpxvideoplaybackcontrolscontroller.cpp \
           mpxvideoplaybackviewfiledetails.cpp \
           hbvideobaseplaybackview.cpp \
           mpxvideoplaybackdocumentloader.cpp \
           mpxvideoplaybackfullscreencontrol.cpp \
           hblabel.cpp
           