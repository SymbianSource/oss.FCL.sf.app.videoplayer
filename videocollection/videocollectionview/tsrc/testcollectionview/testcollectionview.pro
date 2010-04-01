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
    \ # keep these at bottom so that stubbed headers are taken first
    ../../../inc \
    ../../../videocollectionview/inc \
    ../../../videocollectionwrapper/inc
    
CONFIG += qtestlib \
    Hb \
    symbian_test
          
LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll \
    -lxqplugins.dll \
    -lmpxviewframeworkqt.dll \
    -lvideocollectionwrapper.dll

HEADERS +=  inc/testcollectionview.h \
    \ # headers needed in test
    ../../inc/videocollectionviewplugin.h \
    \ # headers needed in stubs
    ../../../videocollectionview/inc/videocollectionuiloader.h \
    ../../../videocollectionview/inc/videocollectionviewutils.h \
    ../../../videocollectionview/inc/videolistview.h \
    ../../../videocollectionview/inc/videolistwidget.h \
    ../../../videocollectionview/inc/videolistselectiondialog.h \
    ../../../videocollectionview/inc/videohintwidget.h
           
SOURCES +=  src/testcollectionview.cpp \
    \ # sources needed in test
    ../../src/videocollectionviewplugin.cpp \
    \ # sources needed in stubs
    ../../../tsrc/stubs/src/hbmainwindow.cpp \
    ../../../tsrc/stubs/src/hblistview.cpp \
    ../../../tsrc/stubs/src/hbwidget.cpp \
    ../../../tsrc/stubs/src/hbscrollbar.cpp \
    ../../../tsrc/stubs/src/hbdialog.cpp \
    ../../../tsrc/stubs/src/hbaction.cpp \
    ../../../tsrc/stubs/src/videocollectionuiloader.cpp \
    ../../../tsrc/stubs/src/videocollectionviewutils.cpp \
    ../../../tsrc/stubs/src/videolistview.cpp \
    ../../../tsrc/stubs/src/videolistwidget.cpp \
    ../../../tsrc/stubs/src/videolistselectiondialog.cpp \
    ../../../tsrc/stubs/src/videohintwidget.cpp

RESOURCES += ../../data/videocollectionview.qrc
