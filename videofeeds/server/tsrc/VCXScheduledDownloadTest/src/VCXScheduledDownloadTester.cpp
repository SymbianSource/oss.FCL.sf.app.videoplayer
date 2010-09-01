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
#include "VCXScheduledDownloadTester.h"
#include "VCXTestMyVideosCollectionClient.h"
#include "VCXTestLog.h"

#include <ipvideo/CCseSchedulerAPI.h>
#include <ipvideo/CCseScheduledProgram.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::NewL
// -----------------------------------------------------------------------------
//
CVCXScheduledDownloadTester* CVCXScheduledDownloadTester::NewL( CVCXTestStifScriptBaseExt* aStifScriptBase )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::NewL");
    CVCXScheduledDownloadTester* self = new (ELeave) CVCXScheduledDownloadTester( aStifScriptBase );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::~CVCXScheduledDownloadTester
// -----------------------------------------------------------------------------
//
CVCXScheduledDownloadTester::~CVCXScheduledDownloadTester()
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::~CVCXScheduledDownloadTester");
    
    delete iMyVideosCollectionClient;
    iMyVideosCollectionClient = NULL;
    
    delete iMessageWait;
    iMessageWait = NULL;
    
    delete iSchedulerClient;
    iSchedulerClient = NULL;
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::~CVCXScheduledDownloadTester");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::CVCXScheduledDownloadTester
// -----------------------------------------------------------------------------
//
CVCXScheduledDownloadTester::CVCXScheduledDownloadTester( CVCXTestStifScriptBaseExt* aStifScriptBase )
 : iStifScriptBase( aStifScriptBase )
    {
    
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::ConstructL()
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::ConstructL");
    iMessageWait = CVCXTestMessageWait::NewL( this );
    
    iMyVideosCollectionClient = CVCXTestMyVideosCollectionClient::NewL( this );

    // Wait for video list to open.
    iMessageWait->WaitForMessageL( EVCXTestVideoListRefreshed, 30, ETrue );
    
    iSchedulerClient = CCseSchedulerApi::NewL();

    VCXLOGLO1("<<<CVCXScheduledDownloadTester::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::RefreshL
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::RefreshL()
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::RefreshL");
    
    iMyVideosCollectionClient->RefreshCollectionL();
    iMessageWait->AddMessage( EVCXTestVideoListRefreshed );
    iMessageWait->WaitForAllL( 60, ETrue );
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::RefreshL");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::RemoveAllMediasL
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::RemoveAllMediasL()
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::RemoveAllMediasL");
    
    iMyVideosCollectionClient->EnsureMediaFilesAreNotInUseL();
    
    TBool retries = 3;
    TInt err( KErrNone );
    TBool removalFailed( EFalse );

    TInt videoCount = iMyVideosCollectionClient->VideoCount();
    TInt downloadCount = iMyVideosCollectionClient->DownloadCount();
    
    VCXLOGLO2("CVCXScheduledDownloadTester:: videoCount: %d", videoCount );
    VCXLOGLO2("CVCXScheduledDownloadTester:: downloadCount: %d", downloadCount );
    
    do
        {
        TInt successCount = 0;
        
        for( TInt i = 0; i < videoCount; i++ )
            {
            TRAP( err, RemoveVideoL( 0 ) );
            if( err != KErrNone )
                {
                VCXLOGLO2("CVCXScheduledDownloadTester:: removing of video left with %d", err );
                removalFailed = ETrue;
                }
            else
                {
                successCount++;
                }
            }

        videoCount -= successCount;
        successCount = 0;
        
        if( err == KErrNone )
            {
            for( TInt i = 0; i < downloadCount; i++ )
                {
                TRAP( err, CancelDownloadL( 0 ) );
                if( err != KErrNone )
                    {
                    VCXLOGLO2("CVCXScheduledDownloadTester:: cancel download left with %d", err );
                    removalFailed = ETrue;
                    }
                else
                    {
                    successCount++;
                    }
                }
            }
        
        downloadCount -= successCount;
        
        // If something failed, wait minute before retry.
        if( removalFailed )
            {
            User::After( 60 * 1000000 );
            }   
        }
        while( --retries > 0 && removalFailed );
    
    User::LeaveIfError( err );
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::RemoveAllMediasL");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::RemoveVideoL
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::RemoveVideoL( TUint aIndex )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::RemoveVideoL");
    
    iMyVideosCollectionClient->RemoveVideoL( aIndex );
    iMessageWait->AddMessage( EVCXTestVideoVideoDeleted );
    iMessageWait->AddMessage( EVCXTestVideoListRefreshed );
    iMessageWait->WaitForAllL( 60, ETrue );
    
    iMyVideosCollectionClient->RefreshCollectionL();
    iMessageWait->AddMessage( EVCXTestVideoListRefreshed );
    iMessageWait->WaitForAllL( 60, ETrue );
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::RemoveVideoL");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::DownloadL
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::DownloadL( const TDesC& aUrl, TUint aIapId, const TDesC& aUserName, const TDesC& aPassword )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::DownloadL");
    
    iMessageWait->Reset();
    iMyVideosCollectionClient->DownloadL( aUrl, aIapId, aUserName, aPassword );
    iMessageWait->AddMessage( EVCXTestVideoDownloadStarted );
    iMessageWait->AddMessage( EVCXTestVideoDownloadCompleted );
    iMessageWait->WaitForAllL( 4*60, ETrue );
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::DownloadL");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::CancelDownloadL
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::CancelDownloadL( TUint aIndex )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::CancelDownloadL");
    
    iMessageWait->Reset();
    iMyVideosCollectionClient->CancelDownloadL( aIndex );
    
    iMessageWait->AddMessage( EVCXTestVideoVideoDeleted );
    iMessageWait->WaitForAllL( 60, ETrue );

    iMyVideosCollectionClient->RefreshCollectionL();
    iMessageWait->AddMessage( EVCXTestVideoListRefreshed );
    iMessageWait->WaitForAllL( 60, ETrue );
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::CancelDownloadL");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::WaitForVideoEventsL
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::WaitForVideoEventsL( TInt aTimeoutSeconds, RArray<TInt32> aEvents )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::WaitForVideoEventsL");

    iMessageWait->Reset();
    for( TInt i = 0; i < aEvents.Count(); i++ )
        {
        VCXLOGLO2("CVCXScheduledDownloadTester:: message: %d", aEvents[i]);
        iMessageWait->AddMessage( aEvents[i] );
        }
    iMessageWait->WaitForAllL( aTimeoutSeconds, ETrue );
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::WaitForVideoEventsL");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::CoolDown
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::CoolDown()
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::CoolDown");

    iMessageWait->CoolDownL( 30 );
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::CoolDown");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::VideoCount
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTester::VideoCount()
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::VideoCount");
    TInt count = iMyVideosCollectionClient->VideoCount();
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::VideoCount");
    return count;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::DownloadCount
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTester::DownloadCount()
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::DownloadCount");
    TInt count = iMyVideosCollectionClient->DownloadCount();
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::DownloadCount");
    return count;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::RemoveAllSchedulesL
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::RemoveAllSchedulesL()
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::RemoveAllSchedulesL");

    TTimeIntervalYears years( 2 );
    
    TTime start;
    start.HomeTime();
    start -= years;
    
    TTime end;
    end.HomeTime();
    end += years;    
    
    RPointerArray<CCseScheduledProgram> programs;
    
    iSchedulerClient->GetSchedulesByTime( start, end, programs );
    CleanupClosePushL( programs );
    
    for( TInt i = 0; i < programs.Count(); i++ )
        {
        User::LeaveIfError( iSchedulerClient->RemoveSchedule( programs[i]->DbIdentifier() ) );
        }

    CleanupStack::PopAndDestroy( &programs );
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::RemoveAllSchedulesL");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::GetLastError
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTester::GetLastError()
    {
    return iError;
    }
    
// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::VideoEventL
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::VideoEventL( TVCXTestVideoEvent aEvent, 
        TUint32 aParam1, TUint32 aParam2, TInt aError )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTester::VideoEventL");
    
    iMessageWait->ReceiveMessage( aEvent, aError );
        
    switch( aEvent )
        {
        case EVCXTestVideoGeneral:
            {
            VCXLOGLO1("CVCXScheduledDownloadTester::VideoEventL: EVCXTestVideoGeneral =----->");            
            }
            break;
            
        case EVCXTestVideoListRefreshed:
            {
            VCXLOGLO1("CVCXScheduledDownloadTester::VideoEventL: EVCXTestVideoListRefreshed =----->");
            }
            break;

        case EVCXTestVideoVideoAdded:
            {
            VCXLOGLO2("CVCXScheduledDownloadTester::VideoEventL: EVCXTestVideoVideoAdded. Id: %d =----->", aParam1);
            }
            break;

        case EVCXTestVideoVideoDeleted:
            {
            VCXLOGLO2("CVCXScheduledDownloadTester::VideoEventL: EVCXTestVideoVideoDeleted. Id: %d =----->", aParam1);
            }
            break;

        case EVCXTestVideoDownloadStarted:
            {
            VCXLOGLO2("CVCXScheduledDownloadTester::VideoEventL: EVCXTestVideoDownloadStarted. Id: %d =----->", aParam1);
            }
            break;
            
        case EVCXTestVideoDownloadPaused:
            {
            VCXLOGLO2("CVCXScheduledDownloadTester::VideoEventL: EVCXTestVideoDownloadPaused. Id: %d =----->", aParam1);
            }
            break;

        case EVCXTestVideoDownloadFailed:
            {
            VCXLOGLO2("CVCXScheduledDownloadTester::VideoEventL: EVCXTestVideoDownloadFailed. Id: %d =----->", aParam1);
            }
            break;
        
        case EVCXTestVideoDownloadCompleted:
            {
            VCXLOGLO2("CVCXScheduledDownloadTester::VideoEventL: EVCXTestVideoDownloadCompleted. Id: %d =----->", aParam1);
            }
            break;
            
        case EVCXTestVideoDownloadProgressed:
            {
            VCXLOGLO3("CVCXScheduledDownloadTester::VideoEventL: EVCXTestVideoDownloadProgressed. Id: %d, progress: %d =----->", aParam1, aParam2);
            }
            break;

        default:
            break;
        }
    
    if( aError != KErrNone )
        {
        VCXLOGLO2("CVCXScheduledDownloadTester::VideoEventL: aError: %d =----->", aError);
        iError = aError;
        }
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTester::VideoEventL");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTester::MessageWaitComplete
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTester::MessageWaitComplete( TInt aError )
    {
    if( aError == KErrTimedOut )
        {
        VCXLOGLO1("CVCXScheduledDownloadTester:: Message wait TIMEOUT!");
        iStifScriptBase->Signal( aError );
        }
    }
