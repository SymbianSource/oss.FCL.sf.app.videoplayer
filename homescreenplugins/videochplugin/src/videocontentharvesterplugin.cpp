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
* Description:    CVcxNsContentHarvesterPlugin class definition*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32def.h>
#include <e32cmn.h>

#include <LiwServiceHandler.h>
#include <LiwVariant.h>
#include <LiwGenericParam.h>

#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>

#include <mpxcommandgeneraldefs.h>

#include "IptvDebug.h"


#include "videocontentharvesterplugin.h"
#include "videocontentharvestermpxclient.h"
#include "videocontentharvesterservicelistener.h"
#include "videonotificationlauncher.h"
#include "videochpublishabledata.h"
#include "videochpublisher.h"
#include "vcxnsmediatorids.h"
#include "videochmcategorylistener.h"
#include "videochiptvserverclient.h"
#include "videochlastwatchedupdater.h"
#include "iptvlastwatchedapi.h"
#include "iptvlastwatcheddata.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CVcxNsContentHarvesterPlugin::CVcxNsContentHarvesterPlugin( MLiwInterface* aCPSInterface ) :
    iCPSInterface( aCPSInterface )
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterPlugin::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterPlugin::ConstructL IN");
    
    iPublishableData = CVcxNsChPublishableData::NewL();
    
    iPublisher = CVcxNsChPublisher::NewL( iCPSInterface, iPublishableData );

    iMpxClient = CVcxNsContentHarvesterMpxClient::NewL( iPublisher, iPublishableData, this );

    iServiceListener = CVcxNsContentHarvesterServiceListener::NewL( iPublisher, iPublishableData );

    iNotificationLauncer = CVcxNsNotificationLauncher::NewL( iPublishableData, iPublisher );
    
    iLwUpdater = CVcxNsChLastWatchedUpdater::NewL( iPublishableData, iPublisher );
    
    iIptvServerClient = CVcxNsChIptvServerClient::NewL( iPublisher, iPublishableData );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterPlugin::ConstructL OUT");
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxNsContentHarvesterPlugin* CVcxNsContentHarvesterPlugin::NewL(  MLiwInterface* aCPSInterface )
    {
    CVcxNsContentHarvesterPlugin* self = new ( ELeave ) CVcxNsContentHarvesterPlugin( 
                                                                             aCPSInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxNsContentHarvesterPlugin::~CVcxNsContentHarvesterPlugin()
    {
    delete iIptvServerClient;
    delete iLwUpdater;      
    delete iNotificationLauncer;    
    delete iServiceListener;   
    delete iMpxClient; 
    delete iPublisher;
    delete iPublishableData;
    }

// ----------------------------------------------------------------------------
// CVcxNsContentHarvesterPlugin::UpdateL() 
// ----------------------------------------------------------------------------
//    
void CVcxNsContentHarvesterPlugin::UpdateL() 
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterPlugin::UpdateL in");
    // check and update data content that needs refreshing during boot.
    // in case something fails, there's nothing to do
    
    // last watched  
    UpdateLastWatchedL();
    
    // preloaded video
    UpdatePreloadedVideoL();
    
    // predefined service group
    iIptvServerClient->StartIPTVServerInterfaceL();
    TRAPD( err, UpdatePredefinedServiceL() );
    iIptvServerClient->StopIPTVServerInterface();
    User::LeaveIfError( err );

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterPlugin::UpdateL out");
    }

// ----------------------------------------------------------------------------
// CVcxNsContentHarvesterPlugin::UpdateLastWatchedL()        
// ----------------------------------------------------------------------------
//    
void CVcxNsContentHarvesterPlugin::UpdateLastWatchedL()
    {
    iPublisher->ReadContentL( EVCxNsCPContentLastWatched );
        
    TPtrC name = iPublishableData->GetDesCDataL( EVCxNsCPContentLastWatched,
                                                    TPtrC8( KTvVideoName ) );
    TInt lastWType = 
                    iPublishableData->GetIntDataL( EVCxNsCPContentLastWatched,
                                                     TPtrC8( KTvVideoLastWatchedType ) );
        
    if( name.Length() > 0 && lastWType == EVCxNsCPContentLastWatched )
        {
        // check last watched from the filesystem
        CIptvLastWatchedData* data = CIptvLastWatchedData::NewL();
        CleanupStack::PushL( data );
        iPublishableData->LastWatchedApi().GetLastWatchedDataL( *data );
		
        if( data->MpxId() != KIdUndefined &&
            !iPublishableData->FileExists( data->Uri() ) )
            {
            // clear and publish empty content
            iPublishableData->ClearData( EVCxNsCPContentLastWatched );
            iPublisher->PublishContentL( EVCxNsCPContentLastWatched );
            }
        CleanupStack::PopAndDestroy( data );
        }
    else
        {
        // clear and publish empty content
        iPublishableData->ClearData( EVCxNsCPContentLastWatched );
        iPublisher->PublishContentL( EVCxNsCPContentLastWatched );
        }         
    }
        
// ----------------------------------------------------------------------------
// CVcxNsContentHarvesterPlugin::UpdatePreloadedVideoL()        
// ----------------------------------------------------------------------------
// 
void CVcxNsContentHarvesterPlugin::UpdatePreloadedVideoL()
    {
    // read preloaded content from the CP and from the filesystem  
    iPublisher->ReadContentL( EVCxNsCPContentPreloaded );
    
    TPtrC preName = 
                iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                                TPtrC8( KTvVideoName ) ); 
    TPtrC prePath =
                iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                                TPtrC8( KTvVideoPath ) ); 
    
    if( preName.Length() == 0 || 
        !iPublishableData->FileExists( prePath ) ) 
        {
        // file does not exists, remove it's data from the CP so
        // that item is not shown
        iPublisher->DeleteContentL( EVCxNsCPContentPreloaded );
        }              
    }
        
// ----------------------------------------------------------------------------
// CVcxNsContentHarvesterPlugin::UpdatePredefinedServiceL()        
// ----------------------------------------------------------------------------
// 
void CVcxNsContentHarvesterPlugin::UpdatePredefinedServiceL()
    {
    iPublisher->ReadContentL( EVCxNsCPContentServiceGroup );

    TPtrC serviceUri = iPublishableData->GetDesCDataL( EVCxNsCPContentServiceGroup,
                                                                TPtrC8( KTvVideoPath ) ); 
    TInt id = iIptvServerClient->GetServiceGroupIdL( serviceUri ); 
        
    if(id != KErrNotFound )
        {                 
        // save ID and publish
        iPublishableData->SetDataL( EVCxNsCPContentServiceGroup,
                                                TPtrC8( KTvVideoId ), id);
        iPublisher->PublishContentL( EVCxNsCPContentServiceGroup );
        }
    else 
        {   
        // Restore backed up data
        iPublisher->RestoreBackupL( EVCxNsCPContentServiceGroup );            
        }
    }
    
// ----------------------------------------------------------------------------
// CVcxNsContentHarvesterPlugin::NotificationLauncher()        
// ----------------------------------------------------------------------------
// 
CVcxNsNotificationLauncher* CVcxNsContentHarvesterPlugin::NotificationLauncher()
    {
    return iNotificationLauncer;
    }

