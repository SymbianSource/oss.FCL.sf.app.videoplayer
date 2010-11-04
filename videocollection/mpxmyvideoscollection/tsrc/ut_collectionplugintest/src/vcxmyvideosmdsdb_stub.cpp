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
* Description:    MPX My Videos collection plugin's MDS database class*
*/




//#include <harvesterclient.h>
//#include <mpxlog.h>
#include <mdequery.h>
#include <mdeconstants.h>
#include <mdequery.h>
#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxmediaarray.h>
#include <mpxmediavideodefs.h>
#include "vcxmyvideosmdsdb_stub.h"
#include "vcxmyvideoscollectionutil.h"


const TInt KInvalidId( -100 );
const TInt KMPXIdNotFound( 998 );

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::NewL()
// ---------------------------------------------------------------------------
//
CVcxMdsShutdownMonitor* CVcxMdsShutdownMonitor::NewL( MVcxMdsShutdownMonitorObserver& aObserver,
                                                const TUid& aKeyCategory,
                                                const TInt aPropertyKey,
                                                TBool aDefineKey)
    {
    CVcxMdsShutdownMonitor* self = new( ELeave )CVcxMdsShutdownMonitor( aObserver, 
                                                                  aKeyCategory,
                                                                  aPropertyKey,
                                                                  aDefineKey);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::CVcxMdsShutdownMonitor()
// ---------------------------------------------------------------------------
//
CVcxMdsShutdownMonitor::CVcxMdsShutdownMonitor( MVcxMdsShutdownMonitorObserver& aObserver,
                                          const TUid& aKeyCategory,
                                          const TInt aPropertyKey,
                                          TBool aDefineKey)
    : CActive( CActive::EPriorityStandard ), iObserver( aObserver ),
      iKeyCategory( aKeyCategory ), iPropertyKey(aPropertyKey), iDefineKey( aDefineKey )
    {   
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::ConstructL()
// ---------------------------------------------------------------------------
//
void CVcxMdsShutdownMonitor::ConstructL()
    { 
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::~CVcxMdsShutdownMonitor()
// ---------------------------------------------------------------------------
//
CVcxMdsShutdownMonitor::~CVcxMdsShutdownMonitor()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::RunL()
// ---------------------------------------------------------------------------
//
void CVcxMdsShutdownMonitor::RunL()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMdsShutdownMonitor::DoCancel()
// ---------------------------------------------------------------------------
//
void CVcxMdsShutdownMonitor::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::CVcxMyVideosMdsDb
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsDb::CVcxMyVideosMdsDb( MVcxMyVideosMdsDbObserver* aObserver,
        RFs& aFs, MVcxMyVideosMdsAlbumsObserver* aAlbumsObserver )
: iFs( aFs ), iMdsDbObserver(aObserver), iAlbumsObserver(aAlbumsObserver) 
    {
    }


// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::ConstructL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::ConstructL()
    {
    //iCmdQueue = CVcxMyVideosMdsCmdQueue::NewL( *this );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::OpenMdsSessionL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::OpenMdsSessionL()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleObjectPresentNotification( CMdESession& /*aSession*/,
        TBool /*aPresent*/, const RArray<TItemId>& /*aObjectIdArray*/)
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsDb* CVcxMyVideosMdsDb::NewL( MVcxMyVideosMdsDbObserver* aObserver,
        RFs& aFs, MVcxMyVideosMdsAlbumsObserver* aAlbumsObserver )
    {
    CVcxMyVideosMdsDb* self =
            CVcxMyVideosMdsDb::NewLC( aObserver, aFs, aAlbumsObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsDb* CVcxMyVideosMdsDb::NewLC( MVcxMyVideosMdsDbObserver* aObserver,
        RFs& aFs, MVcxMyVideosMdsAlbumsObserver* aAlbumsObserver )
    {
    CVcxMyVideosMdsDb* self = new( ELeave ) CVcxMyVideosMdsDb( aObserver, aFs, aAlbumsObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsDb::~CVcxMyVideosMdsDb()
    {
    delete iVideoQuery;
    delete iMdsSession;
    delete iActiveSchedulerWait;
    delete iAsyncHandleQueryCompleteCaller;
    delete iMdsShutdownMonitor;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::Cancel
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::Cancel( TRequestType /*aType*/ )
    {
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::AddVideoL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::AddVideoL(
        CMPXMedia& /*aVideo*/, 
        TUint32& /*aMdsId*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::RemoveVideo
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosMdsDb::RemoveVideo( TUint32 /*aMdsId*/ )
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::UpdateVideoL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::UpdateVideoL( CMPXMedia& /*aVideo*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::CreateVideoListL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::CreateVideoListL( TVcxMyVideosSortingOrder /*aSortingOrder*/,
        TBool /*aAscending*/, TBool /*aFullDetails*/, CMPXMedia*& /*aVideoList*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::DoCreateVideoListL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::DoCreateVideoListL( TVcxMyVideosSortingOrder /*aSortingOrder*/,
        TBool /*aAscending*/, TBool /*aFullDetails*/, CMPXMedia*& aVideoList )
    {
    iVideoList = aVideoList;
    CMPXMediaArray* mediaArray = iVideoList->Value<CMPXMediaArray>(KMPXMediaArrayContents);
    CleanupStack::PushL( mediaArray );
    for( TInt i = 0; i < 10; i++ )
        {
        TMPXItemId mpxId( i, 0 );

        CMPXMedia* newMedia = CMPXMedia::NewL();
        CleanupStack::PushL( newMedia );
        newMedia->SetTObjectValueL( KMPXMessageMediaGeneralId, mpxId );
        newMedia->SetTObjectValueL( KMPXMediaGeneralId, mpxId );
        mediaArray->AppendL( *newMedia );
        CleanupStack::PopAndDestroy( newMedia );
        }
    CleanupStack::Pop( mediaArray );
    
    iVideoList->SetTObjectValueL( KMPXMediaArrayCount, mediaArray->Count() );
    }

// ---------------------------------------------------------------------------
// From class MMdEQueryObserver.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleQueryNewResults(
        CMdEQuery& /*aQuery*/,
        TInt /*aFirstNewItemIndex*/,
        TInt /*aNewItemCount*/ )
    {
    }
    
// ---------------------------------------------------------------------------
// Leaving version of HandleQueryNewResults.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::DoHandleQueryNewResultsL(
        CMdEQuery& /*aQuery*/,
        TInt /*aFirstNewItemIndex*/,
        TInt /*aNewItemCount*/ )
    {
    }


            
// ---------------------------------------------------------------------------
// From class MMdEQueryObserver.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleQueryCompleted(
        CMdEQuery& /*aQuery*/,
        TInt /*aError*/)
    {
    //iAsyncHandleQueryCompleteCaller->CallBack();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::AsyncHandleQueryCompleted
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosMdsDb::AsyncHandleQueryCompleted( TAny* /*aThis*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::CreateVideoL
// ---------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosMdsDb::CreateVideoL( TUint32 aId, TBool /*aFullDetails*/ )
    {
    if( aId == KInvalidId )
        {
        return NULL;
        }
    
    if( aId == KMPXIdNotFound )
        {
        return NULL;
        }
    
    CMPXMedia* video = CMPXMedia::NewL( );
    CleanupStack::PushL( video );
    TMPXItemId id( aId, KVcxMvcMediaTypeVideo );
    video->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
    CleanupStack::Pop( video );
    return video;
    }

// ---------------------------------------------------------------------------
// From class MMdESessionObserver.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleSessionOpened(
        CMdESession& /*aSession*/,
        TInt /*aError*/)
    {
    }

// ---------------------------------------------------------------------------
// From class MMdESessionObserver.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleSessionError(
        CMdESession& /*aSession*/,
        TInt aError)
    {
    iMdsDbObserver->HandleCreateVideoListResp( iVideoList, aError, ETrue );
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::HandleObjectNotification
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::HandleObjectNotification(
        CMdESession& /*aSession*/,
        TObserverNotificationType /*aType*/,
        const RArray<TItemId>& /*aObjectIdArray*/)
    {
    //TRAP_IGNORE( DoHandleObjectNotificationL( aType, aObjectIdArray ));
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::ObjectL
// ---------------------------------------------------------------------------
//
CMdEObject* CVcxMyVideosMdsDb::ObjectL( const TItemId /*aId*/, TBool /*aIsVideo*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::Object2MediaL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::Object2MediaL(
        CMdEObject& /*aObject*/,
        CMPXMedia& /*aVideo*/,
        TBool /*aFullDetails*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::Media2ObjectL
// Called by AddVideoL()
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::Media2ObjectL(
        CMPXMedia& /*aVideo*/,
        CMdEObject& /*aObject*/)
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::SetCreationDateToObjectL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::SetCreationDateToObjectL( const CMPXMedia& /*aVideo*/, CMdEObject& /*aObject*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::GetSchemaDefinitionsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::GetSchemaDefinitionsL()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::DoHandleObjectNotificationL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::DoHandleObjectNotificationL(
        TObserverNotificationType /*aType*/,
        const RArray<TItemId>& /*aObjectIdArray*/ )
    {     
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::SetCreationAndModifiedDatesL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsDb::SetCreationAndModifiedDatesL( CMdEObject& /*aObject*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::MdsSessionL
// ---------------------------------------------------------------------------
//
CMdESession& CVcxMyVideosMdsDb::MdsSessionL()
    {
    return *iMdsSession;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsDb::ShutdownNotification
// From MVcxMdsShutDownMonitorObserver
// ---------------------------------------------------------------------------
//    
void CVcxMyVideosMdsDb::ShutdownNotification( TInt /*aShutdownState*/ )
    {
    }
