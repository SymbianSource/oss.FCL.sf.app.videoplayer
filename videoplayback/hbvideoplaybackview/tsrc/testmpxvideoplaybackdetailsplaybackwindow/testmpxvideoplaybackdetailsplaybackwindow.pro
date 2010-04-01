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
# Description: Project file for building testmpxvideoplaybackdetailsplaybackwindow
#
#
# Version : %version:  1 %

TEMPLATE = app
TARGET = testmpxvideoplaybackdetailsplaybackwindow
CONFIG += qtestlib qt hb

INCLUDEPATH += stub/inc \
               ../../../../inc \
               ../../../inc


DEPENDPATH += inc src stub/inc stub/src
                
# Input
HEADERS += testmpxvideoplaybackdetailsplaybackwindow.h \
           mpxvideoplaybackcontrolscontroller.h \
           mpxvideoplaybackdocumentloader.h \
           mpxvideoplaybackviewfiledetails.h \
           hbpushbutton.h \
           ../../controlinc/mpxvideoplaybackdetailsplaybackwindow.h
  
SOURCES += testmpxvideoplaybackdetailsplaybackwindow.cpp \
           mpxvideoplaybackcontrolscontroller.cpp \
           mpxvideoplaybackdocumentloader.cpp \
           mpxvideoplaybackviewfiledetails.cpp \
           hbpushbutton.cpp \
           ../../controlsrc/mpxvideoplaybackdetailsplaybackwindow.cpp
