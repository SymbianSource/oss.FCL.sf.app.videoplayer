#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


do_nothing :
	@echo ---
	@echo Do nothing
	@echo ---

ifeq (WINSCW,$(findstring WINSCW, $(PLATFORM)))
MAKMAKE : do_nothing

else
MAKMAKE :
	@echo ---
	@echo ARMV5
	@echo ---
endif

ifeq (WINSCW,$(findstring WINSCW, $(PLATFORM)))
BLD : do_nothing
else
ifeq (UREL,$(findstring UREL, $(CFG)))
BLD :
	@echo ---
	$(EPOCROOT)epoc32\tools\makesis -v cenrep_rel6.pkg cenrep.sis
	$(EPOCROOT)epoc32\tools\signsis cenrep.sis cenrep.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key	
	
	$(EPOCROOT)epoc32\tools\makesis -v Videocenter_Rel6.pkg Videocenter_Rel6.sis
	$(EPOCROOT)epoc32\tools\signsis Videocenter_Rel6.sis videocenter.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key	
	
	if exist Videocenter_Rel6.sis erase Videocenter_Rel6.sis
	if exist cenrep.sis erase cenrep.sis
	
	@echo ---
else
BLD :
	@echo ---
	@echo armv5 udeb
	@echo ---
endif
endif

CLEAN :
	if exist Videocenter_Rel6.sis erase Videocenter_Rel6.sis
	if exist videocenter.sisx erase videocenter.sisx

FINAL FREEZE LIB CLEANLIB RESOURCE RELEASABLES SAVESPACE : do_nothing