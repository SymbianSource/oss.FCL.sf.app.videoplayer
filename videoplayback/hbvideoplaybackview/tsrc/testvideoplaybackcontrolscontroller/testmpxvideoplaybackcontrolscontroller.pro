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
# Description: Project file for building testmpxvideoplaybackcontrolscontroller
#
#
# Version : %version: 5 %


TEMPLATE = app
TARGET = testmpxvideoplaybackcontrolscontroller
CONFIG += qtestlib hb qt

LIBS += -lxqserviceutil.dll \
        -lefsrv.dll

INCLUDEPATH += stub/inc \
               ../inc \
               ../../inc \
               ../../../inc \            
               ../../../../inc \                                          

DEPENDPATH += stub/inc stub/src inc src  
        
# Input
HEADERS += hbvideobaseplaybackview.h  \
           mpxvideoplaybackcontrolbar.h \
           mpxvideoplaybackcontrolpolicy.h \
           mpxvideoplaybackcontrolconfiguration.h \
           mpxvideoplaybackdetailsplaybackwindow.h \
           mpxvideoplaybackdocumentloader.h \
           mpxvideoplaybackfiledetailswidget.h \
           mpxvideoplaybackfullscreencontrol.h \
           mpxvideoplaybackstatuspanecontrol.h \
           mpxvideoplaybackviewfiledetails.h \
           mpxvideoviewwrapper.h \
           thumbnailmanager_qt.h \
           testmpxvideoplaybackcontrolscontroller.h \
           hbvolumesliderpopup.h \
           xqserviceutilxtra.h \
           xqserviceutil.h \
           videoservices.h \
           videoserviceurifetch.h \
           ../../inc/mpxvideoplaybackcontrolscontroller.h 
					 
               
SOURCES += hbvideobaseplaybackview.cpp \ 
           mpxvideoplaybackcontrolbar.cpp \
           mpxvideoplaybackcontrolpolicy.cpp \
           mpxvideoplaybackcontrolconfiguration.cpp \
           mpxvideoplaybackdetailsplaybackwindow.cpp \
           mpxvideoplaybackdocumentloader.cpp \
           mpxvideoplaybackfiledetailswidget.cpp \
           mpxvideoplaybackfullscreencontrol.cpp \
           mpxvideoplaybackstatuspanecontrol.cpp \
           mpxvideoplaybackviewfiledetails.cpp \
           mpxvideoviewwrapper.cpp \
           thumbnailmanager_qt.cpp \
           testmpxvideoplaybackcontrolscontroller.cpp \
           hbvolumesliderpopup.cpp \
           xqserviceutilxtra.cpp \
           xqserviceutil.cpp \
           videoservices.cpp \
           videoserviceurifetch.cpp \
           ../../controlsrc/mpxvideoplaybackcontrolscontroller.cpp 
					 
           