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
    
CONFIG += qtestlib \
    Hb \
    symbian_test

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lxqplugins.dll \
    -lmpxviewframeworkqt.dll \
    -lxqservice.dll \
    -lxqserviceutil.dll

HEADERS_TEMP = \
    inc/hbabstractitemview.h \
    inc/hbdocumentloader.h \
    inc/hbaction.h \
    inc/hbcheckbox.h \
    inc/hbdeviceprofile.h \
    inc/hbdialog.h \
    inc/hbglobal.h \
    inc/hbgroupbox.h \
    inc/hbinputdialog.h \
    inc/hbinstance.h \
    inc/hblabel.h \
    inc/hblistwidget.h \
    inc/hblistview.h \
    inc/hblistviewitem.h \
    inc/hbmainwindow.h \
    inc/hbmenu.h \
    inc/hbmenudata.h \
    inc/hbmessagebox.h \
    inc/hbmessageboxdata.h \
    inc/hbnamespace.h \
    inc/hbpushbutton.h \
    inc/hbscrollbar.h \
    inc/hbstackedwidget.h \
    inc/hbstyleloader.h \
    inc/hbtoolbar.h \
    inc/hbwidget.h \
    inc/hbview.h \
    inc/hbeffect.h \
    inc/videocollectionexport.h \
    inc/videocollectionuiloaderdata.h \
    inc/videocollectionviewutilsdata.h \
    inc/videocollectionwrapperdata.h \
    inc/videolistdatamodeldata.h \
    inc/videolistselectiondialogdata.h \
    inc/videolistwidgetdata.h \
    inc/videolistviewdata.h \
    inc/videoplayerappexport.h \
    inc/videosortfilterproxymodeldata.h \
    inc/videothumbnailtestdata.h \
    inc/xqserviceutil.h \
    inc/xqserviceutilxtra.h \
    ../../videocollectionview/inc/videocollectionuiloader.h \
    ../../videocollectionview/inc/videocollectionviewutils.h \
    ../../videocollectionview/inc/videohintwidget.h \
    ../../videocollectionview/inc/videolistselectiondialog.h \
    ../../videocollectionview/inc/videolistwidget.h \
    ../../videocollectionview/inc/videolistview.h \
    ../../videocollectionwrapper/inc/videolistdatamodel.h \
    ../../videocollectionwrapper/inc/videocollectionwrapper.h \
    ../../videocollectionwrapper/inc/videosortfilterproxymodel.h \
    ../../videocollectionwrapper/inc/videothumbnaildata.h \
    ../../../videoplayerapp/videoplayerengine/inc/videoservices.h \
    ../../../videoplayerapp/videoplayerengine/inc/videoserviceurifetch.h \

SOURCES_TEMP = \
    src/hbdocumentloader.cpp \
    src/hbaction.cpp \
    src/hbcheckbox.cpp \
    src/hbdialog.cpp \
    src/hbinputdialog.cpp \
    src/hbglobal.cpp \
    src/hblabel.cpp \
    src/hblistview.cpp \
    src/hblistviewItem.cpp \
    src/hbmainwindow.cpp \
    src/hbmenu.cpp \
    src/hbmessagebox.cpp \
    src/hbscrollbar.cpp \
    src/hbstackedwidget.cpp \
    src/hbstyleloader.cpp \
    src/hbwidget.cpp \
    src/xqserviceutilxtra.cpp \
    src/videocollectionuiloader.cpp \
    src/videocollectionviewutils.cpp \
    src/videohintwidget.cpp \
    src/videolistselectiondialog.cpp \
    src/videolistwidget.cpp \
    src/videolistview.cpp \
    src/videolistdatamodel.cpp \
    src/videocollectionwrapper.cpp \
    src/videosortfilterproxymodel.cpp \
    src/videothumbnaildata.cpp \
    src/videoservices.cpp \
    src/videoserviceurifetch.cpp \

HEADERS += $$find(HEADERS_TEMP, ^(?!.*$$TESTEDCLASS).*$)
SOURCES += $$find(SOURCES_TEMP, ^(?!.*$$TESTEDCLASS).*$)
