/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:      Telephony client component.*
*/




#ifndef VCXHGVODTELEPHONYCLIENT_H
#define VCXHGVODTELEPHONYCLIENT_H

// INCLUDE FILES
#include <e32base.h>
#include <etelmm.h>


/**
* Class connects to ETel for asking lock code from user.
* Copied from vcnsuis/vcnssettings/vcnssettingsengine.
*/
NONSHARABLE_CLASS( CVcxHgVodTelephonyClient ) : public CBase
    { 

    public:

        /**
         * Constructor.
         */
        IMPORT_C CVcxHgVodTelephonyClient();

        /**
         * Closes the ETel connection.
         */
        virtual ~CVcxHgVodTelephonyClient();

        /**
         * Check the lock code 
         *
         * @return ETrue if the user enters the correct lock code
         */
        IMPORT_C TBool CheckLockCodeL();
  
    private: // data
         
        /**
         * Flag that tells when connection has been made.
         */
        TBool iPhoneConnected;
        
        /**
         * Telephony server session.
         */
        RTelServer iServer;
        
        /**
         * Access to mobile phone functionality.
         */
        RMobilePhone iPhone;

    };

#endif // VCXHGTELEPHONYCLIENT_H

