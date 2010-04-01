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

TEMPLATE = lib
TARGET = 
symbian: { 
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x200211FD
    BLD_INF_RULES.prj_exports += "rom/videocollectionwrapper.iby CORE_APP_LAYER_IBY_EXPORT_PATH(videocollectionwrapper.iby)"
    MMP_RULES += "DEFFILE videocollectionwrapper.def"
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
}
CONFIG      += hb qt dll
DEFINES     += BUILD_VIDEOCOLLECTION_DLL

DEPENDPATH += . inc src

INCLUDEPATH += .

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

INCLUDEPATH += $$OS_LAYER_STDCPP_SYSTEMINCLUDE

INCLUDEPATH += ../inc \
               /epoc32/include/mw/hb/hbtools

# Input
HEADERS += inc/videodatasignalreceiver.h \
           inc/videocollectionclient.h \
           inc/videocollectionlistener.h \
           inc/videocollectionwrapper.h \
           inc/videocollectionwrapper_p.h \
           inc/videolistdatamodel.h \
           inc/videolistdatamodel_p.h \
           inc/videothumbnaildata.h \
           inc/videothumbnaildata_p.h \
           inc/videocollectionutils.h \
           inc/videosortfilterproxymodel.h \
           inc/videodatacontainer.h \
           ../inc/videocollectionexport.h
           
SOURCES += src/videocollectionclient.cpp \
           src/videocollectionlistener.cpp \
           src/videocollectionwrapper.cpp \
           src/videocollectionwrapper_p.cpp \
           src/videolistdatamodel.cpp \
           src/videolistdatamodel_p.cpp \
           src/videothumbnaildata.cpp \
           src/videothumbnaildata_p.cpp \
           src/videocollectionutils.cpp \
           src/videosortfilterproxymodel.cpp \
           src/videodatacontainer.cpp
          
LIBS += -lmpxcommon.dll \
        -lmpxcollectionutility.dll \
        -lthumbnailmanagerqt.dll \
        -lestor.dll \
        -lcentralrepository.dll

RESOURCES += data/videocollectionwrapper.qrc
