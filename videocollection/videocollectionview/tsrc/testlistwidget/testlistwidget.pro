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
    \ # keep these at bottom so that stub headers are taken first
    ../../../inc \
    ../../../videocollectionview/inc \
    ../../../videocollectionwrapper/inc \
    ../../../../inc \
    ../../../../videoplayerapp/videoplayerengine/inc
    
CONFIG += qtestlib \
    Hb \
    symbian_test

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll \
    -lvideocollectionwrapper.dll \
    -lxqservice.dll \
    -lxqserviceutil.dll

HEADERS += inc/testlistwidget.h \
    \ # headers needed in test
    ../../../videocollectionview/inc/videolistwidget.h \
    \ # headers needed in stubs
    ../../../tsrc/stubs/inc/hbmainwindow.h \
    ../../../tsrc/stubs/inc/hbwidget.h \
    ../../../tsrc/stubs/inc/hbinstance.h \
    ../../../tsrc/stubs/inc/hbscrollbar.h \
    ../../../tsrc/stubs/inc/hbaction.h \
    ../../../tsrc/stubs/inc/hbview.h \
    ../../../tsrc/stubs/inc/hblistviewitem.h \
    ../../../tsrc/stubs/inc/hblistview.h \
    ../../../tsrc/stubs/inc/hbglobal.h \
    ../../../tsrc/stubs/inc/hbstyleloader.h \
    ../../../tsrc/stubs/inc/hbmenu.h \
    ../../../tsrc/stubs/inc/hbmessagebox.h \
    ../../../tsrc/stubs/inc/xqserviceutil.h \
    ../../../videocollectionview/inc/videocollectionviewutils.h \
    ../../../videocollectionview/inc/videocollectionuiloader.h \
    ../../../videocollectionview/inc/videolistselectiondialog.h \
    ../../../videocollectionwrapper/inc/videocollectionwrapper.h \
    ../../../videocollectionwrapper/inc/videosortfilterproxymodel.h \
    ../../../videocollectionwrapper/inc/videolistdatamodel.h \
    ../../../videocollectionwrapper/inc/videothumbnaildata.h \
    ../../../../videoplayerapp/videoplayerengine/inc/videoserviceurifetch.h

SOURCES += src/testlistwidget.cpp \
    \ # sources needed in test
    ../../../videocollectionview/src/videolistwidget.cpp \
    \ # sources needed in stubs
    ../../../tsrc/stubs/src/hbwidget.cpp \
    ../../../tsrc/stubs/src/hbscrollbar.cpp \
    ../../../tsrc/stubs/src/hbaction.cpp \
    ../../../tsrc/stubs/src/hblistviewitem.cpp \
    ../../../tsrc/stubs/src/hblistview.cpp \
    ../../../tsrc/stubs/src/hbglobal.cpp \
    ../../../tsrc/stubs/src/hbstyleloader.cpp \
    ../../../tsrc/stubs/src/hbmenu.cpp \
    ../../../tsrc/stubs/src/hbmessagebox.cpp \
    ../../../tsrc/stubs/src/hbdialog.cpp \
    ../../../tsrc/stubs/src/videocollectionviewutils.cpp \
    ../../../tsrc/stubs/src/videocollectionuiloader.cpp \
    ../../../tsrc/stubs/src/videolistselectiondialog.cpp \
    ../../../tsrc/stubs/src/videocollectionwrapper.cpp \
    ../../../tsrc/stubs/src/videosortfilterproxymodel.cpp \
    ../../../tsrc/stubs/src/videolistdatamodel.cpp \
    ../../../tsrc/stubs/src/videothumbnaildata.cpp \
    ../../../tsrc/stubs/src/videoservices.cpp \
    ../../../tsrc/stubs/src/videoserviceurifetch.cpp \
    