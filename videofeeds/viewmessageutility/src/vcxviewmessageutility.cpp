/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Message sending component for MPX views.*
*/




#include "IptvDebug.h"

#include "vcxviewmessageutility.h"
#include "vcxviewmessageutility.hrh"

// Each CCoeStatic object must have a unique TUid, let's use dll uid
const TUid KVcxViewMessageUtilitySingletonUid = { KVcxViewMessageUtilityDllUid };

// Negative priority destroys this singleton after AppUi is destroyed.
// This way all views can remove observers before this object is destroyed.
const TInt KVcxSingletonDestructionPriority( -1 );

// ---------------------------------------------------------------------------
// Static function that initializes the singleton object
// ---------------------------------------------------------------------------
//
EXPORT_C CVcxViewMessageUtility* CVcxViewMessageUtility::InstanceL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxViewMessageUtility::InstanceL"); 
    // Ownership of the object remains with CCoeEnv
    CVcxViewMessageUtility* instance = static_cast<CVcxViewMessageUtility*>( 
        CCoeEnv::Static( KVcxViewMessageUtilitySingletonUid ) );
    
    if ( !instance )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxViewMessageUtility::InstanceL - Creating object"); 
        instance = new( ELeave ) CVcxViewMessageUtility();
        }
    return instance;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CVcxViewMessageUtility::~CVcxViewMessageUtility()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxViewMessageUtility destructor"); 
    iObservers.Close();
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxViewMessageUtility::CVcxViewMessageUtility() :
        CCoeStatic( KVcxViewMessageUtilitySingletonUid, 
                    KVcxSingletonDestructionPriority )
    {
    }

// -----------------------------------------------------------------------------
// Adds observer
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxViewMessageUtility::AddObserverL( MVcxViewMessageObserver& aObserver )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CVcxViewMessageUtility::AddObserverL - Adding %X", &aObserver); 
    TInt errorCode = KErrNoMemory;
    MtvObserverUtils::AddObserverL(iObservers, aObserver, errorCode);
    }

// -----------------------------------------------------------------------------
// Removes observer
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxViewMessageUtility::RemoveObserver( MVcxViewMessageObserver& aObserver )
    {
    MtvObserverUtils::RemoveObserver(iObservers, aObserver );       
    }

// -----------------------------------------------------------------------------
// Forwards messages to views
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxViewMessageUtility::SendCustomViewMessageL( 
        const TUid aMessageUid, const TDesC8 &aCustomMessage )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CVcxViewMessageUtility::SendCustomViewMessageL Uid=%d", aMessageUid.iUid); 
    // Send the same message to all observers.
    TInt err( KErrNone );
    RUN_NOTIFY_LOOP_L( HandleCustomViewMessageL( aMessageUid, aCustomMessage ), iObservers, err );
    User::LeaveIfError( err );
    }

