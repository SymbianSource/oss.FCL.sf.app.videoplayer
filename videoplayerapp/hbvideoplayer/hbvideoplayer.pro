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
# Description: Project file for building Videoplayer components
#
#
# Version : %version: 15 %


TARGET = videoplayer
CONFIG += hb service
TEMPLATE = app
TRANSLATIONS += videos.ts

symbian: 
{ 
    TARGET.CAPABILITY = ALL -DRM -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x1600000
    TARGET.UID3 = 0x200211FE

    BLD_INF_RULES.prj_exports += "rom/hbvideoplayer.iby          CORE_APP_LAYER_IBY_EXPORT_PATH(hbvideoplayer.iby)" \
                                 "rom/hbvideoplayerresources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(hbvideoplayerresources.iby)" \
                                 "sis/videoplayer_stub.sis       /epoc32/data/z/system/install/videoplayer_stub.sis"
}

# Service provider specific configuration.
SERVICE.FILE = resources/service_conf.xml
SERVICE.OPTIONS = embeddable
# Service provider specific configuration ends

INCLUDEPATH += .
INCLUDEPATH += ../inc \
               ../../inc \
               /epoc32/include/mw/hb/hbtools

LIBS += -lvideoplayerengine.dll \
        -lxqservice.dll \
        -lxqserviceutil.dll

VPATH += src

SOURCES += main.cpp
           
