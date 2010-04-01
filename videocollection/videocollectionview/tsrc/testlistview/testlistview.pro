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
INCLUDEPATH += stub/inc \
    . \
    /epoc32/include/domain \
    /epoc32/include/domain/middleware \
    /epoc32/include/domain/applications \
    /epoc32/include/osextensions \
    /epoc32/include/middleware \
    /epoc32/include/osextensions/stdapis/stlport \    
    ../../../inc
    
CONFIG += qtestlib \
    Hb \
    symbian_test
    
LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll \
    -lxqplugins.dll \
    -lmpxviewframeworkqt.dll \
    -lxqserviceutil.dll

# Input
HEADERS += stub/inc/hbmessagebox.h \
           inc/testlistview.h \           
           stub/inc/videolistwidget.h \
           stub/inc/videohintwidget.h \
           stub/inc/videocollectionviewutils.h \
           stub/inc/videolistselectiondialog.h \
           stub/inc/videosortfilterproxymodel.h \
           stub/inc/videocollectionwrapper.h \
           stub/inc/dummydatamodel.h \
           stub/inc/videocollectionuiloader.h \
           stub/inc/dummyhbmenu.h \
           stub/inc/videoservices.h \
           stub/inc/videoserviceurifetch.h \
           ../../inc/videolistview.h \
               
SOURCES += stub/src/hbmessagebox.cpp \
           src/testlistview.cpp \           
           stub/src/videolistwidget.cpp \
           stub/src/videohintwidget.cpp \
           stub/src/videocollectionviewutils.cpp \
           stub/src/videolistselectiondialog.cpp \ 
           stub/src/videosortfilterproxymodel.cpp \
           stub/src/videocollectionwrapper.cpp \
           stub/src/dummydatamodel.cpp \
           stub/src/videocollectionuiloader.cpp \
           stub/src/videoservices.cpp \
           stub/src/videoserviceurifetch.cpp \
           ../../src/videolistview.cpp \

RESOURCES += ../../data/videocollectionview.qrc           

