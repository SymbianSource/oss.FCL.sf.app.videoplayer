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




// INCLUDE FILES
#include <bldvariant.hrh>
#include "IptvDebug.h"

#include "CIptvMediaContent.h"
#include "IptvLiveDataStructures.h"

#include <mpxmediageneraldefs.h>
#include <mpxitemid.h>
#include <mpxmedia.h>
#include "CIptvUtil.h" 

#include "CIptvMyVideosGlobalFileId.h"
#include "MIptvVodContentClientObserver.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvVodContentClient.h"
#include "vcxnsserviceprovider.h"
#include "vcxnscontentproviderobserver.h"
#include "vcxnscontentclienthandler.h"
#include "vcxnsmpxcollectionclienthandler.h"
#include "vcxnscontentprovider.h"
#include "vcxnscategoryprovider.h"
#include "vcxnsuiengine.h"
#include "vcxnscontent.h"
#include "vcxnscontentaccess.h"
#include "vcxnsdatalist.h"

// CONSTS
const TInt KMaxContentHistory = 50; 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::CVcxNsContentProvider()
// -----------------------------------------------------------------------------
//
CVcxNsContentProvider::CVcxNsContentProvider( CVcxNsUiEngine& aUiEngine ) :
    iUiEngine( aUiEngine )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsContentProvider* CVcxNsContentProvider::NewL( CVcxNsUiEngine& aUiEngine )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsContentProvider::NewL()");
    
    return new (ELeave) CVcxNsContentProvider( aUiEngine );
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::~CVcxNsContentProvider()
// -----------------------------------------------------------------------------
//
CVcxNsContentProvider::~CVcxNsContentProvider()
    {
    iContentObservers.Reset();
    
    iContentListList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// Get contents for active service and active category
// -----------------------------------------------------------------------------
//
RPointerArray<CVcxNsContent>& CVcxNsContentProvider::GetContentsL( TInt& aHighlight )
    {
    //Check if current state is content state and last state was category state:
    
    if ( iUiEngine.CategoryProvider() )
        {
        aHighlight = iUiEngine.CategoryProvider()->GetCurrentHighlightL();
        }
    else
        {
        aHighlight = 0;
        }
    
    return GetContentsL();
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::GetContentsL()
// -----------------------------------------------------------------------------
//
RPointerArray<CVcxNsContent>& CVcxNsContentProvider::GetContentsL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::GetContentsL");
    
    //  Dl list does not need to be updated, if fetching same old list again. 
    if( iPreviousList &&
        iPreviousList->iServiceId == iUiEngine.ActiveService() &&
        iPreviousList->iCategoryId == iUiEngine.ActiveCategory() )
        {
        return iPreviousList->iArray;
        }
    
    CVcxNsContentList* list = NULL;    
    
    list = GetListFromCache();
    
    if( !list )
        {
        // Reset old download list, items might not be visible anymore.
        iUiEngine.GetMpxCollectionClientHandlerL()->ResetDownloadList(
                EFalse /* don't reset commands from content items */);
        list = LoadEpgDataL();
        BuildDownloadListL( list->iArray );
        iUiEngine.GetMpxCollectionClientHandlerL()->LoadMpxMediaObjectsL();
        }
    else
        {
        // Reset old download list, items might not be visible anymore.
        iUiEngine.GetMpxCollectionClientHandlerL()->ResetDownloadList(
                ETrue /* reset commands from content items */);
        BuildDownloadListL( list->iArray );
        }

    iPreviousList = list;

    return list->iArray;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::RefreshContentsL()
// -----------------------------------------------------------------------------
//
TInt CVcxNsContentProvider::RefreshContentsL()
    {
    return iUiEngine.GetContentClientHandlerL()->UpdateEcgL( iUiEngine.ActiveService() );
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::StartDownloadL()
// -----------------------------------------------------------------------------
//
TInt CVcxNsContentProvider::StartDownloadL( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::StartDownloadL");
    CVcxNsService* service = ( iUiEngine.GetServiceProviderL()->GetActiveServiceData() );
    CVcxNsContent* content = NULL;
    
    RPointerArray<CVcxNsContent>& contents = GetContentsL();
    
    if( service && aIndex >= 0 && aIndex < contents.Count() )
	    {
	    content = contents[aIndex];
	    }
    else
        {
        return KErrArgument;
        }

    TInt retVal( KErrNotFound );
    
    if( service && content )
        {
        retVal = iUiEngine.GetMpxCollectionClientHandlerL()->DownloadL( *service, *content );
        }
    
    return retVal;
    }


// -----------------------------------------------------------------------------
// CVcxNsContentProvider::PauseDownloadL()
// -----------------------------------------------------------------------------
//
TInt CVcxNsContentProvider::PauseDownloadL( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::PauseDownloadL");
    CVcxNsContent* content = NULL;
    RPointerArray<CVcxNsContent>& contents = GetContentsL();
    
    if( aIndex >= 0 && aIndex < contents.Count() )
        {
        content = contents[aIndex];
        }
    else
        {
        return KErrArgument;
        }

    TInt retVal( KErrNotFound );
    
    if( content )
        {
        retVal = iUiEngine.GetMpxCollectionClientHandlerL()->PauseDownloadL( *content );
        }
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::ResumeDownloadL()
// -----------------------------------------------------------------------------
//
TInt CVcxNsContentProvider::ResumeDownloadL( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::ResumeDownloadL");
    CVcxNsContent* content = NULL;
    RPointerArray<CVcxNsContent>& contents = GetContentsL();

    
    if( aIndex >= 0 && aIndex < contents.Count() )
        {
        content = contents[aIndex];
        }
    else
        {
        return KErrArgument;
        }

    TInt retVal( KErrNotFound );
    
    if( content )
        {
        retVal = iUiEngine.GetMpxCollectionClientHandlerL()->ResumeDownloadL( *content );
        }
    
    return retVal;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::ResumeDownloadL()
// -----------------------------------------------------------------------------
//
TInt CVcxNsContentProvider::ResumeDownloadL( CVcxNsContent& aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::ResumeDownloadL()");
    
    if ( iUiEngine.ServiceProvider() )
        {
        return iUiEngine.GetMpxCollectionClientHandlerL()->ResumeDownloadL( aContent );
        }
    
    return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::CancelDownloadL()
// -----------------------------------------------------------------------------
//
TInt CVcxNsContentProvider::CancelDownloadL( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::CancelDownloadL");
    CVcxNsContent* content = NULL;
    RPointerArray<CVcxNsContent>& contents = GetContentsL();

    if( aIndex >= 0 && aIndex < contents.Count() )
        {
        content = contents[aIndex];
        }
    else
        {
        return KErrArgument;
        }

    TInt retVal( KErrNotFound );

    if( content )
        {
        retVal = iUiEngine.GetMpxCollectionClientHandlerL()->CancelDownloadL( *content );
        }

    return retVal;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::RemoveServiceData
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::RemoveServiceData( TUint32 aServiceId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::RemoveServiceData");
    
    if( iPreviousList && iPreviousList->iServiceId == aServiceId )
        {
        iPreviousList = NULL;
        }
    
    for( TInt k = 0; k < iContentListList.Count(); k++ )
        {
        if( (iContentListList[k])->iServiceId == aServiceId )
            {
            delete (iContentListList[k]);
            iContentListList.Remove( k );
            iContentListList.Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::RemoveCategoryData
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::RemoveCategoryData( 
    TUint32 aServiceId, 
    TUint32 aCategoryId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::RemoveCategoryData");

    if( iPreviousList && iPreviousList->iServiceId == aServiceId )
        {
        iPreviousList = NULL;
        }

    for( TInt k = 0; k < iContentListList.Count(); k++ )
        {
        if( (iContentListList[k])->iServiceId == aServiceId
                && (iContentListList[k])->iCategoryId == aCategoryId )
            {
            delete (iContentListList[k]);
            iContentListList.Remove( k );
            iContentListList.Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::RegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::RegisterObserver( MVcxNsContentProviderObserver* aObserver )
    {
    for ( TInt i = 0; i < iContentObservers.Count(); i++ )
        {
        if( iContentObservers[i] == aObserver )
            {
            return;
            }
        }
    
    iContentObservers.Append( aObserver );
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::DeRegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::DeRegisterObserver( MVcxNsContentProviderObserver* aObserver )
    {
    for ( TInt i = 0; i < iContentObservers.Count(); i++ )
        {
        if( iContentObservers[i] == aObserver )
            {
            iContentObservers.Remove( i );
            
            iContentObservers.Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::ContentUpdated()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::ContentUpdated( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::ContentUpdated");
    
    for ( TInt i = 0; i < iContentObservers.Count(); i++ )
        {
        if( iContentObservers[i] )
            {
            iContentObservers[i]->ContentUpdated( aIndex );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::RefreshView()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::RefreshView()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::RefreshView");
    for ( TInt i = 0; i < iContentObservers.Count(); i++ )
        {
        if( iContentObservers[i] )
            {
            iContentObservers[i]->RefreshView();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::UpdateIconsPathL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::UpdateIconsPathL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::UpdateIconsPathL");
    
    CVcxNsContentList* list = NULL; 
    list = LoadEpgDataL();
    
    if ( list && iPreviousList )
        {
        for ( TInt i = 0; i < list->iArray.Count(); i++ )
            {
            for ( TInt j = 0; j < iPreviousList->iArray.Count(); j++ )
                {
                if ( ( iPreviousList->iArray[j]->GetContentId() == list->iArray[i]->GetContentId() ) && 
                     ( iPreviousList->iArray[j]->GetServiceId() == list->iArray[i]->GetServiceId() ) )
                    {
                    iPreviousList->iArray[j]->SetIconPathL( list->iArray[i]->GetIconPath() );        
                    break;
                    }
                }       
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::HandleAppStateChanged()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::HandleAppStateChangedL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::HandleAppStateChangedL");
    for ( TInt i = 0; i < iContentObservers.Count(); i++ )
        {
        if( iContentObservers[i] )
            {
            iContentObservers[i]->HandleAppStateChangedL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::SearchContentByUri()
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsContentProvider::SearchContentByUri( const TDesC& aUri )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::SearchContentByUri IN");
    for( TInt i = iContentListList.Count()-1; i >= 0 ; i-- )
        {
        // improve nested loops
        for( TInt j = (iContentListList[i])->iArray.Count()-1; j >= 0 ; j-- )
            {
            TPtrC contentUri = TPtrC( ((iContentListList[i])->iArray[j])->GetUrl( CVcxNsContent::EVcxContentTypeVideo ) );
            
            if ( contentUri.CompareF( aUri ) == 0 )
                {
                IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::SearchContentByUri OUT");
                return (iContentListList[i])->iArray[j];
                }
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::SearchContentByUri OUT");
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::SearchContentByMpxId()
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsContentProvider::SearchContentByMpxId( TMPXItemId aItemId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::SearchContentByMpxId IN");
    CVcxNsContent* content = NULL;
    
    for( TInt i = iContentListList.Count()-1; i >= 0 ; i-- )
        {
        for( TInt j = (iContentListList[i])->iArray.Count()-1; j >= 0 ; j-- )
            {
            CMPXMedia* media = ((iContentListList[i])->iArray[j])->GetMpxMedia( CVcxNsContent::EVcxContentTypeVideo );
            TUint32 mediaId = aItemId.iId1;
            
            if( media )
                {
                TUint32 contentsId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1;
                
                if ( mediaId == contentsId )
                    {
                    content = (iContentListList[i])->iArray[j];
                    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::SearchContentByMpxId OUT, id found from mpx data");
				    return content;
                    }
                }
            else if ( ((iContentListList[i])->iArray[j])->GetMpxId( CVcxNsContent::EVcxContentTypeVideo ) && 
                      ((iContentListList[i])->iArray[j])->GetMpxId( CVcxNsContent::EVcxContentTypeVideo ) == mediaId )
                {
                content = (iContentListList[i])->iArray[j];
                IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::SearchContentByMpxId OUT, found by stored mpx id ");
                return content;
                }
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::SearchContentByMpxId OUT");
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::SearchContentById()
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsContentProvider::SearchContentById( TUint32 aServiceId, 
                                                         TUint32 aContentId )
    {
    // improve nested loops
    for( TInt i = iContentListList.Count()-1; i >= 0 ; i-- )
        {
        if( (iContentListList[i])->iServiceId == aServiceId )
            {
            for( TInt j = (iContentListList[i])->iArray.Count()-1; j >= 0 ; j-- )
                {
                if (((iContentListList[i])->iArray[j])->GetContentId() == aContentId )
                    {
                    return (iContentListList[i])->iArray[j];
                    }
                }            
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::ContentIndex()
// -----------------------------------------------------------------------------
//
TInt CVcxNsContentProvider::ContentIndex( CVcxNsContent* aContent )
    {
    if( !aContent )
        {
        return KErrArgument;
        }

    for( TInt i = iContentListList.Count()-1; i >= 0 ; i-- )
        {
        if( iUiEngine.ActiveService() == (iContentListList[i])->iServiceId && 
            iUiEngine.ActiveCategory() == (iContentListList[i])->iCategoryId )
            {
            for( TInt j = (iContentListList[i])->iArray.Count()-1; j >= 0 ; j-- )
                {
                if ( ((iContentListList[i])->iArray[j])->GetContentId() == aContent->GetContentId() )
                    {
                    return j;
                    }
                }
            break;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::UpdateContent()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::UpdateContent( CVcxNsContent* aContentdata )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::UpdateContent");
    if( aContentdata )
        {
        TInt index = ContentIndex( aContentdata );
    
        if( index >= 0 )
            {
            ContentUpdated( index );
            }        
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::ShowUpdatingNoteL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::ShowUpdatingNoteL( 
    TBool aShow,
    TInt aDownloadedTbns, 
    TInt aTotalTbns )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentProvider::ShowUpdatingNote");
    for ( TInt i = 0; i < iContentObservers.Count(); i++ )
        {
        if( iContentObservers[i] )
            {
            iContentObservers[i]->ShowUpdatingNoteL( aShow, aDownloadedTbns, aTotalTbns );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::StoreLastPlayPosL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::StoreLastPlayPosL( CVcxNsContent* aContent, 
                                               CVcxNsContent::TVcxNsContentAccessType aType,
                                               TReal32 aPos )
    {
    
    if( aType == CVcxNsContent::EVcxContentTypeStream )
        {
        CVcxNsContentAccess* access = aContent->GetContentAccess( aType );
        
        if ( access )
            {
            iUiEngine.GetContentClientHandlerL()->
                                    StoreLastPlayPosL( aContent->GetServiceId(), 
                                                       aContent->GetContentId(), 
                                                       access->iIndex,
                                                       aPos );            
            // Stream's pos is stored to epg, but it's fetched only when
            // loading content list, so it needs to be set to content item.
            CVcxNsContent* contentInCache = SearchContentById( aContent->GetServiceId(), aContent->GetContentId() );
			
            if( contentInCache )
			    {
				contentInCache->SetLastPlaybackPosition( aPos, aType );
				}
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::HandleErrorL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::HandleErrorL( 
    TInt aError,
    TBool aUpdate,
    TUint32 aServiceId )
    {
    for ( TInt i = 0; i < iContentObservers.Count(); i++ )
        {
        if ( iContentObservers[i] &&
              aUpdate &&
              aServiceId == iUiEngine.ActiveService() )
            {
            iContentObservers[i]->HandleUpdateErrorL( aError );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::HandleDlErrorL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::HandleDlErrorL( 
    TIptvDlError aError,
    CVcxNsContent& aContent )
    {
    for ( TInt i = 0; i < iContentObservers.Count(); i++ )
        {
        if ( iContentObservers[i] )
            {
            iContentObservers[i]->HandleDownloadErrorL( aError, aContent );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::GetListFromCache()
// -----------------------------------------------------------------------------
//
CVcxNsContentList* CVcxNsContentProvider::GetListFromCache()
    {
    for( TInt k = 0; k < iContentListList.Count(); k++ )
        {
        if( (iContentListList[k])->iServiceId == iUiEngine.ActiveService() &&
            (iContentListList[k])->iCategoryId == iUiEngine.ActiveCategory() )
            {
            // Sort the data by use order
            CVcxNsContentList* list = iContentListList[k];
            if( k != iContentListList.Count()-1 )
                {
                iContentListList.Remove( k );
                iContentListList.Compress();
                iContentListList.Append( list );
                }
            return list;
            }
        } 
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::GetListFromCache()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::BuildDownloadListL( RPointerArray<CVcxNsContent>& aContentList )
    {
    for( TInt k = 0; k < aContentList.Count(); k++ )
        {
        if( (aContentList[k])->GetMpxId( CVcxNsContent::EVcxContentTypeVideo ) )
            {
            iUiEngine.GetMpxCollectionClientHandlerL()->AppendToDownloadList( (aContentList[k]) );            
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::GetListFromCache()
// -----------------------------------------------------------------------------
//
CVcxNsContentList* CVcxNsContentProvider::LoadEpgDataL()
    {
    RPointerArray<CIptvVodContentContentBriefDetails> ecgList;
    TUint32 total;
    TBuf<1>KEmptyDes( KNullDesC ); 
    
    TInt error = iUiEngine.GetContentClientHandlerL()->GetVodContentClientL( 
                        iUiEngine.ActiveService() )->GetEcgListL( 
                        iUiEngine.ActiveCategory(),
                        KEmptyDes, 
                        0, 
                        0, 
                        total,
                        ecgList );

    // If cache is already full, remove the oldest data.
    if( iContentListList.Count() >= KMaxContentHistory )
        {
        delete iContentListList[0];
        iContentListList.Remove(0);
        iContentListList.Compress();
        }
    
    // Append the new data to cache
    CVcxNsContentList* entry = new (ELeave) CVcxNsContentList;
    entry->iCategoryId = iUiEngine.ActiveCategory();
    entry->iServiceId = iUiEngine.ActiveService();
    iContentListList.AppendL( entry );
    RPointerArray<CVcxNsContent>& contentList = entry->iArray;
    
    // Translate data classes
    if ( error == KErrNone )
        {
        CVcxNsContent* content = NULL;

        for( TInt i = 0; i < ecgList.Count(); i++ )
            {
            content = ConstructContentItemL( ecgList[ i ], iUiEngine.ActiveService() );
            
            if( content )
                {
                contentList.AppendL( content );
                }
            }
        }

    ecgList.ResetAndDestroy();
    
    return entry;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::HasVideoDetailsL()
// -----------------------------------------------------------------------------
//
TBool CVcxNsContentProvider::HasVideoDetailsL( TInt aSelected )
    {
    TBool hasDetails ( EFalse );
    CVcxNsContent* content = NULL;
    RPointerArray<CVcxNsContent>& contents = GetContentsL();
    
    if( aSelected >= 0 && aSelected < contents.Count() )
        {
        content = contents[aSelected];
        }
    
    if ( content )
        {
        hasDetails = ( content->GetLanguage() != KNullDesC ) ||
                     ( content->GetLength() > 0 ) ||
                     ( content->GetSize() > 0 ) ||
                     ( content->GetDescription() != KNullDesC ) ||
                     ( content->GetAuthor() != KNullDesC ) ||
                     ( content->GetCopyright() != KNullDesC );
        }

    if ( !hasDetails && content )
        {
        FetchFullDetailsL( content );
        
        hasDetails = ( content->GetDescription() != KNullDesC ) ||
                     ( content->GetAuthor() != KNullDesC ) ||
                     ( content->GetCopyright() != KNullDesC );
        }

    return hasDetails;
    }


// -----------------------------------------------------------------------------
// CVcxNsContentProvider::FullDetailsL()
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsContentProvider::GetFullDetailsL( TInt aSelected )
    {
    CVcxNsContent* content = NULL;
    RPointerArray<CVcxNsContent>& contents = GetContentsL();
        
    if( aSelected >= 0 && aSelected < contents.Count() )
        {
        content = contents[aSelected];
        }
    
    if ( content )
        {
        //if full details have already been fetched for this item, no need to fetch again
        if ( content->GetDescription().Length() > 0 ||
             content->GetAuthor().Length() > 0 ||
             content->GetCopyright().Length() > 0 )
            {
            return content;
            }
        else
            {
            FetchFullDetailsL( content );
            }
        }
    return content;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::AddContentAccessesL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::AddContentAccessesL( CVcxNsContent& aContent, 
                                                 RPointerArray<CIptvMediaContent>& aContentAccessList )
    {
    for ( TInt i=0; i < aContentAccessList.Count(); i++ )
        {
        TUint32 type = (aContentAccessList[i])->iDownloadType;
        TUint32 expr = (aContentAccessList[i])->iExpression;
        
        if( expr == CIptvMediaContent::EFull && type == EIptvDownloadTypeStream )
            {
            CVcxNsContentAccess* contentAccess = CVcxNsContentAccess::NewL();
            CleanupStack::PushL( contentAccess );

            contentAccess->SetUrlL( (aContentAccessList[i])->GetMediaContentUrl() );
            contentAccess->iType = CVcxNsContent::EVcxContentTypeStream;
            contentAccess->iIndex = i;
            contentAccess->iLastPosition = (aContentAccessList[i])->iLastPosition;
            
            aContent.AddContentAccess( contentAccess );
            CleanupStack::Pop( contentAccess );
            }
        else if( expr == CIptvMediaContent::EFull && type == EIptvDownloadTypeImmediate )
            {
            CVcxNsContentAccess* contentAccess = CVcxNsContentAccess::NewL();
            CleanupStack::PushL( contentAccess );

            contentAccess->SetUrlL( (aContentAccessList[i])->GetMediaContentUrl() );
            contentAccess->iType = CVcxNsContent::EVcxContentTypeVideo;
            contentAccess->iIndex = i;
            contentAccess->iMpxId = (aContentAccessList[i])->iFileId;
            
            aContent.AddContentAccess( contentAccess );
            CleanupStack::Pop( contentAccess );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::GetContentL()
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsContentProvider::GetContentL( TUint32 aServiceId, TUint32 aContentId )
    {
    CVcxNsContent* cnt = SearchContentById(  aServiceId, aContentId );
    
    if( !cnt )
        {
        // Get all contents of the service to cache
        RPointerArray<CIptvVodContentContentBriefDetails> ecgList;
        TUint32 total;
        TBuf<1>KEmptyDes( KNullDesC ); 
        
        TInt error = iUiEngine.GetContentClientHandlerL()->GetVodContentClientL( aServiceId )->
                            GetEcgAllListL( KEmptyDes, 0, 0, total, ecgList );

        User::LeaveIfError( error );

        CVcxNsContentList* entry = new (ELeave) CVcxNsContentList;
        entry->iServiceId  = aServiceId;
        entry->iCategoryId = KIptvVodContentCategoryRootId;
        iContentListList.AppendL( entry );
        RPointerArray<CVcxNsContent>& contentList = entry->iArray;
        
        // Translate data classes
        CVcxNsContent* content = NULL;

        for( TInt i = 0; i < ecgList.Count(); i++ )
            {
            content = ConstructContentItemL( ecgList[ i ], aServiceId );
            
            if( content )
                {
                contentList.AppendL( content );
                }
            }

        ecgList.ResetAndDestroy();
        
        // Search the content from new list
        for( TInt i = 0; i < contentList.Count() ; i++ )
            {
            if ( contentList[i]->GetContentId() == aContentId )
                {
                cnt = contentList[i];
                break;
                }
            }
        }
    
    return cnt;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::ConstructContentItemL()
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsContentProvider::ConstructContentItemL( CIptvVodContentContentBriefDetails* aData,
                                                             TUint32 aServiceId )
    {
    CVcxNsContent* content = CVcxNsContent::NewL();
    
    CleanupStack::PushL( content );

    content->SetNameL( aData->iName );
    content->SetIconPathL( aData->iThumbnailPath );
    content->SetServiceId( aServiceId );
    content->SetContentId( aData->iContentId );
    content->SetSize( aData->iSize );
    content->SetLength( aData->iPlaytime );
    content->SetLanguageL( aData->iLanguage );
    content->SetBrowserUrlL( aData->GetBrowserUrlL() );
    content->SetAgeProfile( aData->iRatingAge );

    RPointerArray<CIptvMediaContent> caList;
    CleanupResetAndDestroyPushL( caList );
    TInt ret = iUiEngine.GetContentClientHandlerL()->GetVodContentClientL( content->GetServiceId() )->GetContentAccessListL( content->GetContentId(), caList );

    if( KErrNone == ret && caList.Count() > 0 )
        {
        AddContentAccessesL( *content, caList );
        
        CleanupStack::PopAndDestroy( &caList );
        CleanupStack::Pop( content );
        
        return  content;
        }
    else
        {
        // Trash 'em
        CleanupStack::PopAndDestroy( &caList );
        CleanupStack::PopAndDestroy( content );
        
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentProvider::FetchFullDetailsL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::FetchFullDetailsL( CVcxNsContent* aContent )
    {
    if ( aContent->FullDetailsFetched() )
        {
        // Do this only once
        return;
        }

    CIptvVodContentContentFullDetails* fullDetails = CIptvVodContentContentFullDetails::NewL();
    CleanupStack::PushL( fullDetails );
    
    TInt err = iUiEngine.GetContentClientHandlerL()->GetVodContentClientL(
                        iUiEngine.ActiveService() )->GetContentDetailsL(
                        aContent->GetContentId(), *fullDetails );
    
    if ( err == KErrNone )
        {
        aContent->SetFullDetailsFetched( ETrue );
        
        if ( fullDetails->iDescription != KNullDesC )
            {
            aContent->SetDescriptionL( fullDetails->iDescription );
            }
        if ( fullDetails->iAuthor != KNullDesC )
            {
            aContent->SetAuthorL( fullDetails->iAuthor );
            }
        if ( fullDetails->iCopyright != KNullDesC )
            {
            aContent->SetCopyrightL( fullDetails->iCopyright );
            }
        }
    
    CleanupStack::PopAndDestroy( fullDetails );
    }

// -----------------------------------------------------------------------------
// Set highlighted content index for active category
// -----------------------------------------------------------------------------
//
void CVcxNsContentProvider::SetContentHighlight( TInt aHighlight )
    {
    if ( iUiEngine.CategoryProvider() )
        {
        if ( aHighlight < 0 )
            {
            aHighlight = 0;
            }
        
        TRAP_IGNORE( 
            iUiEngine.CategoryProvider()->SetCurrentHighlightL( aHighlight ) );
        }
    }
