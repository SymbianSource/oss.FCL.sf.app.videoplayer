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




#include "TIptvRssSearchQuery.h"
#include "CIptvEpgCategory.h"
#include "CIptvEpgPluginInterface.h"
#include "IptvDebug.h"
#include "CIptvEpgContent.h"
#include "CIptvMediaContent.h"
#include <uri8.h>
#include <uriutils.h>
#include <bautils.h>
#include <f32file.h>
#include <centralrepository.h>

#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvService.h"
#include "CIptvSearchManager.h"
#include "CIptvServer.h"
#include "CIptvServiceManager.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvEpgVodMsqQueue.h"
#include "CIptvContentIdMediaContentItem.h"
#include "CIptvEpgSession.h"

_LIT( KIptvSearchStringKeyword, "#search#" );

const TInt KParentalRatingOff = 0;

// ---------------------------------------------------------
// CIptvSearchManager::NewL
// 
// ---------------------------------------------------------
CIptvSearchManager* CIptvSearchManager::NewL( CIptvServer& aServer,
                                              CIptvEpgSession& aParent,
											  CIptvEpgVodMsqQueue* aVodMsgQueue )
	{
	CIptvSearchManager* self = new (ELeave) CIptvSearchManager( aServer, aParent );
	CleanupStack::PushL( self );
	self->ConstructL( aVodMsgQueue );
	CleanupStack::Pop( self );
	return self;
	}
	
// ---------------------------------------------------------
// CIptvSearchManager::~CIptvSearchManager
// 
// ---------------------------------------------------------
CIptvSearchManager::~CIptvSearchManager()
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::~CIptvSearchManager()");
	
	iUsedContentKeys.Reset();
	
	ClearAllResults();
	delete iFileMan;
	
	iVodMsgQueues.Reset();
	delete iUpdater;
	delete iSearchCategory;
	delete iCurrentService;
	iAllContentFullDetails.ResetAndDestroy();
	iMediaContents.ResetAndDestroy();
	iFs.Close();

    delete iRssPlugin;

    if ( iCenRepNotifyHandler )
        {
        iCenRepNotifyHandler->StopListening();
        delete iCenRepNotifyHandler;
        }

    delete iCenRepSession;
    
    delete iSearchUrlTemplate;
	}
	
// ---------------------------------------------------------
// CIptvSearchManager::CIptvSearchManager
// 
// ---------------------------------------------------------
CIptvSearchManager::CIptvSearchManager( CIptvServer& aServer,
                                        CIptvEpgSession& aParent ) :
										iServer( aServer ),
										iParent( aParent ),
										iParentalControl( KParentalRatingOff )
	{
	ResetContentKey();
	}
	
// ---------------------------------------------------------
// CIptvSearchManager::ConstructL
// 
// ---------------------------------------------------------
void CIptvSearchManager::ConstructL( CIptvEpgVodMsqQueue* aVodMsgQueue )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::ConstructL()");

	User::LeaveIfError( iFs.Connect() );
	
	iFileMan = CFileMan::NewL( iFs );

	iVodMsgQueues.InsertInAddressOrder( aVodMsgQueue );
	
	iUpdater = CIptvEpgManagerUpdater::NewL(*this);
	
    iSearchCategory = CIptvVodContentCategoryBriefDetails::NewL();
    iSearchCategory->iCategoryId = KIptvVodContentCategoryRootId;
    iSearchCategory->iIsSearchCategory = ETrue;

    TRAPD( ret, iCenRepSession = CRepository::NewL( KIptvCenRepUid ) );

    if ( ret == KErrNone && iCenRepSession )
        {
        // Register CenRep change notifier.
        iCenRepNotifyHandler = CCenRepNotifyHandler::NewL(
            *this,
            *iCenRepSession,
            CCenRepNotifyHandler::EIntKey,
            KIptvCenRepParentControlKey );
        iCenRepNotifyHandler->StartListeningL();

        // Read settings from CenRep.
        ReadCenRepData();
        }
        
    // Just to make sure we dont have null pointer available
    iSearchUrlTemplate = HBufC::NewL( 0 );
	}

// -----------------------------------------------------------------------------
// CIptvSearchManager::ReadCenRepData()
// CenRep
// -----------------------------------------------------------------------------
//
void CIptvSearchManager::ReadCenRepData()
    {
    IPTVLOGSTRING_LOW_LEVEL( "Search Mgr ## CIptvSearchManager::ReadCenRepData" );

    if ( iCenRepSession )
        {
        TInt age;
        if ( iCenRepSession->Get( KIptvCenRepParentControlKey, age ) != KErrNone )
            {
            age = KIptvCenRepParentControlKeyDefault;
            }
        iParentalControl = age;
        }
    IPTVLOGSTRING2_LOW_LEVEL("Search Mgr ## Currently used parental rating = %d", iParentalControl );
    }

// ---------------------------------------------------------
// CIptvSearchManager::HandleNotifyInt
// 
// ---------------------------------------------------------
void CIptvSearchManager::HandleNotifyInt( TUint32 aId, TInt aNewValue )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::HandleNotifyInt");
    if ( aId == KIptvCenRepParentControlKey )
        {
        IPTVLOGSTRING2_LOW_LEVEL( "Search Mgr ## Parental control key changed. New value = %d", aNewValue );
        iParentalControl = aNewValue;
        }
	}

// ---------------------------------------------------------
// CIptvSearchManager::AddEpgVodMsgQueue
// 
// ---------------------------------------------------------
void CIptvSearchManager::AddEpgVodMsgQueue( CIptvEpgVodMsqQueue* aVodMsgQueue )
    {
    IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::UpdateEpgVodMsgQueue()");

    if ( aVodMsgQueue )
        {
        iVodMsgQueues.InsertInAddressOrder( aVodMsgQueue );
        }
    }

// ---------------------------------------------------------
// CIptvSearchManager::RemoveEpgVodMsgQueue
// 
// ---------------------------------------------------------
void CIptvSearchManager::RemoveEpgVodMsgQueue( CIptvEpgVodMsqQueue* aVodMsgQueue )
    {
    IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::RemoveEpgVodMsgQueue()");

    for ( TInt i = 0; i < iVodMsgQueues.Count(); i++ )
        {
        if ( iVodMsgQueues[i] == aVodMsgQueue )
            {
            iVodMsgQueues.Remove( i );
            return;
            }
        }
    }

// ---------------------------------------------------------
// CIptvSearchManager::ParseSearchAddressL
// 
// ---------------------------------------------------------
void CIptvSearchManager::ParseSearchAddressL( TIptvRssSearchQuery& aQuery )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::ParseSearchAddressL()");
	
	TInt newTotalLength = aQuery.SearchString().Length() +
						  iSearchUrlTemplate->Length();

	HBufC* parsedSearchUrl = HBufC::NewL( newTotalLength );
	CleanupStack::PushL( parsedSearchUrl );
	parsedSearchUrl->Des().Copy( *iSearchUrlTemplate );
						  
	// Try searching the 'search=' string from the template
	TInt queryLocation = parsedSearchUrl->Find( KIptvSearchStringKeyword );
	if( queryLocation != KErrNotFound )
		{
		// Replace KIptvSearchStringKeyword with given search string
		parsedSearchUrl->Des().Replace( queryLocation,
								 		KIptvSearchStringKeyword().Length(),
								 		aQuery.SearchString() );
		}

    CUri8* inetUri = UriUtils::CreateUriL( *parsedSearchUrl );
    CleanupStack::PushL( inetUri );
    
    HBufC8* parameter = HBufC8::NewLC( inetUri->Uri().UriDes().Length() );
    _LIT8( KStringFormat, "%S");
    parameter->Des().Format( KStringFormat, 
                             &inetUri->Uri().UriDes() );
        
    HBufC* actualSearchUrl = HBufC::NewLC( parameter->Length() );
    actualSearchUrl->Des().Copy( *parameter );
    
    iCurrentService->SetSearchUrlL( *actualSearchUrl );
    CleanupStack::PopAndDestroy( actualSearchUrl );
    CleanupStack::PopAndDestroy( parameter );
	CleanupStack::PopAndDestroy( inetUri );
	CleanupStack::PopAndDestroy( parsedSearchUrl );
	}

// ---------------------------------------------------------
// CIptvSearchManager::UpdateUsedContentIdsListL
// 
// ---------------------------------------------------------
void CIptvSearchManager::UpdateUsedContentIdsListL( RArray<TInt>& aUsedIdArray )
    {
    iUsedContentKeys.Reset();
    for ( TInt i = 0; i < aUsedIdArray.Count(); i++ )
        {
        IPTVLOGSTRING2_LOW_LEVEL("Search Mgr ## CIptvSearchManager::UpdateUsedContentIdsListL() adding Content ID %d to used list.", aUsedIdArray[i]);

        iUsedContentKeys.AppendL( aUsedIdArray[i] );
        }
    }

// ---------------------------------------------------------
// CIptvSearchManager::Search
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::Search( TIptvRssSearchQuery& aQuery )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::Search()");

	TInt retVal = KErrNone;
	ClearAllResults();
	
	if( IsSearchPossible() && !iPluginIsRunning )
		{
		TInt err = KErrNone;
		TRAP( err, HandleSearchL( aQuery ) );
		retVal = err;
		}
	else
		{
		retVal = KErrNotReady;
		}
	return retVal;
	}

// ---------------------------------------------------------
// CIptvSearchManager::CancelSearch
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::CancelSearch()
	{
    iUpdater->Stop();
	iPluginIsRunning = EFalse;
	delete iRssPlugin;
	iRssPlugin = NULL;
	// If vod update aint running, close ecom session
    if ( ! iParent.IsPluginRunning() )
        {
        REComSession::FinalClose();
        }

	ClearAllResults();
	return KErrNone;
	}

// ---------------------------------------------------------
// CIptvSearchManager::HandleSearchL
// 
// ---------------------------------------------------------
void CIptvSearchManager::HandleSearchL( TIptvRssSearchQuery& aQuery )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::HandleSearchL()");
	
	ParseSearchAddressL( aQuery );
	if( iRssPlugin )
		{
		delete iRssPlugin;
		iRssPlugin = NULL;
		}
		
    iUpdater->Start();
		
	iRssPlugin = CIptvEpgPluginInterface::NewL( iCurrentService->iUid );
	iRssPlugin->SetVodCallback( this );
	iRssPlugin->SetServiceInformation( iCurrentService, ETrue );
	iPluginIsRunning = ETrue;
	iRssPlugin->RunPlugin();
	}

// ---------------------------------------------------------
// CIptvSearchManager::SearchCategoryBriefDetailsL
// 
// ---------------------------------------------------------
CIptvVodContentCategoryBriefDetails* CIptvSearchManager::SearchCategoryContentBriefDetailsL()
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::SearchCategoryContentBriefDetailsL()");

	CIptvVodContentCategoryBriefDetails* searchCat = NULL;
	if( IsSearchPossible() )
		{
		searchCat = CIptvVodContentCategoryBriefDetails::NewL();
		searchCat->iCategoryId = KIptvRssSearchCategoryId;
		searchCat->iIsSearchCategory = ETrue;
		searchCat->iContentCount = iSearchCategory->iContentCount;
		IPTVLOGSTRING2_LOW_LEVEL( "Content count for search category = %u", searchCat->iContentCount );
		}
	return searchCat;
	}

// ---------------------------------------------------------
// CIptvSearchManager::GetSearchContentBriefDetailsL
// 
// ---------------------------------------------------------
void CIptvSearchManager::GetSearchContentBriefDetailsL( TIptvCategoryId aContentId,
														TDesC& /*aSearchString*/,
														TUint32 /*aFrom*/,
														TUint32 /*aAmount*/,
														TUint32& aTotalAmount,
														RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::GetSearchContentBriefDetailsL()");

	TInt contentsAdded = 0;
	if( aContentId == KIptvRssSearchCategoryId )
		{
		for( TInt i = 0; i < iAllContentFullDetails.Count(); i++ )
			{
			CIptvVodContentContentFullDetails* fullDetails = iAllContentFullDetails[i]; // Owned by the array
			// Check the parental control
			if( iParentalControl == KParentalRatingOff ||
				fullDetails->iRatingAge < iParentalControl )
				{
				IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## Parental rating check passed");
				CIptvVodContentContentBriefDetails* bd = CIptvVodContentContentBriefDetails::NewL();
				CleanupStack::PushL( bd );
				bd->iContentId = fullDetails->iContentId;
				bd->iName.Copy( fullDetails->iName );
				bd->iThumbnailPath.Copy( fullDetails->iThumbnailPath );
				bd->iPlaytime = fullDetails->iPlaytime;
				bd->iSize = fullDetails->iSize;
				bd->iFileId->iDrive = fullDetails->iFileId->iDrive;
				bd->iFileId->iFileId = fullDetails->iFileId->iFileId;
				bd->iPreviewGlobalFileId->iDrive = fullDetails->iPreviewGlobalFileId->iDrive;
				bd->iPreviewGlobalFileId->iFileId = fullDetails->iPreviewGlobalFileId->iFileId;
				bd->iPubDate= fullDetails->iPubDate;
				bd->iLanguage.Copy( fullDetails->iLanguage );
				bd->iRatingAge = fullDetails->iRatingAge;
				bd->SetBrowserUrlL( fullDetails->GetBrowserUrlL() );
				aEcgList.AppendL( bd );
				CleanupStack::Pop( bd );
				contentsAdded++;
				}
			else
				{
				IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## Content skipped. Reason: Parental Rating");
				IPTVLOGSTRING2_LOW_LEVEL("Search Mgr ## Device parental rating = %d", iParentalControl );
				IPTVLOGSTRING2_LOW_LEVEL("Search Mgr ## Contents parental rating = %d", fullDetails->iRatingAge );
				}
			}
		}
	aTotalAmount = contentsAdded;
	}

// ---------------------------------------------------------
// CIptvSearchManager::GetParentCategory
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::GetParentCategory( TIptvCategoryId aCategoryId,
											TIptvCategoryId& aParentCategoryId )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::GetParentCategory()");

	TInt retVal = KErrNotSupported;
	if( aCategoryId == KIptvRssSearchCategoryId )
		{
		aParentCategoryId = KIptvVodContentCategoryRootId;
		retVal = KErrNone;
		}
	return retVal;
	}

// ---------------------------------------------------------
// CIptvSearchManager::SetServiceId
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::SetServiceId( TUint32 aServiceId )
	{
	IPTVLOGSTRING2_LOW_LEVEL("Search Mgr ## CIptvSearchManager::SetServiceId(%d)", aServiceId);
	iServiceId = aServiceId;
	return KErrNone;
	}

// ---------------------------------------------------------
// CIptvSearchManager::SetServiceL
// 
// ---------------------------------------------------------
void CIptvSearchManager::SetServiceL( CIptvEpgService* aService )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::SetServiceL()");

	if( iCurrentService )
		{
		delete iCurrentService;
		iCurrentService = NULL;
		}
	iCurrentService = CIptvEpgService::NewL();

    iCurrentService->iAddress.Copy( aService->iAddress);      
    iCurrentService->SetSearchUrlL( aService->SearchUrl() );
    iCurrentService->iIap = aService->iIap;
    iCurrentService->iUserName.Copy( aService->iUserName );
    iCurrentService->iPassword.Copy( aService->iPassword );
    iCurrentService->iUid = aService->iUid;
    iCurrentService->iUpdateSet = aService->iUpdateSet;
    iCurrentService->iServiceType = aService->iServiceType;
    iCurrentService->iServiceId = aService->iServiceId;
    iCurrentService->iLastUpdated = aService->iLastUpdated;

	iServiceId = iCurrentService->iServiceId;

	if( iSearchUrlTemplate )
		{
		delete iSearchUrlTemplate;
		iSearchUrlTemplate = NULL;
		}
	iSearchUrlTemplate = aService->SearchUrl().AllocL();

	ResolveThumbnailDirectory();
    }

// ---------------------------------------------------------
// CIptvSearchManager::ResolveThumbnailDirectory
// 
// ---------------------------------------------------------
void CIptvSearchManager::ResolveThumbnailDirectory()
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::ResolveThumbnailDirectory");
	iThumbnailDirectory.Zero();
	TRAPD( error, CIptvUtil::GetPathL( iFs,
									   EIptvPathRssSearchThumbnail,
									   iServiceId,
									   iThumbnailDirectory,
									   EFalse ) );
	if( error != KErrNone )
		{
		IPTVLOGSTRING2_LOW_LEVEL("CIptvUtil::GetPathL failed with error %d", error );
		}
	}

// ---------------------------------------------------------
// CIptvSearchManager::SetIap
// 
// ---------------------------------------------------------
void CIptvSearchManager::SetIap( TUint32 aIap )
	{
	IPTVLOGSTRING2_LOW_LEVEL("Search Mgr ## CIptvSearchManager::SetIap(%d)", aIap);

	iIap = aIap;
	}
	
// ---------------------------------------------------------
// CIptvSearchManager::StartedL
// 
// ---------------------------------------------------------
void CIptvSearchManager::StartedL()
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::StartedL() -- RSS Plugin STARTED!");
	}

// ---------------------------------------------------------
// CIptvSearchManager::ContentUpdateCompletedL
// 
// ---------------------------------------------------------
void CIptvSearchManager::ContentUpdateCompletedL()
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::ContentUpdateCompletedL()");
    // Service id = 0 because we dont have to bring this information
    // over IPC. Service ID is already on the client side.
    for ( TInt i = 0; i < iVodMsgQueues.Count(); i++ )
        {
        if ( iVodMsgQueues[i] )
            {
            iVodMsgQueues[i]->SendMessageToClientL( KIptvRssSearchContentUpdateCompleted, 0 );
            }
        }
	}

// ---------------------------------------------------------
// CIptvSearchManager::FinishedL
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::FinishedL( TTime& /*aUpdateInterval*/ )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::FinishedL() -- RSS Plugin FINISHED!");

	iPluginIsRunning = EFalse;

	return KErrNone;
	}

// ---------------------------------------------------------
// CIptvSearchManager::AddCategoryL
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::AddCategoryL( CIptvEpgCategory& /*aCategory*/,
									   TUint32& /*aCategoryKey*/ )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::AddCategoryL()");
	
	return KErrNone;
	}

// ---------------------------------------------------------
// CIptvSearchManager::AddContentL
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::AddContentL( CIptvEpgContent& aContent,
    				  				  RPointerArray<CIptvMediaContent> aMediaContents,
    				  				  TUint32& aContentKey )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::AddContentL()");

	IncrementContentKey();
	aContentKey = GetContentKey();
	CIptvVodContentContentFullDetails* details = CreateContentFullDetailsFromEpgDataL( aContent );
	CleanupStack::PushL( details );
	details->iContentId = GetContentKey();
	iAllContentFullDetails.AppendL( details ); // Ownership in an array
	CleanupStack::Pop( details );

	CIptvContentIdMediaContentItem* item = CIptvContentIdMediaContentItem::NewL();
	CleanupStack::PushL( item );
	item->SetContentId( GetContentKey() );
	item->SetMediaContentArrayL( aMediaContents );
	
	iMediaContents.AppendL( item ); // item variable's ownership now in iMediaContents
	CleanupStack::Pop( item );

	iSearchCategory->iContentCount++;
	IPTVLOGSTRING2_LOW_LEVEL( "iSearchCategory->iContentCount = %u", iSearchCategory->iContentCount );

	return KErrNone;
	}

// ---------------------------------------------------------
// CIptvSearchManager::UpdateServiceThumbnailL
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::UpdateServiceThumbnailL(
    TDesC& /*aIconFilePath*/,
    const TBool /*aGrouped*/ )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::UpdateServiceThumbnailL()");

	return KErrNone;
	}

// ---------------------------------------------------------
// CIptvSearchManager::AddRelationL
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::AddRelationL( TUint32& /*aCategoryKey*/,
									   TUint32& /*aContentKey*/ )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::AddRelationL()");

	return KErrNone;
	}

// ---------------------------------------------------------
// CIptvSearchManager::ErrorL
// 
// ---------------------------------------------------------
void CIptvSearchManager::ErrorL(TInt aError, TInt aInfo )
	{
	IPTVLOGSTRING3_LOW_LEVEL("Search Mgr ## CIptvSearchManager::ErrorL(%d, %d)", aError, aInfo);

    for ( TInt i = 0; i < iVodMsgQueues.Count(); i++ )
        {
        if ( iVodMsgQueues[i] )
            {
            iVodMsgQueues[i]->SendMessageToClientL(aError, aInfo);
            }
        }
	}

// ---------------------------------------------------------
// CIptvSearchManager::UpdateCategoryIconL
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::UpdateCategoryIconL(
								TUint32& /*aCategoryKey*/,
    						  	TUint32& /*aIconFileId*/,
    						  	TDesC& aIconFilePath,
    						  	TBool aIsUrl )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::UpdateCategoryIconL()");

	if( !aIsUrl )
		{
		iSearchCategory->iThumbnailPath.Copy( aIconFilePath );
		}

	return KErrNone;
	}

// ---------------------------------------------------------
// CIptvSearchManager::UpdateContentIconL
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::UpdateContentIconL(
								TUint32& aContentKey,
    						 	TUint32& /*aIconFileId*/,
    						 	TDesC& aIconFilePath,
    						 	TBool aIsUrl )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::UpdateContentIconL()");

	if( !aIsUrl )
		{
		// Find the content
		for( TInt i = 0; i < iAllContentFullDetails.Count(); i++ )
			{
			CIptvVodContentContentFullDetails* fd = iAllContentFullDetails[i];
			if( fd->iContentId == aContentKey )
				{
				fd->iThumbnailPath.Copy( aIconFilePath );
				}
			}
		}

	return KErrNone;
	}
	
// ---------------------------------------------------------
// CIptvSearchManager::IsSearchPossible
// 
// ---------------------------------------------------------
TBool CIptvSearchManager::IsSearchPossible() const
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::IsSearchPossible()");

	TBool isPossible = EFalse;
	// Here we check if there is 
	if( iCurrentService &&
		iSearchUrlTemplate &&
		iSearchUrlTemplate->Length() > 0 )
		{
	    IPTVLOGSTRING_LOW_LEVEL("CIptvSearchManager::IsSearchPossible: Search is possible");
		isPossible = ETrue;
		}
	else
		{
	    IPTVLOGSTRING_LOW_LEVEL("CIptvSearchManager::IsSearchPossible: Search is NOT possible");
		}
	return isPossible;
	}

// ---------------------------------------------------------
// CIptvSearchManager::PrepareForNewSearch
// 
// ---------------------------------------------------------
void CIptvSearchManager::ClearAllResults()
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::ClearAllResults()");
	
	iAllContentFullDetails.ResetAndDestroy();
	iMediaContents.ResetAndDestroy();
	ResetContentKey();
	// Remove contents of iThumbnailDirectory
	if( iThumbnailDirectory.Length() > 0 &&
		BaflUtils::PathExists( iFs, iThumbnailDirectory ) )
		{
		TInt error = iFileMan->Delete( iThumbnailDirectory, CFileMan::ERecurse );
		if( error != KErrNone )
			{
			IPTVLOGSTRING2_LOW_LEVEL("Failed to remove directory %S", &iThumbnailDirectory );
			}
		}

	iSearchCategory->iContentCount = 0;
	}

// ---------------------------------------------------------
// CIptvSearchManager::GetContentAccessListL
// 
// ---------------------------------------------------------
void CIptvSearchManager::GetContentAccessListL(
								TUint32 aContentId,
								RPointerArray<CIptvMediaContent>& aContents )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::GetContentAccessListL()");
    
	for( TInt i = 0; i < iMediaContents.Count(); i++ )
		{
		if( iMediaContents[i]->ContentId() == aContentId )
			{
		    IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::GetContentAccessListL() FOUND MATCH!");
			iMediaContents[i]->GetMediaContentsL( aContents );
			i = iMediaContents.Count(); // Get out of here. break; would work as well
			}
		}
	}

// ---------------------------------------------------------
// CIptvSearchManager::GetContentAccessL
// 
// ---------------------------------------------------------
CIptvMediaContent* CIptvSearchManager::GetContentAccessL( TUint32 aContentId, TUint32 aIndex )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::GetContentAccessL()");
	
	CIptvMediaContent* returnContent = NULL;
	
	for( TInt i = 0; i < iMediaContents.Count(); i++ )
		{
		CIptvContentIdMediaContentItem* contentItem = iMediaContents[i];
		if( contentItem->ContentId() == aContentId )
			{
  			returnContent = contentItem->MediaContentByIndexL( aIndex );
  			i = iMediaContents.Count();
			}
		}
	return returnContent;
	}

// ---------------------------------------------------------
// CIptvSearchManager::GetContentDetailsL
// 
// ---------------------------------------------------------
TInt CIptvSearchManager::GetContentDetailsL( TIptvContentId aContentId,
							 				 CIptvVodContentContentFullDetails& aContentFullDetails )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::GetContentDetailsL()");

	for( TInt i = 0; i < iAllContentFullDetails.Count(); i++ )
		{
		CIptvVodContentContentFullDetails* content = iAllContentFullDetails[i];
		if( content->iContentId == aContentId )
			{
			aContentFullDetails.iContentId = content->iContentId;
			aContentFullDetails.iName.Copy( content->iName );
			aContentFullDetails.iThumbnailPath.Copy( content->iThumbnailPath );
			aContentFullDetails.iPlaytime = content->iPlaytime;
			aContentFullDetails.iSize = content->iSize;
			aContentFullDetails.iFileId->iDrive = content->iFileId->iDrive;
			aContentFullDetails.iFileId->iFileId = content->iFileId->iFileId;
			aContentFullDetails.iPreviewGlobalFileId->iDrive = content->iPreviewGlobalFileId->iDrive;
			aContentFullDetails.iPreviewGlobalFileId->iFileId = content->iPreviewGlobalFileId->iFileId;
			aContentFullDetails.iPubDate = content->iPubDate;
			aContentFullDetails.iLanguage.Copy( content->iLanguage );
			aContentFullDetails.iRatingAge = content->iRatingAge;
			aContentFullDetails.iDescription.Copy( content->iDescription );
			aContentFullDetails.iContentProtection = content->iContentProtection;
			aContentFullDetails.iAuthor.Copy( content->iAuthor );
			aContentFullDetails.iCopyright.Copy( content->iCopyright );
			}
		}
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CIptvSearchManager::ResetGlobalId
//
// -----------------------------------------------------------------------------
TInt CIptvSearchManager::ResetGlobalId( CIptvMyVideosGlobalFileId& aId )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::ResetGlobalId()");
	
	TInt error = KErrNone;
	TUint32 contentIdForMatch = 0;
	TInt i = 0;
	for( ; i < iMediaContents.Count(); i++ )
		{
		CIptvContentIdMediaContentItem* contentItem = iMediaContents[i];
		error = contentItem->ResetGlobalId( aId );
		if( error == KErrNone )
			{
			// Store the content id of the item we found the match to
			// be used to update parent details as well
			contentIdForMatch = contentItem->ContentId();
			i = iMediaContents.Count(); // break; would've done the same
			}
		}
	
	// Update the parent's file id details as well
	if( error == KErrNone )
		{
		for( i = 0; i < iAllContentFullDetails.Count(); i++ )
			{
			CIptvVodContentContentFullDetails* bd = iAllContentFullDetails[i];
			if( bd->iContentId == contentIdForMatch )
				{
				bd->iFileId->iFileId = 0;
				bd->iFileId->iDrive = 0;
				i = iAllContentFullDetails.Count(); // break; would've done the same
				}
			}
		}
	return error;
	}

// -----------------------------------------------------------------------------
// CIptvSearchManager::SetGlobalId
//
// -----------------------------------------------------------------------------
TInt CIptvSearchManager::SetGlobalId( TUint32 aContentKey,
					  				  CIptvMyVideosGlobalFileId& aId,
					  				  TUint32 aIndex )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::SetGlobalId()");

	TInt error = KErrNotFound;
	TUint32 contentIdForMatch = 0;
	TInt i = 0;
	for( ; i < iMediaContents.Count(); i++ )
		{
		CIptvContentIdMediaContentItem* contentItem = iMediaContents[i];
		error = contentItem->SetGlobalId( aContentKey, aId, aIndex );
		if( error == KErrNone )
			{
			contentIdForMatch = contentItem->ContentId();
			i = iMediaContents.Count(); // break; would've done the same thing
			}
		}
		
	// Update parent's file id details
	if( error == KErrNone )
		{
		for( i = 0; i < iAllContentFullDetails.Count(); i++ )
			{
			CIptvVodContentContentFullDetails* bd = iAllContentFullDetails[i];
			if( bd->iContentId == contentIdForMatch )
				{
				bd->iFileId->iFileId = aId.iFileId;
				bd->iFileId->iDrive = aId.iDrive;
				i = iAllContentFullDetails.Count(); // break; would've done the same
				}
			}
		}
	return error;
	}
	
// -----------------------------------------------------------------------------
// CIptvSearchManager::CheckIsPluginRunning
//
// -----------------------------------------------------------------------------
void CIptvSearchManager::CheckIsPluginRunning()
    {
    IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::CheckIsPluginRunning");  

    if ( ! iPluginIsRunning )
        {
        iUpdater->Stop();
        delete iRssPlugin;
        iRssPlugin = NULL;
        if ( ! iParent.IsPluginRunning() )
            {
            REComSession::FinalClose();
            }
        }
    }	

// -----------------------------------------------------------------------------
// CIptvSearchManager::CreateContentFullDetailsFromEpgDataL
//
// -----------------------------------------------------------------------------
CIptvVodContentContentFullDetails* CIptvSearchManager::CreateContentFullDetailsFromEpgDataL(
														CIptvEpgContent& aContent )
	{
	IPTVLOGSTRING_LOW_LEVEL("Search Mgr ## CIptvSearchManager::CreateContentFullDetailsFromEpgDataL()");

	CIptvVodContentContentFullDetails* details = CIptvVodContentContentFullDetails::NewL();
	CleanupStack::PushL( details );
	details->iContentId = aContent.iKey;
	details->iName.Copy( aContent.iName );
	details->iThumbnailPath.Copy( aContent.iIconFilePath );
	details->iPlaytime = aContent.iDuration;
	details->iSize = aContent.iSize;
	details->iPubDate = aContent.iPubDate;
	details->iLanguage.Copy( aContent.iLanguage );
	details->iRatingAge = aContent.iRatingAge;
	details->iDescription.Copy( aContent.iDescription );
	details->iContentProtection = aContent.iContentProtection;
	details->iAuthor.Copy( aContent.iAuthor );
	details->iCopyright.Copy( aContent.iCopyright );
	details->SetBrowserUrlL( aContent.iBrowserUrl );
	CleanupStack::Pop( details );
	return details;
	}

// -----------------------------------------------------------------------------
// CIptvSearchManager::ResetContentKey
//
// -----------------------------------------------------------------------------
void CIptvSearchManager::ResetContentKey()
    {
    SetToNextAvailableContentKey( KIptvRssSearchInitialContentId );
    
    IPTVLOGSTRING2_LOW_LEVEL("Search Mgr ## CIptvSearchManager::ResetContentKey() resetted to value %d.", iContentKey);
    }

// -----------------------------------------------------------------------------
// CIptvSearchManager::IncrementContentKey
//
// -----------------------------------------------------------------------------
void CIptvSearchManager::IncrementContentKey()
    {
    SetToNextAvailableContentKey( iContentKey + 1 );

    IPTVLOGSTRING2_LOW_LEVEL("Search Mgr ## CIptvSearchManager::IncrementContentKey() incremented to value %d.", iContentKey);
    }

// -----------------------------------------------------------------------------
// CIptvSearchManager::GetContentKey
//
// -----------------------------------------------------------------------------
TUint32 CIptvSearchManager::GetContentKey()
    {
    return iContentKey;
    }

// -----------------------------------------------------------------------------
// CIptvSearchManager::SetToNextAvailableContentKey
//
// -----------------------------------------------------------------------------
void CIptvSearchManager::SetToNextAvailableContentKey( TUint32 aBase )
    {
    TBool found = EFalse;
    iContentKey = aBase;

    while ( ! found )
        {
        found = ETrue;
        for ( TInt i = 0; i < iUsedContentKeys.Count(); i++ )
            {
            if ( iContentKey == iUsedContentKeys[i] )
                {
                found = EFalse;
                break;
                }
            }
            
        if ( ! found )
            {
            iContentKey++;
            }
        }
    }

// End of file.
