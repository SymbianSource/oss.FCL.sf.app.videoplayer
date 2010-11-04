#
# Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif


TARGETDIR=$(ZDIR)\RESOURCE\APPS
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\mediasettings.mif
HEADERFILENAME=$(HEADERDIR)\mediasettings.mbg


do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : 
	del $(ICONTARGETFILENAME)
	del $(HEADERFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing


RESOURCE : 
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
	   /c8,8 qgn_menu_video.svg\
	   /c8,8 qgn_prop_set_service.svg\
	   /c8,8 qgn_prop_set_apps_mp.bmp\
           /c8,8 qgn_prop_set_mp_video_sub.bmp \
           /c8,8 qgn_prop_set_mp_stream_sub.bmp \
           /c8,8 qgn_prop_set_mp_audio_sub.bmp \
           /c8,1 qgn_prop_set_mp_video_tab2.bmp \
           /c8,1 qgn_prop_set_mp_audio_tab3.bmp \
           /c8,1 qgn_prop_set_mp_stream_tab2.bmp \
           /c8,1 qgn_prop_set_mp_video_tab3.bmp

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
