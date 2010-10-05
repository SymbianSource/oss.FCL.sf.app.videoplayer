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
DEFINES     += BUILD_VIDEOCOLLECTION_DLL
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
    ../../../inc

CONFIG += qtestlib \
    Hb \
    symbian_test
          
LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll

# Input
HEADERS +=  inc/testvideoproxymodelgeneric.h \
            stub/inc/filterproxytester.h \
            stub/inc/videolistdatamodel.h \
            stub/inc/videocollectionclient.h \
            stub/inc/videothumbnaildata.h \
            stub/inc/videocollectionwrapper.h \
            ../../inc/videoproxymodelgeneric.h \
            ../../../inc/videocollectionexport.h
    
SOURCES += src/testvideoproxymodelgeneric.cpp \
           stub/src/filterproxytester.cpp \
           stub/src/videolistdatamodel.cpp \
           stub/src/videocollectionclient.cpp \
           stub/src/videothumbnaildata.cpp \
           ../../src/videoproxymodelgeneric.cpp
