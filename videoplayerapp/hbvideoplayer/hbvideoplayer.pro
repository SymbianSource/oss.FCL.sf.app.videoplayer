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
# Version : %version: da1mmcf#17 %


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

RSS_RULES += \
"    datatype_list =                                                                              " \
"        {                                                                                        " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"video/avi\"; }, 											" \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"video/msvideo\"; },                  " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"application/x-msvideo\"; },          " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"video/x-pn-realvideo\"; },           " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-pn-realaudio\"; },           " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"video/vnd.rn-realvideo\"; },         " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"application/vnd.rn-realmedia\"; },   " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"application/x-pn-realmedia\"; },     " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"audio/x-pn-realaudio-plugin\"; },    " \
"        DATATYPE { priority=EDataTypePrioritySystem; type=\"video/x-matroska\"; },               " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"video/x-ms-wmv\"; },                   " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"video/x-ms-asf\"; },                   " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"video/3gp\"; },                        " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"video/3gpp2\"; },                      " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"video/3gpp\"; },                       " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"video/mp4\"; },                        " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"video/x-m4v\"; },                      " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"video/mpeg4\"; },                      " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"application/ram\"; },                  " \
"        DATATYPE { priority=EDataTypePriorityHigh; type=\"application/sdp\"; }                   " \
"        };                                                                                       " \          
