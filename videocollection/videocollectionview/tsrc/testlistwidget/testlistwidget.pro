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
    /epoc32/include/domain \
    /epoc32/include/domain/middleware \
    /epoc32/include/domain/applications \
    /epoc32/include/osextensions \
    /epoc32/include/middleware \
    /epoc32/include/osextensions/stdapis/stlport \
    stub/inc \
    ../../../inc \
    ../../../../inc \
    
CONFIG += qtestlib \
          symbian_test

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll \
    -lvideocollectionwrapper.dll \
    -lxqserviceutil.dll \

# Input
HEADERS += inc/testlistwidget.h \
           stub/inc/videosortfilterproxymodel.h \
           stub/inc/hbmainwindow.h \
           stub/inc/hbwidget.h \
           stub/inc/hbinstance.h \
           stub/inc/hbscrollbar.h \
           stub/inc/hbaction.h \
           stub/inc/hbwidget.h \
           stub/inc/hbmenu.h \
           stub/inc/hbview.h \
           stub/inc/hblistview.h \
           stub/inc/hblistviewitem.h \
           stub/inc/dummydatamodel.h \
           stub/inc/hbmessagebox.h \
           stub/inc/videothumbnaildata.h \
           stub/inc/videoservices.h \
           stub/inc/videoserviceurifetch.h \
           ../../inc/videolistwidget.h 
               
SOURCES += src/testlistwidget.cpp \
           stub/src/hbaction.cpp \
           stub/src/hbscrollbar.cpp \
           stub/src/hbwidget.cpp \
           stub/src/hblistview.cpp \
           stub/src/hblistviewitem.cpp \
           stub/src/hbmenu.cpp \
           stub/src/videosortfilterproxymodel.cpp \
           stub/src/dummydatamodel.cpp \
           stub/src/hbmessagebox.cpp \
           stub/src/videothumbnaildata.cpp \
           stub/src/videoservices.cpp \
           stub/src/videoserviceurifetch.cpp \
           ../../src/videolistwidget.cpp
