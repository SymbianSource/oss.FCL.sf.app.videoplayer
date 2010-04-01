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
# Description: Project file for building testprogressbar
#
#
# Version : %version:  1 %

TEMPLATE = app
TARGET = testprogressbar
CONFIG += qtestlib qt hb

INCLUDEPATH += stub/inc \
               ../../../../inc \
               ../../../inc


DEPENDPATH += stub/inc stub/src inc src 
                
# Input
HEADERS += testprogressbar.h \
           mpxvideoplaybackcontrolscontroller.h \
           mpxvideoplaybackdocumentloader.h \
           mpxvideoplaybackviewfiledetails.h \
           hblabel.h \
           ../../controlinc/mpxvideoplaybackprogressbar.h
  
SOURCES += testprogressbar.cpp \
           mpxvideoplaybackcontrolscontroller.cpp \
           mpxvideoplaybackdocumentloader.cpp \
           mpxvideoplaybackviewfiledetails.cpp \
           hblabel.cpp \
           ../../controlsrc/mpxvideoplaybackprogressbar.cpp
