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
    
INCLUDEPATH += . \
    inc \
    ../../../tsrc/stubs/inc \
    \ # keep these at bottom so that stubbed headers are searched first
    ../../../inc \
    ../../../videocollectionview/inc \
    ../../../videocollectionwrapper/inc \
    
CONFIG += qtestlib \
    Hb \
    symbian_test

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll \
    -lvideocollectionwrapper.dll

HEADERS +=  inc/testhintwidget.h \
    \ # headers needed in test
    ../../../videocollectionview/inc/videohintwidget.h \
    \ # headers needed in stubs
    ../../../videocollectionview/inc/videolistview.h \
    ../../../videocollectionview/inc/videolistwidget.h \
    ../../../videocollectionview/inc/videocollectionuiloader.h \
    ../../../videocollectionview/inc/videocollectionviewutils.h \
    ../../../videocollectionview/inc/videolistselectiondialog.h
               
SOURCES +=  src/testhintwidget.cpp \
    \ # sources needed in test
    ../../../videocollectionview/src/videohintwidget.cpp \
    \ # sources needed in stubs
    ../../../tsrc/stubs/src/hblistview.cpp \
    ../../../tsrc/stubs/src/hbwidget.cpp \
    ../../../tsrc/stubs/src/hbscrollbar.cpp \
    ../../../tsrc/stubs/src/hbdialog.cpp \
    ../../../tsrc/stubs/src/hbaction.cpp \
    ../../../tsrc/stubs/src/videocollectionuiloader.cpp \
    ../../../tsrc/stubs/src/videocollectionviewutils.cpp \
    ../../../tsrc/stubs/src/videolistview.cpp \
    ../../../tsrc/stubs/src/videolistwidget.cpp \
    ../../../tsrc/stubs/src/videolistselectiondialog.cpp

RESOURCES += ../../data/videocollectionview.qrc
