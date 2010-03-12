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
* Description:   Handles collection Open operation related functionality.*
*/



// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include <mpxcollectionplugin.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>

#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideosopenhandler.h"
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideoscategories.h"
#include "vcxmyvideosmessagelist.h"
#include "vcxmyvideoscollectionutil.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosOpenHandler::CVcxMyVideosOpenHandler( CVcxMyVideosCollectionPlugin& aCollection,
        CVcxMyVideosVideoCache& aCache,
        CVcxMyVideosMdsDb& aMds )
: iCollection( aCollection ), iCache( aCache ), iMds( aMds )
    {
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::ConstructL()
    {
    iCategoryIdsBeingOpened.Reset();
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosOpenHandler* CVcxMyVideosOpenHandler::NewL( CVcxMyVideosCollectionPlugin& aCollection,
        CVcxMyVideosVideoCache& aCache,
        CVcxMyVideosMdsDb& aMds )
    {
    CVcxMyVideosOpenHandler* self = new(ELeave) CVcxMyVideosOpenHandler( aCollection,
            aCache, aMds );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosOpenHandler::~CVcxMyVideosOpenHandler()
    {
    iCategoryIdsBeingOpened.Close();
    
    TInt count = iVideoListsBeingOpened.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        delete iVideoListsBeingOpened[i];
        iVideoListsBeingOpened[i] = NULL; 
        }
    iVideoListsBeingOpened.Close();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::OpenL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::OpenL(
    const CMPXCollectionPath& aPath )
    {
    MPX_FUNC("CVcxMyVideosOpenHandler::OpenL");

    MPX_DEBUG1("CVcxMyVideosOpenHandler:: path before open:");
    MPX_DEBUG_PATH( aPath );

    switch ( aPath.Levels() )
        {
        case 1:
            {
            //we are at root level, return main level items (categories)
            
            // this is needed to update category counters
            TRAPD( err, iCache.CreateVideoListL() );
            
            if ( err != KErrNone )
                {
                MPX_DEBUG2("iCache->CreateVideoListL() left: %d. Returning categories anyway.", err);
                }
            
            iCollection.CategoriesL().iList->
                        SetTObjectValueL<TBool>( KVcxMediaMyVideosVideoListIsPartial,
                        iCache.iVideoListIsPartial );

            iCollection.iObs->HandleOpen( iCollection.CategoriesL().iList, KErrNone );
            }
            break;

        case 2:
            {
            //we are at second level, return video list from some category

            TInt categoryId = aPath.Id();

            MPX_DEBUG2("CVcxMyVideosOpenHandler:: opening category %d", categoryId);
            
            if ( categoryId != KVcxMvcCategoryIdAll &&
                 categoryId != KVcxMvcCategoryIdDownloads &&
                 categoryId != KVcxMvcCategoryIdTvRecordings &&
                 categoryId != KVcxMvcCategoryIdCaptured &&
                 categoryId != KVcxMvcCategoryIdOther )
                {
                MPX_DEBUG2("CVcxMyVideosOpenHandler:: category ID not valid (%d) -> calling HandleOpen with KErrNotFound", categoryId);
                iCollection.iObs->HandleOpen( static_cast<CMPXMedia*>(NULL), KErrNotFound );
                return;
                }

            TUint8 origin = static_cast<TUint8>( TVcxMyVideosCollectionUtil::Origin( categoryId ) );

            iCache.CreateVideoListL(); // Causes async call to MDS, callbacks to DoHandleCreateVideoListRespL will happen.
                                       // If iCache.iVideoList is complete and can be used (correct sorting order),
                                       // then nothing is done.

            if ( !iCache.iVideoListIsPartial )
                {
                MPX_DEBUG1("CVcxMyVideosOpenHandler:: videolist complete");

                // iCache.iVideoList is complete
                if ( categoryId == KVcxMvcCategoryIdAll )
                    {
                    MPX_DEBUG1("CVcxMyVideosOpenHandler:: KVcxMvcCategoryIdAll: calling HandleOpen(iCache.iVideoList)");
                    iCollection.iObs->HandleOpen( iCache.iVideoList, KErrNone );                    
                    }
                else
                    {
                    MPX_DEBUG1("CVcxMyVideosOpenHandler:: other than KVcxMvcCategoryIdAll: creating new category video list");
                    CMPXMedia* videoList = iCache.CreateVideoListByOriginL( origin );
                    MPX_DEBUG1("CVcxMyVideosOpenHandler:: calling HandleOpen(new list)");
                    iCollection.iObs->HandleOpen( videoList, KErrNone );
                    delete videoList;
                    }
                iCollection.iMessageList->AddEventL( KVcxMessageMyVideosListComplete );
                iCollection.iMessageList->SendL();

                // No append events will arrive anymore -> we create our own version of the
                // video list.
                iCache.ReCreateVideoListL();
                }
            else
                {
                MPX_DEBUG1("CVcxMyVideosOpenHandler:: video list incomplete");
                // iCache.iVideoList is incomplete                

                if ( categoryId == KVcxMvcCategoryIdAll )
                    {
                    MPX_DEBUG1("CVcxMyVideosOpenHandler:: KVcxMvcCategoryIdAll, calling HandleOpen");
                    iCollection.iObs->HandleOpen( iCache.iVideoList, KErrNone  );
                    }
                else
                    {   
                    MPX_DEBUG1("CVcxMyVideosOpenHandler:: other than KVcxMvcCategoryIdAll");

                    TInt pos = iCategoryIdsBeingOpened.Find( categoryId );
                    if ( pos == KErrNotFound )
                        {
                        MPX_DEBUG1("CVcxMyVideosOpenHandler:: category was not opened yet, creating list for it");
                        iCategoryIdsBeingOpened.AppendL( categoryId ); 
                        CMPXMedia* videoList = iCache.CreateVideoListByOriginL( origin );
                        iVideoListsBeingOpened.AppendL( videoList );
                        MPX_DEBUG1("CVcxMyVideosOpenHandler:: calling HandleOpen");
                        iCollection.iObs->HandleOpen( iCache.iVideoList, KErrNone  );
                        }
                    else
                        {
                        MPX_DEBUG1("CVcxMyVideosOpenHandler:: category was already being opened, calling HandleOpen with that");                        
                        iCollection.iObs->HandleOpen( iVideoListsBeingOpened[pos], KErrNone  );
                        }
                    }
                }

            }
            break;

        case 3:
			{
			iCollection.iObs->HandleOpen(const_cast<CMPXCollectionPath*> (&aPath), KErrNone);
			}
			break;

        default:
            break;

        } //switch
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::DoHandleCreateVideoListRespL
// New items fetched from MDS. iCache.iVideoList = aVideoList.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::DoHandleCreateVideoListRespL(
        CMPXMedia* aVideoList, TInt aNewItemsStartIndex, TBool aComplete )
    {
    MPX_FUNC("CVcxMyVideosOpenHandler::DoHandleCreateVideoListRespL()");

    MPX_DEBUG2("CVcxMyVideosOpenHandler:: aComplete = %d", aComplete);
    
    if ( aNewItemsStartIndex != KErrNotFound )
        {
        //iCache.iVideoList now contains new items, tell iCache to check if partial list contains the new items
        iCache.CheckForPartialVideoListItemsL( aNewItemsStartIndex );
        }

    // Append new items to category video lists. All category is using iCache.iVideoList, thus no need to append to it.
    TInt count = iCategoryIdsBeingOpened.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iCache.AppendToListL( *iVideoListsBeingOpened[i], *aVideoList,
                TVcxMyVideosCollectionUtil::Origin( iCategoryIdsBeingOpened[i] ),
                aNewItemsStartIndex );
        }
            
    if ( aComplete == 0 )
        {
        iCollection.iMessageList->AddEventL( KVcxMessageMyVideosItemsAppended );

        // Still fetching items
        // Missing download fields filled from download manager
        TRAP_IGNORE( iCollection.SyncVideoListWithDownloadsL( *(iCache.iVideoList),
                EFalse /* dont send events */, aNewItemsStartIndex ); ); 
        iCollection.CategoriesL().UpdateCategoriesL( *aVideoList, aNewItemsStartIndex );
        }                           
    else
        {
        // End event arrived
        
        iCache.iVideoListIsPartial = EFalse;

        // Create modify event for All category.
        // This will cause client to make a new OpenL() call.
        // Also this causes collection framework to purge its possibly outdated
        // (KVcxMessageMyVideosItemsAppended events don't update collection framework cache) cache.
        MPX_DEBUG3("CVcxMyVideosOpenHandler:: adding modify event for category[%d], extra info = %d",
                KVcxMvcCategoryIdAll, EVcxMyVideosVideoListOrderChanged );
        iCollection.iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdAll, 1 ), EMPXItemModified,
                EVcxMyVideosVideoListOrderChanged );
        // We dont send here, the send is at the end of this function.

        for ( TInt i = 0; i < count; i++ )
            {
            // Create modify event for other than All categories.
            // This will cause client to make a new OpenL() call.
            // Also this causes collection framework to purge its possibly outdated
            // (KVcxMessageMyVideosItemsAppended events don't update collection frameworks cache) cache.
            MPX_DEBUG3("CVcxMyVideosOpenHandler:: adding modify event for category[%d], extra info = %d",
                    iCategoryIdsBeingOpened[i], EVcxMyVideosVideoListOrderChanged );
            iCollection.iMessageList->AddEventL( TMPXItemId( iCategoryIdsBeingOpened[i], 1 ), EMPXItemModified,
                    EVcxMyVideosVideoListOrderChanged );
            // We dont send here, the send is at the end of this function.
            delete iVideoListsBeingOpened[i]; // we can delete our copy, client has its own copy
            iVideoListsBeingOpened[i] = NULL;
            }
        iVideoListsBeingOpened.Reset();
        iCategoryIdsBeingOpened.Reset();

        // all category, lets delete our copy and start using new one, this gives the full ownership to client
        iCache.ReCreateVideoListL();

        iCollection.CategoriesL().UpdateCategoriesNewVideoNamesL();

        iCollection.iMessageList->AddEventL( KVcxMessageMyVideosListComplete );
        }
    iCollection.iMessageList->SendL();
    }

// END OF FILE
