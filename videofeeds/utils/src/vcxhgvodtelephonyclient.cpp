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
* Description:      Implementation of a telephony client.*
*/




// INCLUDE FILES
#include <mmtsy_names.h>
#include <SecUi.h>
#include <SecUiSecurityHandler.h>

#include "vcxhgvodtelephonyclient.h"

// CONSTANTS
const TInt KTimeBeforeRetryingServerConnection = 50000;

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CVcxHgVodTelephonyClient::CVcxHgVodTelephonyClient()
     : iPhoneConnected( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CVcxHgVodTelephonyClient::~CVcxHgVodTelephonyClient()
    {
    if ( iPhoneConnected )
        {
        // close phone
        if ( iPhone.SubSessionHandle() )
            {
            iPhone.Close();
            }

        //close ETel connection
        if ( iServer.Handle() )
            {
            iServer.UnloadPhoneModule( KMmTsyModuleName );
            iServer.Close();
            }
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
EXPORT_C TBool CVcxHgVodTelephonyClient::CheckLockCodeL()
    {
    TBool lockCodeAccepted = EFalse ;
    
    if ( !iPhoneConnected )
        {
        // Connect to ETel server
        // All server connections are retried because occasional
        // fails on connections are possible, at least on some servers.
        TInt err = iServer.Connect();
        if ( err != KErrNone )
            {
            User::After( KTimeBeforeRetryingServerConnection );
            TInt err = iServer.Connect();
            }
        User::LeaveIfError( err );
        User::LeaveIfError( iServer.SetExtendedErrorGranularity(
            RTelServer::EErrorExtended ) );

        // Load TSY.
        err = iServer.LoadPhoneModule( KMmTsyModuleName );
        if ( KErrAlreadyExists != err )
            {
            // May also return KErrAlreadyExists if something else
            // has already loaded the TSY module. And that is
            // not an error.
            User::LeaveIfError( err );
            }

        // Open phones
        User::LeaveIfError( iPhone.Open( iServer, KMmTsyPhoneName ) );
        iPhoneConnected = ETrue;
        }

#ifndef __WINS__
    // Initialize sec ui.
    CSecurityHandler* handler = new( ELeave ) CSecurityHandler( iPhone );
    CleanupStack::PushL( handler );
    TSecUi::InitializeLibL();

    lockCodeAccepted = handler->AskSecCodeL();

    // Uninitialize security ui.
    CleanupStack::PopAndDestroy( handler );
    TSecUi::UnInitializeLib();
#else
        lockCodeAccepted = ETrue;
#endif

    return lockCodeAccepted;
    }

