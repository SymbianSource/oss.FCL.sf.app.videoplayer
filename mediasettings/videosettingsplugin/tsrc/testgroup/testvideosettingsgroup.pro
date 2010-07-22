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

TARGET = testvideosettingsgroup
TEMPLATE = app

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH += inc \
               stub/inc \
               ../../inc \
               ../../../mediasettingsengine/inc \
               ../../../../inc \

CONFIG += qtestlib \
    Hb \
    symbian_test

LIBS += -lcpframework \
        -lxqutils.dll

# Input
HEADERS += inc/testvideosettingsgroup.h \
           ../../inc/videosettingsgroup.h \
           stub/inc/mpsettingsmodel.h \
           stub/inc/hbdataformmodel.h \
           stub/inc/cpitemdatahelper.h \
           stub/inc/hbvalidator.h \
           stub/inc/hblineedit.h \
           stub/inc/hbinputeditorinterface.h \
           stub/inc/videosettingsaccesspointentry.h \
   
SOURCES += src/testvideosettingsgroup.cpp \
           ../../src/videosettingsgroup.cpp \
           stub/src/mpsettingsmodel.cpp \
           stub/src/hbdataformmodel.cpp \
           stub/src/cpitemdatahelper.cpp \
           stub/src/hbvalidator.cpp \
           stub/src/hblineedit.cpp \
           stub/src/hbinputeditorinterface.cpp \
           stub/src/videosettingsaccesspointentry.cpp \

TARGET.CAPABILITY = All -Tcb
