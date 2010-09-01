rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of the License "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:  
rem

dll_versio_nro.pl Videocenter_NFP1.pkg

makesis cenrep_alr.pkg cenrep.sis

signsis cenrep.sis cenrep.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del cenrep.sis

call makesis Videocenter_NFP1.pkg
call signsis Videocenter_NFP1.sis Video_Center.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del Videocenter_NFP1.sis
call del cenrep.sisx
