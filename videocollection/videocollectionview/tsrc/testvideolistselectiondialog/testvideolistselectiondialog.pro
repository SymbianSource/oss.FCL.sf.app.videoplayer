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
# Description: 
#

TEMPLATE = app

TARGET = 

DEPENDPATH += . \
    inc \
    src
    
INCLUDEPATH = . \ 
    inc \
    ../../../tsrc/stubs/inc \
    \ # keep these at bottom so that stub headers are taken first
    ../../../inc \
    ../../../videocollectionview/inc \
    ../../../videocollectionwrapper/inc
    
CONFIG += qtestlib \
    hb \
    symbian_test

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll

HEADERS += inc/testvideolistselectiondialog.h \
    \ # headers needed in test
    ../../inc/videolistselectiondialog.h \
    \ # headers needed in stubs
    ../../../tsrc/stubs/inc/hbdialog.h \            
    ../../../tsrc/stubs/inc/hblabel.h \
    ../../../tsrc/stubs/inc/hbcheckbox.h \
    ../../../tsrc/stubs/inc/hbwidget.h \
    ../../../tsrc/stubs/inc/hbstackedwidget.h \
    ../../../tsrc/stubs/inc/hbdeviceprofile.h \
    ../../../tsrc/stubs/inc/hbabstractitemview.h \
    ../../../tsrc/stubs/inc/hbaction.h \
    ../../../tsrc/stubs/inc/hbview.h \
    ../../../videocollectionview/inc/videolistwidget.h \
    ../../../videocollectionview/inc/videocollectionuiloader.h \
    ../../../videocollectionwrapper/inc/videocollectionwrapper.h \
    ../../../videocollectionwrapper/inc/videolistdatamodel.h \        
    ../../../videocollectionwrapper/inc/videosortfilterproxymodel.h            
    
SOURCES += src/testvideolistselectiondialog.cpp \           
    \ # sources needed in test
    ../../src/videolistselectiondialog.cpp \
    \ # sources needed in stubs
    ../../../tsrc/stubs/src/hbdialog.cpp \
    ../../../tsrc/stubs/src/hblabel.cpp \
    ../../../tsrc/stubs/src/hbcheckbox.cpp \
    ../../../tsrc/stubs/src/hbwidget.cpp \
    ../../../tsrc/stubs/src/hbstackedwidget.cpp \
    ../../../tsrc/stubs/src/hbaction.cpp \
    ../../../tsrc/stubs/src/videolistwidget.cpp \
    ../../../tsrc/stubs/src/videocollectionuiloader.cpp \
    ../../../tsrc/stubs/src/videosortfilterproxymodel.cpp \  
    ../../../tsrc/stubs/src/videolistdatamodel.cpp
