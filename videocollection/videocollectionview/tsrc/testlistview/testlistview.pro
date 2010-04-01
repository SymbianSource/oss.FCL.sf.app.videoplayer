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
    ../../../videocollectionwrapper/inc \
    ../../../../videoplayerapp/videoplayerengine/inc
    
CONFIG += qtestlib \
    Hb \
    symbian_test
    
LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll \
    -lxqplugins.dll \
    -lmpxviewframeworkqt.dll \
    -lxqservice.dll \
    -lxqserviceutil.dll

HEADERS += inc/testlistview.h \
    \ # headers needed in test
    ../../../videocollectionview/inc/videolistview.h \
    \ # headers needed in stubs
    ../../../tsrc/stubs/inc/hbmenu.h \
    ../../../tsrc/stubs/inc/hbmessagebox.h \
    ../../../tsrc/stubs/inc/xqserviceutil.h \
    ../../../tsrc/stubs/inc/xqserviceutilxtra.h \
    ../../../videocollectionview/inc/videocollectionuiloader.h \
    ../../../videocollectionview/inc/videolistselectiondialog.h \
    ../../../videocollectionview/inc/videocollectionviewutils.h \
    ../../../videocollectionview/inc/videohintwidget.h \
    ../../../videocollectionview/inc/videolistwidget.h \
    ../../../videocollectionwrapper/inc/videocollectionwrapper.h \
    ../../../videocollectionwrapper/inc/videosortfilterproxymodel.h \
    ../../../videocollectionwrapper/inc/videolistdatamodel.h \
    ../../../../videoplayerapp/videoplayerengine/inc/videoservices.h \
    ../../../../videoplayerapp/videoplayerengine/inc/videoserviceurifetch.h

SOURCES += src/testlistview.cpp \
    \ # sources needed in test
    ../../../videocollectionview/src/videolistview.cpp \
    \ # sources needed in stubs
    ../../../tsrc/stubs/src/hbmessagebox.cpp \
    ../../../tsrc/stubs/src/hbmenu.cpp \
    ../../../tsrc/stubs/src/hbwidget.cpp \
    ../../../tsrc/stubs/src/hbaction.cpp \
    ../../../tsrc/stubs/src/hbdialog.cpp \
    ../../../tsrc/stubs/src/hblistview.cpp \
    ../../../tsrc/stubs/src/hbscrollbar.cpp \
    ../../../tsrc/stubs/src/xqserviceutilxtra.cpp \
    ../../../tsrc/stubs/src/videocollectionuiloader.cpp \
    ../../../tsrc/stubs/src/videolistselectiondialog.cpp \
    ../../../tsrc/stubs/src/videocollectionviewutils.cpp \
    ../../../tsrc/stubs/src/videohintwidget.cpp \
    ../../../tsrc/stubs/src/videolistwidget.cpp \
    ../../../tsrc/stubs/src/videocollectionwrapper.cpp \
    ../../../tsrc/stubs/src/videosortfilterproxymodel.cpp \
    ../../../tsrc/stubs/src/videolistdatamodel.cpp \
    ../../../tsrc/stubs/src/videoservices.cpp \
    ../../../tsrc/stubs/src/videoserviceurifetch.cpp

RESOURCES += ../../data/videocollectionview.qrc           

