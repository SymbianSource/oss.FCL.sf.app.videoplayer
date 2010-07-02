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
# Version : %version: 25 %


TEMPLATE = lib
CONFIG += hb qt dll
TARGET = hbvideoplaybackview
DEFINES += BUILD_VIDEOPLAYBACK_DLL

symbian:
{
    TARGET.CAPABILITY = ALL -TCB -DRM
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20024334
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE \
                   $$APP_LAYER_SYSTEMINCLUDE SYSTEMINCLUDE
    BLD_INF_RULES.prj_exports += "rom/hbvideoplaybackview.iby CORE_APP_LAYER_IBY_EXPORT_PATH(hbvideoplaybackview.iby)"
    defBlock = \      
        "$${LITERAL_HASH}if defined(EABI)" \
        "DEFFILE ../eabi/hbvideoplaybackview.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE ../bwins/hbvideoplaybackview.def" \
        "$${LITERAL_HASH}endif"
    MMP_RULES += defBlock
}

INCLUDEPATH += ../../inc 

LIBS += -lmpxplaybackutility.dll \
        -lmpxcommon.dll \
        -lestor.dll \
        -lcommonengine.dll \
        -lflogger.dll \
        -lsysutil.dll \
        -lmpxcollectionutility.dll \
        -lremconcoreapi.dll \
        -lremconInterfacebase.dll \
        -lcentralrepository.dll \
        -lthumbnailmanagerqt.dll \
        -lmediaclientvideodisplay.dll \
        -lxqserviceutil.dll \
        -lvideoplayerengine.dll \
        -lcone.dll \
        -lefsrv.dll \
        -lws32.dll \
        -lhal.dll \
        -lgdi.dll \
        -lshareui.dll 

DEPENDPATH += ../inc inc viewinc controlinc
VPATH += viewsrc controlsrc

HEADERS += hbvideobaseplaybackview.h \
           hbvideoplaybackview.h \
           mpxvideoplaybackviewfiledetails.h \
           mpxvideoviewwrapper.h \
           mpxvideoregion.h \
           mpxvideoplaybackdisplayhandler.h \
           mpxvideoplaybackfullscreencontrol.h \
           mpxvideoplaybackstatuspanecontrol.h \
           mpxvideoplaybackprogressbar.h \
           mpxvideoplaybacktoolbar.h \
           mpxvideoplaybackcontrolbar.h \
           mpxvideoplaybackcontrolpolicy.h \
           mpxvideoplaybackcontrolconfiguration.h \
           mpxvideoplaybackcontrolscontroller.h \
           mpxvideoplaybackdocumentloader.h \
           mpxvideoplaybackfiledetailswidget.h \
           mpxvideoplaybackuserinputhandler.h \
           mpxvideoplaybackdetailsplaybackwindow.h \
           mpxvideocontainer.h

SOURCES += hbvideobaseplaybackview.cpp \
           hbvideoplaybackview.cpp \
           mpxvideoplaybackviewfiledetails.cpp \
           mpxvideoviewwrapper.cpp \
           mpxvideoplaybackdisplayhandler.cpp \
           mpxvideoplaybackfullscreencontrol.cpp \
           mpxvideoplaybackstatuspanecontrol.cpp \
           mpxvideoplaybackprogressbar.cpp \
           mpxvideoplaybacktoolbar.cpp \
           mpxvideoplaybackcontrolbar.cpp \
           mpxvideoplaybackcontrolscontroller.cpp \
           mpxvideoplaybackcontrolpolicy.cpp \
           mpxvideoplaybackcontrolconfiguration.cpp \
           mpxvideoplaybackdocumentloader.cpp \
           mpxvideoplaybackfiledetailswidget.cpp \
           mpxvideoplaybackuserinputhandler.cpp \
           mpxvideoplaybackdetailsplaybackwindow.cpp \
           mpxvideocontainer.cpp

RESOURCES += resources/hbvideoplaybackview.qrc
