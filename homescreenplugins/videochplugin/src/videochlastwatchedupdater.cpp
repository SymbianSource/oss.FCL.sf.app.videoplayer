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
* Description:    CVideoChLastWatchedUpdater class definition*
*/



// INCLUDE FILES
#include <e32std.h>
#include <e32def.h>
#include <e32cmn.h>

#include <videoplayercustommessage.h>

#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>
#include <mpxcommandgeneraldefs.h>

#include "IptvDebug.h"
// #include "iptvlastwatchedapi.h"
// #include "iptvlastwatcheddata.h"
#include "videochlastwatchedupdater.h"
#include "videochpublishabledata.h"
#include "videochpublisher.h"
#include "vcxnsmediatorids.h" 
#include "videochmcategorylistener.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CVcxNsChLastWatchedUpdater::CVcxNsChLastWatchedUpdater(  CVcxNsChPublishableData* aPublishableData,
                                                         CVcxNsChPublisher* aPublisher ) :
     iPublishableData( aPublishableData ),
     iPublisher( aPublisher )
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxNsChLastWatchedUpdater::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsChLastWatchedUpdater::ConstructL IN");
    

    TVersion mediatorVcxNsPlayerVersion = TVersion( KVcxNsMediatorPlayerVersion, 0, 0 );   
    TUid domain = KVcxNsVideoCenterMediatorDomain;
    TUid category = KVcxNsPlayerGeneralCategory;
    iMPlayerCategoryListener = CVcxNsChMCategoryListener::NewL( this, 
                                                                domain,
                                                                category,
                                                                mediatorVcxNsPlayerVersion );

    iMPlayerCategoryListener->RegisterAndSubscribeL( KVcxNsMediatorEventPlayerLastWatch );
    iMPlayerCategoryListener->RegisterAndSubscribeL( KVcxNsMediatorEventPreloadedLastWatch );
    
    domain = KVcxNsVideoCenterMediatorDomain;
    category = KVcxNsIPTVGeneralCategory;
    TVersion mediatorVcxNsIPTVVersion = TVersion( KVcxNsMediatorIPTVVersion, 0, 0 );
    iMIPTVCategoryListener = CVcxNsChMCategoryListener::NewL( this, 
                                                              domain,
                                                              category,
                                                              mediatorVcxNsIPTVVersion );
    iMIPTVCategoryListener->RegisterAndSubscribeL( KVcxNsMediatorEventIPTVLastWatch );
       
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsChLastWatchedUpdater::ConstructL OUT");
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxNsChLastWatchedUpdater* CVcxNsChLastWatchedUpdater::NewL( 
                                     CVcxNsChPublishableData* aPublishableData,
                                     CVcxNsChPublisher* aPublisher )
    {
    
    CVcxNsChLastWatchedUpdater* self = new ( ELeave ) CVcxNsChLastWatchedUpdater( 
                                                                     aPublishableData,
                                                                     aPublisher  );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxNsChLastWatchedUpdater::~CVcxNsChLastWatchedUpdater()
    {   
    delete iMPlayerCategoryListener;
    delete iMIPTVCategoryListener;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CVcxNsChLastWatchedUpdater::MediatorEventL( TUid aDomain,
                                                 TUid aCategory,
                                                 TInt aEventId,
                                                 const TDesC8& aData )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsChLastWatchedUpdater::MediatorEventL IN");
    
    if( aDomain == KVcxNsVideoCenterMediatorDomain )
        {
        
        if( aCategory == KVcxNsPlayerGeneralCategory && 
            aEventId  == KVcxNsMediatorEventPlayerLastWatch )
            {       
            // get data and publish to menu
            TVideoPlayerCustomMessage* message = new (ELeave) TVideoPlayerCustomMessage;
            CleanupStack::PushL( message );
            TPckg< TVideoPlayerCustomMessage > paramsPckg( *message );
                        
            paramsPckg.Copy( aData );
            
            // save last watched name
            iPublishableData->SetDataL( EVCxNsCPContentLastWatched, 
                                        TPtrC8( KTvVideoName ),
                                        message->iName );

            // save last watched path
            iPublishableData->SetDataL( EVCxNsCPContentLastWatched, 
                                        TPtrC8( KTvVideoPath ),
                                        message->iContent );
            
            // save possible icon
            iPublishableData->SetDataL( EVCxNsCPContentLastWatched, 
                                        TPtrC8( KTvVideoIconPath ),
                                        message->iIcon );
            
            // save last watched content type, all other than IPTV
            // can use "last watched" content type
            iPublishableData->SetDataL( EVCxNsCPContentLastWatched, 
                                        TPtrC8( KTvVideoLastWatchedType ),
                                        EVCxNsCPContentLastWatched );
            
            CleanupStack::PopAndDestroy( message );
            
            }
        else if( aCategory == KVcxNsPlayerGeneralCategory && 
                 aEventId  == KVcxNsMediatorEventPreloadedLastWatch )
            {
            WritePreloadedToLastWatchedL();
            
            TPtrC name = 
                    iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                                    TPtrC8( KTvVideoName ) );
            // save last watched name
            iPublishableData->SetDataL( EVCxNsCPContentLastWatched,
                                        TPtrC8( KTvVideoName ),
                                        name );
           
            TPtrC iconPath =
                    iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                                    TPtrC8( KTvVideoIconPath ) );
            // save possible icon
            iPublishableData->SetDataL( EVCxNsCPContentLastWatched,
                                        TPtrC8( KTvVideoIconPath ),
                                        iconPath ); 
            
            // save last watched content type, all other than IPTV
            // can use "last watched" content type
            iPublishableData->SetDataL( EVCxNsCPContentLastWatched, 
                                        TPtrC8( KTvVideoLastWatchedType ),
                                        EVCxNsCPContentLastWatched );
            }
        else if( aCategory == KVcxNsIPTVGeneralCategory && 
                 aEventId  == KVcxNsMediatorEventIPTVLastWatch )
            {
            TIPTVMediatorMsg message;
            TPckg< TIPTVMediatorMsg > paramsPckg( message );
            paramsPckg.Copy( aData );
            
            // save last watched name
            iPublishableData->SetDataL( EVCxNsCPContentLastWatched,
                                        TPtrC8( KTvVideoName ),
                                        message.iChannelName );
                       
 
             // save possible icon
             iPublishableData->SetDataL( EVCxNsCPContentLastWatched,
                                         TPtrC8( KTvVideoIconPath ),
                                         message.iIconPath ); 


            // save last watched type 
             iPublishableData->SetDataL( EVCxNsCPContentLastWatched, 
                                         TPtrC8( KTvVideoLastWatchedType ),
                                         EVCxNsCPContentIPTV );
            }
        // publish data
        iPublisher->PublishContentL( EVCxNsCPContentLastWatched ); 
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsChLastWatchedUpdater::MediatorEventL OUT");
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CVcxNsChLastWatchedUpdater:: WritePreloadedToLastWatchedL()
    {
    /*
    CIptvLastWatchedData* data =  CIptvLastWatchedData::NewL();
    CleanupStack::PushL(data);
    
    TPtrC name =
            iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                            TPtrC8( KTvVideoName ) );
    TPtrC path =
            iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                            TPtrC8( KTvVideoPath ) );
    TPtrC iconPath = 
            iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                            TPtrC8( KTvVideoIconPath ) );
    
    data->SetIconPathL( iconPath );
    data->SetNameL( name );
    data->SetUriL( path ); 
    data->SetContentType( KViaPlayerContentTypeFile );
    data->SetMpxId( KIdUndefined );
    
    
    iPublishableData->LastWatchedApi().SetLastWatchedDataL( *data );
    CleanupStack::PopAndDestroy(data);
    */
    }
