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
# Description: Project file for building testfullscreencontrol
#
#
# Version : %version: 2 %


TEMPLATE = app
TARGET = testfullscreencontrol
CONFIG += qtestlib hb qt

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += stub/inc \
               ../inc \
               ../../inc \   
               ../../../inc \            
               ../../../../inc \  
               /epoc32/include/platform/mw \                                        

DEPENDPATH += stub/inc stub/src inc src 
        
# Input
HEADERS += ../../../controlinc/mpxvideoplaybackfullscreencontrol.h \      
           inc/testfullscreencontrol.h \            
           mpxvideoplaybackcontrolscontroller.h \
           mpxvideoplaybackviewfiledetails.h \
           mpxvideoplaybackcontrolbar.h \
           mpxvideoplaybackfiledetailswidget.h \
           mpxvideoplaybackdetailsplaybackwindow.h     
					 
               
SOURCES += ../../../controlsrc/mpxvideoplaybackfullscreencontrol.cpp \
           src/testfullscreencontrol.cpp \
           mpxvideoplaybackcontrolscontroller.cpp \
           mpxvideoplaybackviewfiledetails.cpp \
           mpxvideoplaybackcontrolbar.cpp \
           mpxvideoplaybackfiledetailswidget.cpp \
           mpxvideoplaybackdetailsplaybackwindow.cpp                    
           				 
           