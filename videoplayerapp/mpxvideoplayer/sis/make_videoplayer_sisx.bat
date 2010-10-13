@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: Script file for VideoPlayer
@rem


call makesis.exe cenrep.pkg cenrep.sis 
call signsis.exe cenrep.sis cenrep.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del cenrep.sis

call makesis.exe VideoPlayer.pkg VideoPlayer.sis
call signsis.exe VideoPlayer.sis VideoPlayer.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
call del VideoPlayer.sis
call del cenrep.sisx
