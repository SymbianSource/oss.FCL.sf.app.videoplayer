/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/




// INCLUDE FILES
#include <e32base.h>
#include "IptvDebug.h"
#include "CIptvUtil.h"
#include "CIptvEpgManagerUpdater.h"
#include "MIptvEpgManagerUpdateObserver.h"
#include "CIptvEpgManagerImpl.h"

// CONSTANTS

const TInt KIptvPluginPollDelay = 500000; // 0,5 seconds

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvEpgManagerUpdater::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvEpgManagerUpdater::ConstructL()
    {       
    User::LeaveIfError(iTimer.CreateLocal());            
    }

// ---------------------------------------------------------
// CIptvEpgManagerUpdater::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CIptvEpgManagerUpdater* CIptvEpgManagerUpdater::NewL(MIptvEpgManagerUpdateObserver& aObserver)
    {
    CIptvEpgManagerUpdater* self = new(ELeave) CIptvEpgManagerUpdater(aObserver);
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvEpgManagerUpdater::~CIptvEpgManagerUpdater
// Destructor
// -----------------------------------------------------------------------------
//
CIptvEpgManagerUpdater::~CIptvEpgManagerUpdater()
    {
    Cancel();    
    iTimer.Close();
    }

// ---------------------------------------------------------
// CIptvEpgManagerUpdater::CIptvEpgManagerUpdater
// C++ default constructor
// ---------------------------------------------------------
//
CIptvEpgManagerUpdater::CIptvEpgManagerUpdater(MIptvEpgManagerUpdateObserver& aObserver) : CActive( EPriorityStandard ), 
    iObserver( aObserver )    
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CIptvEpgManagerUpdater::DoCancel
// 
// ---------------------------------------------------------
//
void CIptvEpgManagerUpdater::DoCancel()
    {
    iTimer.Cancel();
    }

// ---------------------------------------------------------
// CIptvEpgManagerUpdater::RunL
// 
// ---------------------------------------------------------
//
void CIptvEpgManagerUpdater::RunL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerUpdater::RunL");

    if (!IsActive())
        {
        iTimer.After(iStatus, KIptvPluginPollDelay);
        SetActive();               
        }

    iObserver.CheckIsPluginRunning();
    }

// ---------------------------------------------------------
// CIptvEpgManagerUpdater::Start
// 
// ---------------------------------------------------------
//
void CIptvEpgManagerUpdater::Start()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerUpdater::Start");

    if (!IsActive())
        {
        iTimer.After(iStatus, KIptvPluginPollDelay);
        SetActive(); 
        }    
    }

// ---------------------------------------------------------
// CIptvEpgManagerUpdater::Stop
// 
// ---------------------------------------------------------
//
void CIptvEpgManagerUpdater::Stop()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgManagerUpdater::Stop");

    iTimer.Cancel();
    Cancel();                  
    }
