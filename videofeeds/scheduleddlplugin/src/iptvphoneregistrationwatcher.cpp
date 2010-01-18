/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    This is helper class which observes whether the phone is on*
*/




#include "IptvDebug.h"
#include <mmtsy_names.h>

#include "iptvphoneregistrationwatcher.h"
#include "iptvphoneregistrationobserver.h"



// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::CIptvPhoneRegistrationWatcher
// ---------------------------------------------------------------------------
//
CIptvPhoneRegistrationWatcher::CIptvPhoneRegistrationWatcher() : CActive( EPriorityNormal )
    {
    }


// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CIptvPhoneRegistrationWatcher::ConstructL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvPhoneRegistrationWatcher::ConstructL");

    CActiveScheduler::Add(this);

	User::LeaveIfError( iTelServer.Connect() );
    User::LeaveIfError( iMobilePhone.Open( iTelServer, KMmTsyPhoneName ) );

    StartObserving();

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvPhoneRegistrationWatcher::ConstructL");
    }


// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::NewL
// ---------------------------------------------------------------------------
//
CIptvPhoneRegistrationWatcher* CIptvPhoneRegistrationWatcher::NewL()
    {
    CIptvPhoneRegistrationWatcher* self = CIptvPhoneRegistrationWatcher::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::NewLC
// ---------------------------------------------------------------------------
//
CIptvPhoneRegistrationWatcher* CIptvPhoneRegistrationWatcher::NewLC()
    {
    CIptvPhoneRegistrationWatcher* self = new( ELeave ) CIptvPhoneRegistrationWatcher;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::~CIptvPhoneRegistrationWatcher
// ---------------------------------------------------------------------------
//
CIptvPhoneRegistrationWatcher::~CIptvPhoneRegistrationWatcher()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvPhoneRegistrationWatcher::~CIptvPhoneRegistrationWatcher");

    Cancel();
    iObservers.Reset();
    iMobilePhone.Close();
    iTelServer.Close();

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvPhoneRegistrationWatcher::~CIptvPhoneRegistrationWatcher");
    }


// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::AddObserverL
// ---------------------------------------------------------------------------
//
void CIptvPhoneRegistrationWatcher::AddObserverL( MPhoneRegistrationWatcherObserver* aObserver )
    {
    iObservers.AppendL( aObserver );
    }


// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::RemoveObserver
// ---------------------------------------------------------------------------
//
void CIptvPhoneRegistrationWatcher::RemoveObserver( MPhoneRegistrationWatcherObserver* aObserver )
    {
    TInt index = iObservers.Find( aObserver );
    if (index != KErrNotFound)
        {
        iObservers.Remove( index );
        }
    }


// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::IsOnHomeNetwork
// ---------------------------------------------------------------------------
//
TBool CIptvPhoneRegistrationWatcher::IsOnHomeNetwork()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvPhoneRegistrationWatcher::IsOnHomeNetwork");

    TBool retVal = EFalse;

	TRequestStatus status;
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
	iMobilePhone.GetNetworkRegistrationStatus( status, regStatus );
	User::WaitForRequest( status );

	if (status.Int() == KErrNone && regStatus == RMobilePhone::ERegisteredOnHomeNetwork)
		{
		retVal = ETrue;
        IPTVLOGSTRING_HIGH_LEVEL("CIptvPhoneRegistrationWatcher::IsOnHomeNetwork, on home network");
		}

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvPhoneRegistrationWatcher::IsOnHomeNetwork");

    return retVal;
    }

// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::StartObserving
// ---------------------------------------------------------------------------
//
void CIptvPhoneRegistrationWatcher::StartObserving()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvPhoneRegistrationWatcher::StartObserving");

    if (!IsActive())
        {
        iMobilePhone.NotifyNetworkRegistrationStatusChange( iStatus, iRegStatus );
        SetActive();
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvPhoneRegistrationWatcher::StartObserving");
    }


// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::RunL
//
// From class CActive.
// ---------------------------------------------------------------------------
//
void CIptvPhoneRegistrationWatcher::RunL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvPhoneRegistrationWatcher::RunL");

    //  Get the current state and inform observers
    if (iStatus.Int() != KErrNone)
        {
        //  Inform observers about error
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvPhoneRegistrationWatcher::RunL, error = %d", iStatus.Int());
        }
    else
        {
        if (iRegStatus != RMobilePhone::ERegisteredOnHomeNetwork)
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvPhoneRegistrationWatcher::RunL, not on home network");
            }
        else
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvPhoneRegistrationWatcher::RunL, on home network");
            }

        //  Inform observers the home network state
        for (TInt i = 0; i < iObservers.Count(); i++)
            {
            iObservers[i]->OnHomeNetwork(
                iRegStatus == RMobilePhone::ERegisteredOnHomeNetwork );
            }

        }

    StartObserving();

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvPhoneRegistrationWatcher::RunL");
    }

// ---------------------------------------------------------------------------
// CIptvPhoneRegistrationWatcher::DoCancel
// 
// From class CActive.
// ---------------------------------------------------------------------------
//
void CIptvPhoneRegistrationWatcher::DoCancel()
    {
    iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifyNetworkRegistrationStatusChange );
    }

