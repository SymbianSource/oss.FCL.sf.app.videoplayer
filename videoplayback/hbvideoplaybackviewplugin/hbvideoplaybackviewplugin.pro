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
# Version : %version: da1mmcf#16 %


TEMPLATE = lib
CONFIG += hb qt ecomplugin
TARGET = hbvideoplaybackviewplugin

symbian: 
{ 
    TARGET.CAPABILITY = ALL -TCB -DRM
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20024335
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE \
                   $$APP_LAYER_SYSTEMINCLUDE SYSTEMINCLUDE
    BLD_INF_RULES.prj_exports += "rom/hbvideoplaybackviewplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(hbvideoplaybackviewplugin.iby)"
}

SERVICE.INTERFACE_NAME = org.nokia.mmdt.QViewPlugin/1.0
SERVICE.CONFIGURATION  = "<t>0x20024338</t><p></p><i>EMPXViewPluginPriorityNormal</i><f>0x00000002</f>"


INCLUDEPATH += ../../inc ../inc
	 
LIBS += -lmpxviewframeworkqt.dll \
        -lhbvideoplaybackview.dll \
        -lflogger.dll

DEPENDPATH += inc
VPATH += src

HEADERS += mpxhbvideoplaybackviewplugin.h

SOURCES += mpxhbvideoplaybackviewplugin.cpp

