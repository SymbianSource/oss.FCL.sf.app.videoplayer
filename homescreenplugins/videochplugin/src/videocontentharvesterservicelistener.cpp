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
* Description: 
*
*/


#include <e32std.h>
#include <e32base.h>

#include "IptvDebug.h"
#include "iptvactivespacepskeys.h"

#include "videocontentharvesterplugin.h"
#include "videocontentharvesterservicelistener.h"
#include "videochpublishabledata.h"
#include "videochpublisher.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CVcxNsContentHarvesterServiceListener::CVcxNsContentHarvesterServiceListener(
        CVcxNsChPublisher* aPublisher,
        CVcxNsChPublishableData* aPublishableData ) :
                                                                  
    iPublisher( aPublisher ),
    iPublishableData( aPublishableData )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsContentHarvesterServiceListener::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterServiceListener::ConstructL IN");

    iVodServiceCount = CVcxNsCHPSSubscriber::NewL( KPSUidIptvServer, KIptvAsVodCastFeedCount,  
                                                    RProperty::EInt, this );
    iVodServiceCount->Start();
    
    iLiveTvServiceCount = CVcxNsCHPSSubscriber::NewL( KPSUidIptvServer, KIptvLiveTVServiceCount,  
                                                        RProperty::EInt, this );
    iLiveTvServiceCount->Start();
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterServiceListener::ConstructL OUT");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CVcxNsContentHarvesterServiceListener* CVcxNsContentHarvesterServiceListener::NewL(
        CVcxNsChPublisher* aPublisher,
        CVcxNsChPublishableData* aPublishableData )
    {
    CVcxNsContentHarvesterServiceListener* self = 
        new( ELeave ) CVcxNsContentHarvesterServiceListener( aPublisher, aPublishableData );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CVcxNsContentHarvesterServiceListener::~CVcxNsContentHarvesterServiceListener()
     {
     delete iVodServiceCount;
     delete iLiveTvServiceCount;     
     }

// ---------------------------------------------------------------------------
// CVcxNsContentHarvesterServiceListener::ValueChanged
// From class MCHPSObserver.
// ---------------------------------------------------------------------------
//
void CVcxNsContentHarvesterServiceListener::ValueChanged( 
        const TUint32& aKey, 
        const TInt& aValue  )
    {
    if( aKey == KIptvAsVodCastFeedCount )
        {
        TRAP_IGNORE( iPublishableData->SetDataL( EVCxNsCPContentFeeds,
                                                 TPtrC8( KTvVideoCount ),
                                                 aValue ) );
        
        TRAP_IGNORE( iPublisher->PublishContentL( EVCxNsCPContentFeeds ) );
        }
        
    // NOTE: LiveTV and IPTV means same thing in this context
    else if( aKey == KIptvLiveTVServiceCount )
        {
        TRAP_IGNORE( iPublishableData->SetDataL( EVCxNsCPContentIPTV,
                                                 TPtrC8( KTvVideoCount ),
                                                 aValue ) );
        if( aValue )
            {
            TRAP_IGNORE( iPublisher->PublishContentL( EVCxNsCPContentIPTV ) );
            }
        else
            {
            TRAP_IGNORE( iPublisher->DeleteContentL( EVCxNsCPContentIPTV ) );
            iPublishableData->ClearData( EVCxNsCPContentIPTV );
            }
        }    
    }

