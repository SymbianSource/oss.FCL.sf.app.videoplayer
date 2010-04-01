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
    
CONFIG += qtestlib \
    Hb \
    symbian_test
          
LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll \
    -lxqplugins.dll \
    -lmpxviewframeworkqt.dll

# Input
HEADERS += inc/testcollectionview.h \
           stub/inc/videolistview.h \
           stub/inc/videocollectionuiloader.h \
           ../../inc/videocollectionviewplugin.h \
               
SOURCES += src/testcollectionview.cpp \
           stub/src/videolistview.cpp \
           stub/src/videocollectionuiloader.cpp \
           ../../src/videocollectionviewplugin.cpp

RESOURCES += ../../data/videocollectionview.qrc

