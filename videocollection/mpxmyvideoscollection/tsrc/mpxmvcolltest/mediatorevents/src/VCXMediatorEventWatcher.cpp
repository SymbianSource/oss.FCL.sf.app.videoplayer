/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDES
#include <vcxmyvideosdefs.h>

#include <MediatorEventConsumer.h>
#include <mediatoreventprovider.h>
#include <mediatorservicedefs.h>

#include "VCXMediatorEventWatcher.h"
#include "VCXTestMyVideosCollectionClient.h"
#include "VCXTestCommon.h"
#include "VCXTestLog.h"
#include "CIptvTestActiveWait.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::NewL
// -----------------------------------------------------------------------------
//
CVCXMediatorEventWatcher* CVCXMediatorEventWatcher::NewL()
    {
    CVCXMediatorEventWatcher* self = new (ELeave) CVCXMediatorEventWatcher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::~CVCXMediatorEventWatcher
// -----------------------------------------------------------------------------
//
CVCXMediatorEventWatcher::~CVCXMediatorEventWatcher()
     {
     VCXLOGLO1(">>>CVCXMediatorEventWatcher::~CVCXMediatorEventWatcher");
     
     delete iMyVideosCollectionClient;
     iMyVideosCollectionClient = NULL;
     
     delete iVideoMessageWait;
     iVideoMessageWait = NULL;
     
     delete iMediatorEventWait;
     iMediatorEventWait = NULL;
     
     UnsubscribeAndDeregisterEvents();
     
     delete iMediatorEventConsumer;
     iMediatorEventConsumer = NULL;
     
     iSubscribedEvents.Close();
     iExpectedEvents.Close();
     
     VCXLOGLO1("<<<CVCXMediatorEventWatcher::~CVCXMediatorEventWatcher");
     }

 // -----------------------------------------------------------------------------
 // CVCXMediatorEventWatcher::CVCXMediatorEventWatcher
 // -----------------------------------------------------------------------------
 //
CVCXMediatorEventWatcher::CVCXMediatorEventWatcher()
    {
    
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXMediatorEventWatcher::ConstructL( )
    {
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::ConstructL");
    
    iVideoMessageWait = CVCXTestMessageWait::NewL( this );
    iMediatorEventWait = CVCXTestMessageWait::NewL( this );
     
    iMyVideosCollectionClient = CVCXTestMyVideosCollectionClient::NewL( this );

    // Wait for video list to open.
    iVideoMessageWait->WaitForMessageL( EVCXTestVideoListRefreshed, 30, ETrue );
    
    iLastNewVideoCount = iMyVideosCollectionClient->GetCountOfNewVideos( ETrue );
    
    // Start listening mediator events.
    
    iMediatorEventConsumer = CMediatorEventConsumer::NewL( this );
    
    iMediatorDomain   = TUid::Uid( KVcxNsMpxMediatorDomain );
    iMediatorCategory = TUid::Uid( KVcxNsMpxMediatorCategory );
    iMediatorVersion = TVersion( KVcxNsMpxEventVersion, 0, 0 );
    
    RegisterAndSubscribeEventL( TInt( KVcxNsMpxEventDownloadStarted ) );
    RegisterAndSubscribeEventL( TInt( KVcxNsMpxEventDownloadCompleted ) );
    RegisterAndSubscribeEventL( TInt( KVcxNsMpxEventAllDownloadsEnded ) );
    RegisterAndSubscribeEventL( TInt( KVcxNsMpxEventNewVideosCountDecreased ) );
    
    VCXLOGLO1("<<<CVCXMediatorEventWatcher::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::AddExpectedEventL
// -----------------------------------------------------------------------------
//
void CVCXMediatorEventWatcher::AddExpectedEventL( TInt aEvent )
    {
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::AddExpectedEventL");
    iExpectedEvents.Append( aEvent );
    VCXLOGLO1("<<<CVCXMediatorEventWatcher::AddExpectedEventL");
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::CheckEventsL
// -----------------------------------------------------------------------------
//
void CVCXMediatorEventWatcher::CheckEventsL()
    {
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::CheckEventsL");
    
    TInt err( iError );
    
    if( iError == KErrCorrupt )
        {
        VCXLOGLO1("CVCXMediatorEventWatcher::CheckEventsL: Error. Check for KVcxNsMpxEventNewVideosCountDecreased has failed before.");
        }
    
    if( iDownloadStartedEventCount > 0 )
        {
        VCXLOGLO2("CVCXMediatorEventWatcher::CheckEventsL: Error. 'Download started' events do not match from mpx plugin and mediator: %d", iDownloadStartedEventCount);
        err = KErrCorrupt;
        }
    
    if( iDownloadCompletedEventCount > 0 )
        {
        VCXLOGLO2("CVCXMediatorEventWatcher::CheckEventsL: Error. 'Download completed' events do not match from mpx plugin and mediator: %d", iDownloadCompletedEventCount);
        err = KErrCorrupt;
        }
    
    if( iAllDownloadsCompletedEventCount > 0 )
        {
        VCXLOGLO2("CVCXMediatorEventWatcher::CheckEventsL: Error. 'All downloads completed' events do not match from mpx plugin and mediator: %d", iDownloadCompletedEventCount);
        err = KErrCorrupt;
        }
    
    if( iExpectedEvents.Count() > 0 )
        {
        VCXLOGLO1("CVCXMediatorEventWatcher::CheckEventsL: Error. Did not get all expected events.");
        for( TInt i = 0; i < iExpectedEvents.Count(); i++ )
            {
            VCXLOGLO2("CVCXMediatorEventWatcher::CheckEventsL: Event not received: %d", iExpectedEvents[i]);
            }
        err = KErrCorrupt;
        }
    
    iError = KErrNone;
    
    User::LeaveIfError( err );

    VCXLOGLO1("<<<CVCXMediatorEventWatcher::CheckEventsL");
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::WaitEventsL
// -----------------------------------------------------------------------------
//
void CVCXMediatorEventWatcher::WaitEventsL( TInt aTimeoutSeconds )
    {
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::WaitEventsL");
    
    iError = KErrNone;
        
    if( iExpectedEvents.Count() > 0 )
        {
        for( TInt i = iExpectedEvents.Count(); --i >= 0; )
            {
            iMediatorEventWait->AddMessage( iExpectedEvents[i] );
            }
        
        iMediatorEventWait->WaitForAllL( aTimeoutSeconds, ETrue );    
        }
    
    User::LeaveIfError( iError );
    
    VCXLOGLO1("<<<CVCXMediatorEventWatcher::WaitEventsL");
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::RegisterAndSubscribeEventL
// -----------------------------------------------------------------------------
//
void CVCXMediatorEventWatcher::RegisterAndSubscribeEventL( TInt aEvent )
    {
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::RegisterAndSubscribeEventL");
    
    TCapabilitySet caps = TCapabilitySet( ECapabilityReadUserData ); 
    CMediatorEventProvider* provider = CMediatorEventProvider::NewL();
    
    TInt ret = provider->RegisterEvent( iMediatorDomain, iMediatorCategory, aEvent, 
                                        iMediatorVersion, caps );
    delete provider;
         
    if( ret != KErrNone && ret != KErrMediatorEventAlreadyExists )
        {        
        User::Leave( ret );
        }
     
    User::LeaveIfError( iMediatorEventConsumer->SubscribeEvent( iMediatorDomain, 
                                                        iMediatorCategory,
                                                        aEvent, 
                                                        iMediatorVersion ) );

    iSubscribedEvents.AppendL( aEvent );        
    
    VCXLOGLO1("<<<CVCXMediatorEventWatcher::RegisterAndSubscribeEventL");
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::UnsubscribeAndDeregisterEvents
// -----------------------------------------------------------------------------
//
void CVCXMediatorEventWatcher::UnsubscribeAndDeregisterEvents( )
    {
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::UnsubscribeAndDeregisterEvents");
    
    CMediatorEventProvider* provider = NULL;
        
    TRAPD( err, provider = CMediatorEventProvider::NewL() );
    if( err != KErrNone )
        {
        return;
        }
    
    TInt count = iSubscribedEvents.Count(); 
    for( TInt i = 0; i < count; ++i )
        {
        iMediatorEventConsumer->UnsubscribeEvent( iMediatorDomain, 
                                          iMediatorCategory,
                                          iSubscribedEvents[ i ] );
        
        provider->UnregisterEvent( iMediatorDomain,
                                   iMediatorCategory,
                                   iSubscribedEvents[ i ] );        
        }
    delete provider;      
    
    VCXLOGLO1("<<<CVCXMediatorEventWatcher::UnsubscribeAndDeregisterEvents");
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::HasVideoCountDecreased
// -----------------------------------------------------------------------------
//
TBool CVCXMediatorEventWatcher::HasVideoCountDecreased( TBool aForceUpdate )
    {
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::HasVideoCountDecreased");
    
    // Don't check again if count has decreased earlier. 
    if( !iNewVideoCountHasDecreased )
        {
        if( aForceUpdate )
            {
            iMyVideosCollectionClient->RefreshCollectionL();
            iVideoMessageWait->WaitForMessageL( EVCXTestVideoListRefreshed, 30, ETrue );
            }
    
        // Check again here because if there was previous force update then this has been called
        // at refresh event.
        if( !iNewVideoCountHasDecreased )
            {
            TInt newVideoCount = iMyVideosCollectionClient->GetCountOfNewVideos( ETrue );
            
            VCXLOGLO3("CVCXMediatorEventWatcher::HasVideoCountDecreased: new videos count, last: %d, now: %d", iLastNewVideoCount, newVideoCount);
           
            iNewVideoCountHasDecreased = newVideoCount < iLastNewVideoCount;
            iLastNewVideoCount = newVideoCount;
            
            VCXLOGLO1("<<<CVCXMediatorEventWatcher::HasVideoCountDecreased");
            }
        }
    
    return iNewVideoCountHasDecreased;
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::MediatorEventL
// -----------------------------------------------------------------------------
//
void CVCXMediatorEventWatcher::MediatorEventL( TUid aDomain, TUid aCategory, TInt aEventId, const TDesC8& /* aData */ )
    {
    if( aDomain != TUid::Uid( KVcxNsMpxMediatorDomain ) || 
        aCategory != TUid::Uid( KVcxNsMpxMediatorCategory ) )
        {
        return;
        }
   
    iMediatorEventWait->ReceiveMessage( aEventId, KErrNone );
    
    switch( aEventId ) 
        {
        case KVcxNsMpxEventDownloadStarted:
            {
            VCXLOGLO1("CVCXMediatorEventWatcher::MediatorEventL: KVcxNsMpxEventDownloadStarted");
            iDownloadStartedEventCount--;
            }
            break;
            
        case KVcxNsMpxEventDownloadCompleted:
            {
            VCXLOGLO1("CVCXMediatorEventWatcher::MediatorEventL: KVcxNsMpxEventDownloadCompleted");
            iDownloadCompletedEventCount--;
            }
            break;
            
        case KVcxNsMpxEventAllDownloadsEnded:
            {
            VCXLOGLO1("CVCXMediatorEventWatcher::MediatorEventL: KVcxNsMpxEventAllDownloadsEnded");
            iAllDownloadsCompletedEventCount--;
            }
            break;            

        case KVcxNsMpxEventNewVideosCountDecreased:
            {
            VCXLOGLO1("CVCXMediatorEventWatcher::MediatorEventL: KVcxNsMpxEventNewVideosCountDecreased");
            
            HasVideoCountDecreased( ETrue );

            if( !iNewVideoCountHasDecreased && iLastNewVideoCount  > 0 )
                {
                VCXLOGLO1("CVCXMediatorEventWatcher::MediatorEventL: Error. Count of new videos has not decreased.");
                iError = KErrCorrupt;
                }
            
            iNewVideoCountHasDecreased = EFalse;
            }
            break;
            
        default:
            break;
        }
    
    for( TInt i = iExpectedEvents.Count(); --i >= 0; )
        {
        if( iExpectedEvents[i] == aEventId )
            {
            iExpectedEvents.Remove( i );
            }
        }
    
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::VideoEventL
// -----------------------------------------------------------------------------
//
void CVCXMediatorEventWatcher::VideoEventL( TVCXTestVideoEvent aEvent, TUint32 aParam1, TUint32 aParam2, TInt aError )
    {
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::VideoEventL");
        
    iVideoMessageWait->ReceiveMessage( aEvent, aError );
    
    switch( aEvent )
        {
        case EVCXTestVideoGeneral:
            {
            VCXLOGLO1("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoGeneral =----->");            
            }
            break;
            
        case EVCXTestVideoListRefreshed:
            {
            VCXLOGLO1("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoListRefreshed =----->");
            HasVideoCountDecreased( EFalse );
            }
            break;

        case EVCXTestVideoVideoAdded:
            {
            VCXLOGLO2("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoVideoAdded. Id: %d =----->", aParam1);
            }
            break;

        case EVCXTestVideoVideoDeleted:
            {
            VCXLOGLO2("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoVideoDeleted. Id: %d =----->", aParam1);
            HasVideoCountDecreased( EFalse );
            }
            break;

        case EVCXTestVideoDownloadStarted:
            {
            VCXLOGLO2("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoDownloadStarted. Id: %d =----->", aParam1);
            iDownloadStartedEventCount++;
            }
            break;
            
        case EVCXTestVideoDownloadPaused:
            {
            VCXLOGLO2("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoDownloadPaused. Id: %d =----->", aParam1);
            }
            break;

        case EVCXTestVideoDownloadFailed:
            {
            VCXLOGLO2("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoDownloadFailed. Id: %d =----->", aParam1);
            }
            break;
        
        case EVCXTestVideoDownloadCompleted:
            {
            VCXLOGLO2("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoDownloadCompleted. Id: %d =----->", aParam1);
            iDownloadCompletedEventCount++;
            }
            break;
            
        case EVCXTestVideoDownloadProgressed:
            {
            VCXLOGLO3("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoDownloadProgressed. Id: %d, progress: %d =----->", aParam1, aParam2);
            }
            break;
            
        case EVCXTestVideoAllDownloadsCompleted:
             {
             VCXLOGLO1("CVCXMediatorEventWatcher::VideoEventL: EVCXTestVideoAllDownloadsCompleted.");
             iAllDownloadsCompletedEventCount++;
             }
             break;            

        default:
            break;
        }
    
    if( aError != KErrNone )
        {
        VCXLOGLO2("CVCXMediatorEventWatcher::VideoEventL: aError: %d =----->", aError);
        }    
    
    VCXLOGLO1("<<<CVCXMediatorEventWatcher::VideoEventL");
    }

// -----------------------------------------------------------------------------
// CVCXMediatorEventWatcher::MessageWaitComplete
// -----------------------------------------------------------------------------
//
void CVCXMediatorEventWatcher::MessageWaitComplete( TInt aError )
    {
    if( aError )
        {
        VCXLOGLO1("CVCXMediatorEventWatcher:: Message wait TIMEOUT!");
        iError = KErrTimedOut;
        }
    }

// EOF
