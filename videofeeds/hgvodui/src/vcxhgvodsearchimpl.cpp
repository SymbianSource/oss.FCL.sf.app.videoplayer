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
* Description:    HG VOD content search related ui implementation*
*/




#include <aknnotewrappers.h>
#include <AknWaitDialog.h>
#include <StringLoader.h>
#include <vcxhgvodui.rsg>
#include "IptvDebug.h"
#include "vcxhgvodui.hrh"

#include "vcxhgvodutils.h"
#include "vcxhgvodsearchimpl.h"
#include "vcxnscontentsearchif.h"
#include "vcxnsuiengine.h"
#include "vcxhgvodlistimplbase.h"
#include "vcxhgvodmainview.h"

// Search max string length
const TInt KVcxNsSearchMaxSearchStringLength = 255;

// -----------------------------------------------------------------------------
// CVcxHgVodSearchImpl::~CVcxHgVodSearchImpl()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodSearchImpl::~CVcxHgVodSearchImpl()
    {
    delete iCurrentSearchQuery;
    CloseSearchWaitNote();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodSearchImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgVodSearchImpl* CVcxHgVodSearchImpl::NewL(
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodSearchImpl* self = 
        CVcxHgVodSearchImpl::NewLC( aDataProviderIf, aView );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodSearchImpl::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodSearchImpl* CVcxHgVodSearchImpl::NewLC(
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
    {
    CVcxHgVodSearchImpl* self = 
        new (ELeave) CVcxHgVodSearchImpl( aDataProviderIf, aView );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodSearchImpl::CVcxHgVodSearchImpl()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodSearchImpl::CVcxHgVodSearchImpl( 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView )
: iUiEngine( aDataProviderIf ),
  iView( aView )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CVcxHgVodSearchImpl::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodSearchImpl::ConstructL( )
    {
    iSearchHandler = iUiEngine.ContentSearchHandlerL();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodSearchImpl::RegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxHgVodSearchImpl::RegisterObserver( )
    {
    iSearchHandler->RegisterObserver( this );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodSearchImpl::DeRegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxHgVodSearchImpl::DeRegisterObserver( )
    {
    iSearchHandler->DeRegisterObserver( this );
    }

// ----------------------------------------------------
// CVcxHgVodSearchImpl::ShowSearchQueryL
// ----------------------------------------------------
//
void CVcxHgVodSearchImpl::ShowSearchQueryL( TBool aShowPrevious )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "VOD ## CVcxHgVodSearchImpl::ShowSearchQueryL()");
    
    CAknTextQueryDialog* dlg = NULL;
    TInt button = 0;

    HBufC* searchQuery = HBufC::NewLC( KVcxNsSearchMaxSearchStringLength );
    if ( aShowPrevious && iCurrentSearchQuery )
        {
        searchQuery->Des().Copy( *iCurrentSearchQuery );
        }
    
    TPtr tPtr( searchQuery->Des() );
    dlg = CAknTextQueryDialog::NewL( tPtr );

    dlg->SetMaxLength( KVcxNsSearchMaxSearchStringLength );
    dlg->SetPredictiveTextInputPermitted( ETrue );
    button = dlg->ExecuteLD( R_VCXHG_SEARCH_QUERY_DLG );

    if ( button ) 
        {
        //User has pressed OK
        if ( !iCurrentSearchQuery )
            {
            iCurrentSearchQuery = 
                HBufC::NewL( KVcxNsSearchMaxSearchStringLength );
            }

        iCurrentSearchQuery->Des().Copy( *searchQuery );
        iSearchHandler->SearchL( *iCurrentSearchQuery );
        }
    else
        {
        //user cancelled search - in customized service view
        //this case needs to be handled to change active service
        iSearchHandler->SearchCancelled();
        }
    
    CleanupStack::PopAndDestroy( searchQuery );
    }

// ----------------------------------------------------
// CVcxHgVodSearchImpl::DisplaySearchWaitNoteL
// ----------------------------------------------------
//
void CVcxHgVodSearchImpl::DisplaySearchWaitNoteL()
    {
    CloseSearchWaitNote();

    iSearchWaitDialog = new ( ELeave ) CAknWaitDialog (
            REINTERPRET_CAST( CEikDialog**, 
            &iSearchWaitDialog ), 
            ETrue );
    
    iSearchWaitDialog->SetCallback( this ); 
    iSearchWaitDialog->ExecuteLD( R_VCXHG_SEARCHING_WAIT_NOTE );
    }

// ----------------------------------------------------
// CVcxHgVodSearchImpl::CloseSearchWaitNote
// ----------------------------------------------------
//
void CVcxHgVodSearchImpl::CloseSearchWaitNote()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "VOD ## CVcxHgVodSearchImpl::CloseSearchWaitNote()");

    if ( iSearchWaitDialog )
        {
        TRAPD( error, iSearchWaitDialog->ProcessFinishedL() );
        if ( error != KErrNone )
            {
            delete iSearchWaitDialog;
            }
        iSearchWaitDialog = NULL;
        } 
    }

// -----------------------------------------------------------------------------
// Search progress dialog cancelled
// -----------------------------------------------------------------------------
// 
void CVcxHgVodSearchImpl::DialogDismissedL( TInt aButtonId )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "VOD ## CVcxHgVodSearchImpl::DialogDismissedL()");

    if ( aButtonId == EAknSoftkeyCancel )
        {
        iSearchHandler->CancelSearchL();
        }   
    }

// -----------------------------------------------------------------------------
// Handle search result
// -----------------------------------------------------------------------------
// 
void CVcxHgVodSearchImpl::HandleSearchResultL( TBool aSucceeded )
    {
    IPTVLOGSTRING2_LOW_LEVEL(
        "VOD ## CVcxHgVodSearchImpl::HandleSearchResultL( %d )", (TInt)aSucceeded );
    
    CloseSearchWaitNote();
    
    if ( aSucceeded )
        {
        //Delete current search query, no reason to save it now
        //No further actions needed here, content and category providers will handle 
        //content update and update view
        delete iCurrentSearchQuery;
        iCurrentSearchQuery = NULL;
        }
    else
        {
        HBufC* textFromResource = StringLoader::LoadLC( R_VCXHG_ERROR_SEARCH_FAILED );
        CAknErrorNote* note = new (ELeave) CAknErrorNote( ETrue );
        note->ExecuteLD( *textFromResource );
        CleanupStack::PopAndDestroy( textFromResource );
        
        //Show search query with previous string 
        ShowSearchQueryL( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// New search started.
// -----------------------------------------------------------------------------
// 
void CVcxHgVodSearchImpl::SearchStartedL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "VOD ## CVcxHgVodSearchImpl::SearchStartedL()");
    
    DisplaySearchWaitNoteL();
    }

// -----------------------------------------------------------------------------
// Start current search again.
// -----------------------------------------------------------------------------
// 
void CVcxHgVodSearchImpl::StartSearchL( TBool aUseCurrentQuery )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "VOD ## CVcxHgVodSearchImpl::StartSearchL()");
    
    if ( aUseCurrentQuery )
        {
        //IAP has been selected. Send search request again with current search query. 
        if ( iCurrentSearchQuery )
            {
            iSearchHandler->SearchL( *iCurrentSearchQuery );
            }
        }
    else
        {
        TVcxNsAppState appState = iUiEngine.VcAppState();
        if ( appState == EStateContentView )
            {
            //Show (no videos) in empty content view
            if ( iView.ActiveList() )
                {
                iView.ActiveList()->SetEmptyListTextL( R_VCXHG_NO_VIDEOS );
                }
            }

        //Show new search query
        ShowSearchQueryL( EFalse );
        }
    }
