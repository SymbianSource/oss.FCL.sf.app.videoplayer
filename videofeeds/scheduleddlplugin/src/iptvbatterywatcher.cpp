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
* Description:    Publish & Subscribe watcher client for battery values.*
*/




#include <hwrmpowerstatesdkpskeys.h>
#include "IptvDebug.h"

#include "iptvbatterywatcher.h"
#include "iptvbatterywatcherobserver.h"



// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::CIptvBatteryWatcher
// ---------------------------------------------------------------------------
//
CIptvBatteryWatcher::CIptvBatteryWatcher() : CActive( EPriorityNormal )
    {
    }


// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::ConstructL
// ---------------------------------------------------------------------------
//
void CIptvBatteryWatcher::ConstructL()
    {
    User::LeaveIfError( iProperty.Attach(
            KPSUidHWRMPowerState,
            KHWRMBatteryStatus,
            EOwnerThread ) );

    CActiveScheduler::Add(this);
    // Do initial subscription and process current property value
    RunL();
    }


// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::NewL
// ---------------------------------------------------------------------------
//
CIptvBatteryWatcher* CIptvBatteryWatcher::NewL()
    {
    CIptvBatteryWatcher* self = CIptvBatteryWatcher::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::NewLC
// ---------------------------------------------------------------------------
//
CIptvBatteryWatcher* CIptvBatteryWatcher::NewLC()
    {
    CIptvBatteryWatcher* self = new( ELeave ) CIptvBatteryWatcher;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::~CIptvBatteryWatcher
// ---------------------------------------------------------------------------
//
CIptvBatteryWatcher::~CIptvBatteryWatcher()
    {
    Cancel();
    iProperty.Close();
    iObservers.Reset();
    }


// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::GetBatteryStatus
// ---------------------------------------------------------------------------
//
TInt CIptvBatteryWatcher::GetBatteryStatus( TInt& aBatteryStatus )
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvBatteryWatcher::GetBatteryStatus");

    RProperty batteryProperty;

    TInt err = batteryProperty.Attach(
            KPSUidHWRMPowerState,
            KHWRMBatteryStatus,
            EOwnerThread );

    if (err == KErrNone)
        {
        err = batteryProperty.Get( aBatteryStatus );
        }
    IPTVLOGSTRING2_HIGH_LEVEL("CIptvBatteryWatcher::GetBatteryStatus %d", aBatteryStatus);
    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvBatteryWatcher::GetBatteryStatus");

    batteryProperty.Close();

    return err;
    }


// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::GetChargerStatus
// ---------------------------------------------------------------------------
//
TInt CIptvBatteryWatcher::GetChargerStatus( TInt& aChargerStatus )
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvBatteryWatcher::GetChargerStatus");

    RProperty chargingProperty;

    TInt err = chargingProperty.Attach(
            KPSUidHWRMPowerState,
            KHWRMChargingStatus,
            EOwnerThread );

    if (err == KErrNone)
        {
        err = chargingProperty.Get( aChargerStatus );
        }

    IPTVLOGSTRING2_HIGH_LEVEL("CIptvBatteryWatcher::GetChargerStatus %d", aChargerStatus);
    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvBatteryWatcher::GetChargerStatus");

    chargingProperty.Close();

    return err;
    }


// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::IsLowPower
// ---------------------------------------------------------------------------
//
TBool CIptvBatteryWatcher::IsLowPower()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvBatteryWatcher::IsLowPower");

    TInt batteryStatus = EBatteryStatusUnknown;
    TBool lowBattery = EFalse;

    // Try to get the battery status
    GetBatteryStatus(batteryStatus);

    // If it is not ok or we didn't get it, do additional stuff
    if (batteryStatus != EBatteryStatusOk)
        {
        // Assume low or empty battery status
        // In empty case just inform observers
        lowBattery = ETrue;
        if (batteryStatus == EBatteryStatusLow)
            {
            //  Low battery, check the charging state also
            //  EBatteryStatusLow status is send multiple times
            TInt chargingStatus = EChargingStatusNotConnected;

            if (GetChargerStatus( chargingStatus ) == KErrNone)
                {
                switch (chargingStatus)
                    {
                    case EChargingStatusCharging:
                    case EChargingStatusAlmostComplete:
                    case EChargingStatusChargingComplete:
                    case EChargingStatusChargingContinued:
                        {
                        lowBattery = EFalse;
                        }
                        break;

                    default:
                        {
                        lowBattery = ETrue;
                        }
                        break;
                    }
                }
            }
        }

    IPTVLOGSTRING2_HIGH_LEVEL("CIptvBatteryWatcher::IsLowPower %d", lowBattery);
    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvBatteryWatcher::IsLowPower");

    return lowBattery;
    }


// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::AddObserverL
// ---------------------------------------------------------------------------
//
void CIptvBatteryWatcher::AddObserverL( MBatteryWatcherObserver* aObserver )
    {
    iObservers.AppendL( aObserver );
    }

void CIptvBatteryWatcher::RemoveObserver( MBatteryWatcherObserver* aObserver )
    {
    TInt index = iObservers.Find( aObserver );
    if (index != KErrNotFound)
        {
        iObservers.Remove( index );
        }
    }


// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::RunL
//
// From class CActive.
// ---------------------------------------------------------------------------
//
void CIptvBatteryWatcher::RunL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvBatteryWatcher::RunL");

    // Resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();

    TBool lowBattery = IsLowPower();

    if (lowBattery)
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvBatteryWatcher::RunL low battery and no charger");

        for (TInt i = 0; i < iObservers.Count(); i++)
            {
            iObservers[i]->BatteryLow();
            }
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvBatteryWatcher::RunL");
    }

// ---------------------------------------------------------------------------
// CIptvBatteryWatcher::DoCancel
// 
// From class CActive.
// ---------------------------------------------------------------------------
//
void CIptvBatteryWatcher::DoCancel()
    {
    iProperty.Cancel();
    }

