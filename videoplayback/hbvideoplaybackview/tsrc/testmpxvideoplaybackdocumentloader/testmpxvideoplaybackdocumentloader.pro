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
# Description: Project file for building testmpxvideoplaybackdocumentloader
#
#
# Version : %version:  1 %

TEMPLATE = app
TARGET = testmpxvideoplaybackdocumentloader
CONFIG += qtestlib qt hb

INCLUDEPATH += stub/inc \
               ../../../../inc \
               ../../../inc


DEPENDPATH += inc src stub/inc stub/src
                
# Input
HEADERS += testmpxvideoplaybackdocumentloader.h \
           mpxvideoplaybackcontrolscontroller.h \
           mpxvideoplaybackfiledetailswidget.h \
           mpxvideoplaybackdetailsplaybackwindow.h \
           mpxvideoplaybackprogressbar.h \
           mpxvideoplaybackbuttonbar.h \
           mpxvideoplaybacknontouchvolumebar.h \
           ../../controlinc/mpxvideoplaybackdocumentloader.h
  
SOURCES += testmpxvideoplaybackdocumentloader.cpp \
           mpxvideoplaybackcontrolscontroller.cpp \
           mpxvideoplaybackfiledetailswidget.cpp \
           mpxvideoplaybackdetailsplaybackwindow.cpp \
           mpxvideoplaybackprogressbar.cpp \
           mpxvideoplaybackbuttonbar.cpp \
           mpxvideoplaybacknontouchvolumebar.cpp \
           ../../controlsrc/mpxvideoplaybackdocumentloader.cpp
