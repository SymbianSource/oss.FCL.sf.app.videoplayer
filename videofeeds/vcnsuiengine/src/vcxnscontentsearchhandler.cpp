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
* Description:    Content search functionality*
*/




// INCLUDE FILES
#include <bldvariant.hrh>
#include "IptvDebug.h"
#include "TIptvRssSearchQuery.h"

#include "CIptvVodContentClient.h"
#include "vcxnscontentsearchobserver.h"
#include "vcxnscontentclienthandler.h"
#include "vcxnscontentsearchhandler.h"
#include "vcxnsuiengine.h"
#include "vcxnscategory.h"
#include "vcxnscategoryprovider.h"
#include "vcxnscontentprovider.h"
#include "vcxnsserviceprovider.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::CVcxNsContentSearchHandler()
// -----------------------------------------------------------------------------
//
CVcxNsContentSearchHandler::CVcxNsContentSearchHandler( 
    CVcxNsUiEngine& aUiEngine ) 
:   iUiEngine( aUiEngine )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsContentSearchHandler* CVcxNsContentSearchHandler::NewL( 
    CVcxNsUiEngine& aUiEngine )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsContentSearchHandler::NewL()");
    
    return new (ELeave) CVcxNsContentSearchHandler( aUiEngine );
    }

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::~CVcxNsContentSearchHandler()
// -----------------------------------------------------------------------------
//
CVcxNsContentSearchHandler::~CVcxNsContentSearchHandler()
    {
    iSearchObservers.Reset();
    }

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::StartNewSearchL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentSearchHandler::StartNewSearchL()
    {
    for (TInt i = 0; i < iSearchObservers.Count(); i++ )
        {
        iSearchObservers[i]->StartSearchL( EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::SearchL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentSearchHandler::SearchL( const TDesC& aSearchString )
    {
    CIptvVodContentClient* contentClient ( 
        iUiEngine.GetContentClientHandlerL()->GetVodContentClientL( 
                                             iUiEngine.ActiveService()));
    if ( contentClient )
        {
        TIptvRssSearchQuery query;
        // Set search string does not really change the given descriptor.
        query.SetSearchString( const_cast<TDesC&>( aSearchString ) );
        contentClient->Search( query );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::CancelSearchL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentSearchHandler::CancelSearchL()
    {
    CIptvVodContentClient* contentClient ( 
        iUiEngine.GetContentClientHandlerL()->GetVodContentClientL( 
                                             iUiEngine.ActiveService()));
    if ( contentClient )
        {
        contentClient->CancelSearch( );
        }
    
    UpdateCategoryVideoCountL();
    SearchCancelled();
    }

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::HandleSearchMsgL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentSearchHandler::HandleSearchMsgL( TInt aStatus, TInt aInfo )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsContentSearchHandler::HandleSearchMsgL()");
    
    TVcxNsAppState appState = iUiEngine.VcAppState();
    
    switch ( aStatus )
        {
        case KIptvErrorRssSearchNoIap: 
            {
            //KIptvErrorRssSearchNoIap was handled earlier, and if status comes
            //to this point, start search again.
            IPTVLOGSTRING_HIGH_LEVEL("UI Engine ##  IAP selected, start search again");
            for (TInt i = 0; i < iSearchObservers.Count(); i++ )
                {
                iSearchObservers[i]->StartSearchL( ETrue );
                }
            }
            break;
        case KIptvErrorRssSearchStarted:
            {
            IPTVLOGSTRING_HIGH_LEVEL("UI Engine ##  Search Started ");
            ShowInfoPopupL( EFalse );
            
            for (TInt i = 0; i < iSearchObservers.Count(); i++ )
                {
                iSearchObservers[i]->SearchStartedL();
                }

            //When new search has started, empty existing search result list.
            if ( iUiEngine.ContentProvider() )
                {
                iUiEngine.ContentProvider()->RemoveCategoryData( 
                        iUiEngine.ActiveService(), KIptvRssSearchCategoryId );
                }
                
            if ( appState == EStateContentView &&
                 iUiEngine.ContentProvider() &&                                           
                 iUiEngine.ActiveCategory() == KIptvRssSearchCategoryId )
                {
                iUiEngine.ContentProvider()->RefreshView();
                }
            }
            break;
            
        case KIptvErrorRssSearchFailed:
            {
            IPTVLOGSTRING_HIGH_LEVEL("UI Engine ##  Search Failed ");
            ShowInfoPopupL( EFalse );
            
            for (TInt i = 0; i < iSearchObservers.Count(); i++ )
                {
                iSearchObservers[i]->HandleSearchResultL( EFalse );
                }            
            }
            break;
            
        case KIptvErrorRssSearchSucceed:
            {
            IPTVLOGSTRING_HIGH_LEVEL("UI Engine ##  Search Succeeded ");
            ShowInfoPopupL( EFalse );
            
            if ( appState == EStateContentView &&
                 iUiEngine.ContentProvider() &&
                 iUiEngine.ActiveCategory() == KIptvRssSearchCategoryId )
                {
                iUiEngine.ContentProvider()->UpdateIconsPathL();
                iUiEngine.ContentProvider()->RefreshView();
                }
            }
            break;

        case KIptvRssSearchContentUpdateCompleted:
            {
            IPTVLOGSTRING_HIGH_LEVEL("UI Engine ##  Search content update completed ");

            //Remove category data again to make sure that we have valid cache for search results
            if ( iUiEngine.ContentProvider() )
                {
                iUiEngine.ContentProvider()->RemoveCategoryData( iUiEngine.ActiveService(),
                                                                 KIptvRssSearchCategoryId );
                }

            //Category state: Switch to content state, set search category as active category
            //Content state: List contents (refresh view) with search results 
            
            for (TInt i = 0; i < iSearchObservers.Count(); i++ )
                {
                iSearchObservers[i]->HandleSearchResultL( ETrue );
                }
            
            if ( appState == EStateCategoryView )
                {
                iUiEngine.SetActiveCategory ( KIptvRssSearchCategoryId );        
                iUiEngine.SetVcAppState( EStateContentView );
                iUiEngine.CategoryProvider()->HandleAppStateChangedL();
                }
            else if ( appState == EStateCustomView  )                
                {
                iUiEngine.SetActiveCategory ( KIptvRssSearchCategoryId );        
                iUiEngine.SetVcAppState( EStateContentView );
                iUiEngine.ServiceProvider()->HandleAppStateChangedL();                
                }
            else if ( appState == EStateContentView )
                {
                iUiEngine.ContentProvider()->RefreshView();
                }

            UpdateCategoryVideoCountL();
            }
            break;

        case KIptvRssSearchThumbnailCount:
            {
            IPTVLOGSTRING2_HIGH_LEVEL("UI Engine ##  Search thumbnail count %d", aInfo);
            iThumbnailCount = aInfo;
            ShowInfoPopupL( ETrue );
            }
            break;

        case KIptvRssSearchContentThumbnailDownloaded:
            {
            iThumbnailsDownloaded++;
            IPTVLOGSTRING2_HIGH_LEVEL("UI Engine ##  Search thumbnail %d downloaded", iThumbnailsDownloaded);
            ShowInfoPopupL( ETrue );
            }
            break;

        default:
            break;
        }

    }

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::RegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxNsContentSearchHandler::RegisterObserver( MVcxNsContentSearchObserver* aObserver )
    {
    // should use find instead of loop
    for ( TInt i = 0; i < iSearchObservers.Count(); i++ )
        {
        if( iSearchObservers[i] == aObserver )
            {
            return;
            }
        }
    
    iSearchObservers.Append( aObserver );
    }

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::DeRegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxNsContentSearchHandler::DeRegisterObserver( MVcxNsContentSearchObserver* aObserver )
    {
    // should use find instead of loop
    for ( TInt i = 0; i < iSearchObservers.Count(); i++ )
        {
        if( iSearchObservers[i] == aObserver )
            {
            iSearchObservers.Remove( i );
            iSearchObservers.Compress();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentSearchHandler::UpdateCategoryVideoCountL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentSearchHandler::UpdateCategoryVideoCountL( )
    {
    //Category's video count needs to be updated to keep search 
    //category's video count in UI up to date.
    //Other categories are updated during normal content update.
    CVcxNsCategory* activeCategory =  
        iUiEngine.GetCategoryProviderL()->GetActiveCategoryDataL();
    
    if ( activeCategory )
        {
        TInt videoCount = iUiEngine.GetContentProviderL()->GetContentsL().Count();
        activeCategory->SetVideoCount( videoCount );
        iUiEngine.CategoryProvider()->UpdateActiveCategoryDataL( *activeCategory );
        }
    }

// -----------------------------------------------------------------------------
// Shows thumbnail loading pop-up
// -----------------------------------------------------------------------------
//
void CVcxNsContentSearchHandler::ShowInfoPopupL( TBool aVisible )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentSearchHandler::ShowInfoPopupL");
    if ( !aVisible )
        {
        // Hiding pop-up, so reset the values
        iThumbnailCount = 0;
        iThumbnailsDownloaded = 0;
        }

    if ( iUiEngine.VcAppState() == EStateContentView && 
            iUiEngine.ActiveCategory() == KIptvRssSearchCategoryId )
        {
        if (  iUiEngine.ContentProvider() && 
             ( !iUiEngine.ServiceProvider() ||
               !iUiEngine.ServiceProvider()->GetActiveServiceData() ||
                iUiEngine.ServiceProvider()->GetActiveServiceData()->GetUpdateStatus() 
                != CVcxNsService::EUpdateOngoing ) )
            {
            iUiEngine.ContentProvider()->ShowUpdatingNoteL( 
                aVisible, iThumbnailCount, iThumbnailsDownloaded );
            }
        }
    }

// -----------------------------------------------------------------------------
// Search cancelled by user
// -----------------------------------------------------------------------------
//
void CVcxNsContentSearchHandler::SearchCancelled( )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentSearchHandler::SearchCancelled()");
    if ( iUiEngine.VcAppState() == EStateCustomView && 
         iUiEngine.ActiveCategory() == KIptvRssSearchCategoryId )
        {
        iUiEngine.SetActiveService( iUiEngine.ActiveServiceGroup() );
        iUiEngine.SetActiveServiceGroup( KMaxTUint );        
        }
    }
