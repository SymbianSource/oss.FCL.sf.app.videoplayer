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
# Version : %version: 32 %


TEMPLATE = lib
CONFIG += hb qt dll
TARGET = videoplaybackview
DEFINES += BUILD_VIDEOPLAYBACK_DLL

symbian:
{
    TARGET.CAPABILITY = ALL -TCB -DRM
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20024334
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE \
                   $$APP_LAYER_SYSTEMINCLUDE SYSTEMINCLUDE
    BLD_INF_RULES.prj_exports += "rom/videoplaybackview.iby CORE_APP_LAYER_IBY_EXPORT_PATH(videoplaybackview.iby)"
    defBlock = \      
        "$${LITERAL_HASH}if defined(EABI)" \
        "DEFFILE ../eabi/videoplaybackview.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE ../bwins/videoplaybackview.def" \
        "$${LITERAL_HASH}endif"
    MMP_RULES += defBlock SMPSAFE
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

HEADERS += videobaseplaybackview.h \
           videoplaybackview.h \
           videoplaybackviewfiledetails.h \
           mpxvideoviewwrapper.h \
           mpxvideoregion.h \
           videoplaybackdisplayhandler.h \
           videoplaybackfullscreencontrol.h \
           videoplaybackstatuspanecontrol.h \
           videoplaybackprogressbar.h \
           videoplaybacktoolbar.h \
           videoplaybackcontrolbar.h \
           videoplaybackcontrolpolicy.h \
           videoplaybackcontrolconfiguration.h \
           videoplaybackcontrolscontroller.h \
           videoplaybackdocumentloader.h \
           videoplaybackfiledetailswidget.h \
           videoplaybackuserinputhandler.h \
           videoplaybackdetailsplaybackwindow.h \
           videocontainer.h

SOURCES += videobaseplaybackview.cpp \
           videoplaybackview.cpp \
           videoplaybackviewfiledetails.cpp \
           mpxvideoviewwrapper.cpp \
           videoplaybackdisplayhandler.cpp \
           videoplaybackfullscreencontrol.cpp \
           videoplaybackstatuspanecontrol.cpp \
           videoplaybackprogressbar.cpp \
           videoplaybacktoolbar.cpp \
           videoplaybackcontrolbar.cpp \
           videoplaybackcontrolscontroller.cpp \
           videoplaybackcontrolpolicy.cpp \
           videoplaybackcontrolconfiguration.cpp \
           videoplaybackdocumentloader.cpp \
           videoplaybackfiledetailswidget.cpp \
           videoplaybackuserinputhandler.cpp \
           videoplaybackdetailsplaybackwindow.cpp \
           videocontainer.cpp

DOCML += resources/videoplaybackview.docml

RESOURCES += resources/videoplaybackview.qrc
