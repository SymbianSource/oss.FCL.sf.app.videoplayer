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
* Description: 
*
*/



// INCLUDE FILES
#include "tvideosettingsdrivemonitorevent.h"

_LIT( KIptvEmptyDes, "" );

#if IPTV_LOGGING_METHOD != 0 
_LIT(KIptvDmeMediaRemovedDes, "EMediaRemoved");
_LIT(KIptvDmeMediaInsertedDes, "EMediaInserted");
_LIT(KIptvDmeDriveLockedDes, "EDriveLocked");
_LIT(KIptvDmeDriveUnlockedDes, "EDriveUnlocked");
_LIT(KIptvDmeDriveMountedDes, "EDriveMounted");
_LIT(KIptvDmeDriveDismountedDes, "EDriveDismounted");
_LIT(KIptvDmeDriveVolumeNameChangedDes, "EDriveVolumeNameChanged");
_LIT(KIptvDmeUnknownDes, "Unknown");
#endif

// ============================ MEMBER FUNCTIONS ===============================

EXPORT_C TIptvDriveMonitorEvent::TIptvDriveMonitorEvent()
    : iEvent( ENone )
    , iDrive( -1 )
    {
    }

// -----------------------------------------------------------------------------
// TIptvDriveMonitorEvent::EventDes()
// -----------------------------------------------------------------------------
//
#if IPTV_LOGGING_METHOD != 0

const TDesC& TIptvDriveMonitorEvent::EventDes( TIptvDriveMonitorEvent::TEvent aEvent )
    {
    switch ( aEvent )
        {
        case EMediaRemoved:
            return KIptvDmeMediaRemovedDes;
            
        case EMediaInserted:
            return KIptvDmeMediaInsertedDes;

        case EDriveLocked:
            return KIptvDmeDriveLockedDes;

        case EDriveUnlocked:
            return KIptvDmeDriveUnlockedDes;

        case EDriveMounted:
            return KIptvDmeDriveMountedDes;

        case EDriveDismounted:
            return KIptvDmeDriveDismountedDes;
        
        case EDriveVolumeNameChanged:
            return KIptvDmeDriveVolumeNameChangedDes;

        default:
            return KIptvDmeUnknownDes;
        }
    }
    
#else

const TDesC& TIptvDriveMonitorEvent::EventDes( TIptvDriveMonitorEvent::TEvent /*aEvent*/ )
    {
    return KIptvEmptyDes;
    }

#endif

