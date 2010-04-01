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
    \ # keep these at bottom so that stubbed headers are taken first
    ../../../inc \
    ../../../videocollectionwrapper/inc
    
CONFIG += qtestlib \
    Hb \
    symbian_test

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll

# Input
HEADERS +=  inc/testvideocollectionviewutils.h \
    \ # headers needed in test
    ../../inc/videocollectionviewutils.h \
    \ # headers needed in stubs
    ../../../tsrc/stubs/inc/hbaction.h \
    ../../../tsrc/stubs/inc/hblabel.h \
    ../../../tsrc/stubs/inc/hbmessagebox.h \
    ../../../tsrc/stubs/inc/hbdialog.h \
    ../../../tsrc/stubs/inc/hbwidget.h \
    ../../../tsrc/stubs/inc/centralrepository.h \
    ../../../videocollectionwrapper/inc/videocollectionwrapper.h
    
SOURCES += src/testvideocollectionviewutils.cpp \
    \ # sources needed in test
    ../../src/videocollectionviewutils.cpp \
    \ # sources needed in stubs
    ../../../tsrc/stubs/src/hbaction.cpp \
    ../../../tsrc/stubs/src/hblabel.cpp \
    ../../../tsrc/stubs/src/hbmessagebox.cpp \
    ../../../tsrc/stubs/src/hbdialog.cpp \
    ../../../tsrc/stubs/src/hbwidget.cpp \
    ../../../tsrc/stubs/src/centralrepository.cpp \
    ../../../tsrc/stubs/src/videocollectionwrapper.cpp
           
