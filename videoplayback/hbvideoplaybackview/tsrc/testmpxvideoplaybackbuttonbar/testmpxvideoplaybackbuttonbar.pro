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
# Description: Project file for building testmpxvideoplaybackbuttonbar
#
#
# Version : %version:  1 %

TEMPLATE = app
TARGET = testmpxvideoplaybackbuttonbar
CONFIG += qtestlib qt hb

INCLUDEPATH += stub/inc \
               ../../../../inc \
               ../../../inc


DEPENDPATH += inc src stub/inc stub/src
                
# Input
HEADERS += testmpxvideoplaybackbuttonbar.h \
           mpxvideoplaybackcontrolscontroller.h \
           mpxvideoplaybackdocumentloader.h \
           mpxvideoplaybackviewfiledetails.h \
           hbpushbutton.h \
           ../../controlinc/mpxvideoplaybackbuttonbar.h
  
SOURCES += testmpxvideoplaybackbuttonbar.cpp \
           mpxvideoplaybackcontrolscontroller.cpp \
           mpxvideoplaybackdocumentloader.cpp \
           mpxvideoplaybackviewfiledetails.cpp \
           hbpushbutton.cpp \
           ../../controlsrc/mpxvideoplaybackbuttonbar.cpp
