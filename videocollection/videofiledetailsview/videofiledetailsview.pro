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
CONFIG += hb \
    qt \
    ecomplugin


symbian: { 
   TARGET.UID2 = 0x10009D8D
   TARGET.UID3 = 0x200211FF

   BLD_INF_RULES.prj_exports += "rom/videofiledetailsview.iby CORE_APP_LAYER_IBY_EXPORT_PATH(videofiledetailsview.iby)"
   
   TARGET.CAPABILITY = ALL -TCB -DRM
   
   TARGET.EPOCALLOWDLLDATA = 1
}

# mpx view plugin definitions:
# plugin interface name
SERVICE.INTERFACE_NAME = org.nokia.mmdt.QViewPlugin/1.0

# opaque data for resolving the correct plugin
SERVICE.CONFIGURATION = "<t>0x20021200</t><p></p><i>EMPXViewPluginPriorityNormal</i><f>0x00000001</f>"

DEPENDPATH += .

INCLUDEPATH += .

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

INCLUDEPATH += $$OS_LAYER_STDCPP_SYSTEMINCLUDE \
               ../../videoplayerapp/videoplayerengine/inc \
               ../../videoplayerapp/inc \
    		/epoc32/include/mw/hb/hbtools \
    		/epoc32/include/mw

INCLUDEPATH += ../../inc \
               ../inc \
               ../videocollectionwrapper/inc

# Input
HEADERS += inc/videofiledetailsviewplugin.h \
   
SOURCES += src/videofiledetailsviewplugin.cpp 
   
LIBS += -lmpxviewframeworkqt.dll \
        -lvideocollectionwrapper.dll \
        -lthumbnailmanagerqt.dll \
        -lmediaclientvideodisplay.dll \
        -lxqserviceutil.dll \
        -lvideoplayerengine.dll

RESOURCES += data/videofiledetails.qrc
