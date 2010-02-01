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
* Description:    HG VOD list base class implementation*
*/



#include <ganes/HgScroller.h>
#include <aknnotewrappers.h>
#include <browserlauncher.h>
#include <browseroverriddensettings.h>
#include "CIptvUtil.h"
#include <uriutils.h>
#include <StringLoader.h>
#include "vcxnsuiengine.h"
#include <vcxhgvodui.rsg>
#include <akntitle.h>
#include <aknViewAppUi.h>

#include "vcxhgvodlistimplbase.h"
#include "vcxhgvodmainview.h"
#include "vcxhgvodutils.h"
#include "vcxhgvodinformationpopup.h"
#include "vcxhgvodsearchimpl.h"
#include "vcxnsserviceproviderif.h"

/* Parameter for embedded browser launch */
_LIT(KUrlLaunchParameter, "4 ");
/* comment constant */
const TInt KThumbnailDownloadTextNumberCount( 2 );

const TInt KVcxMskPosition = 3;

// -----------------------------------------------------------------------------
// CVcxHgVodListImplBase::CVcxHgVodListImplBase()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodListImplBase::CVcxHgVodListImplBase( 
    CVcxHgVodMainView& aView, 
    CVcxNsUiEngine& aModel, 
    CHgScroller& aScroller )
: iView ( aView ),
  iModel ( aModel ),
  iScroller ( aScroller )
    {
    iScroller.SetFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
    iScroller.SetMopParent( &aView );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodListImplBase::~CVcxHgVodListImplBase()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodListImplBase::~CVcxHgVodListImplBase()
    {
    delete iInformationPopup;
    delete iLauncher;
    delete iSearchImpl;
    iView.AppUi()->RemoveFromStack( &iScroller );
    }

// -----------------------------------------------------------------------------
// Open browser in embedded mode with specified uri
// -----------------------------------------------------------------------------
//
void CVcxHgVodListImplBase::OpenEmbeddedBrowserL( const TDesC& aUri )
    {
    if ( aUri.Length() > 0 )
        {
        CUri8* inetUri = NULL;
        inetUri = UriUtils::CreateUriL( aUri );
        CleanupStack::PushL( inetUri );

        HBufC8* parameter = HBufC8::NewLC( inetUri->Uri().UriDes().Length() ); 
        parameter->Des().Format( _L8( "%S" ), &inetUri->Uri().UriDes() );

        HBufC* parsed = HBufC::NewLC( parameter->Length() );                 
        parsed->Des().Copy(*parameter);
                         
        if ( !iLauncher )
            {
            iLauncher = CBrowserLauncher::NewL();
            } 

        HBufC* urlToLaunch = HBufC::NewLC( 
            parsed->Length() + KUrlLaunchParameter.iTypeLength );
        urlToLaunch->Des().Append( KUrlLaunchParameter );
        urlToLaunch->Des().Append( *parsed );

        //Get available iap and pass it to browser 
        TUint32 wap = iModel.GetAvailableWapIdL();
        if ( wap == 0 )
            {
            User::Leave( KErrCouldNotConnect );
            }
        TBrowserOverriddenSettings overriddenSettings;
        overriddenSettings.SetBrowserSetting( EBrowserOverSettingsCustomAp, wap );

        // Check, that the state has not been changed while launching
        if ( iModel.VcAppState() == EStateBrowser )
            {
            iLauncher->LaunchBrowserEmbeddedL( *urlToLaunch, NULL, this, &overriddenSettings );
            }

        CleanupStack::PopAndDestroy( urlToLaunch );
        CleanupStack::PopAndDestroy( parsed );
        CleanupStack::PopAndDestroy( parameter ); 
        CleanupStack::PopAndDestroy( inetUri );
        }
    }

// -----------------------------------------------------------------------------
// Close browser.
// -----------------------------------------------------------------------------
// 
void CVcxHgVodListImplBase::CloseEmbeddedBrowser()
    {
    //Note: Don't do this unless you know this app has started browser.
    if ( iLauncher )
        {
        iLauncher->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// Set content for model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodListImplBase::HandleServerAppExit( TInt /*aReason*/ )
    {
    //Back is called that we can return from EBrowser state to last state
    iModel.HandleBack();    
    TRAP_IGNORE( iView.ChangeListViewL() );
    }

// -----------------------------------------------------------------------------
// Set text for title pane
// -----------------------------------------------------------------------------
// 
void CVcxHgVodListImplBase::SetTitlePaneTextL( const TDesC& aTitle )
    {
    iView.TitlePaneL()->SetTextL( aTitle );
    }

// -----------------------------------------------------------------------------
// Set empty list widget text
// -----------------------------------------------------------------------------
//
void CVcxHgVodListImplBase::SetTitlePaneTextFromResourceL( TInt aResourceId )
    {
    HBufC* title = StringLoader::LoadLC( aResourceId );
    SetTitlePaneTextL( *title );
    CleanupStack::PopAndDestroy( title );
    }

// -----------------------------------------------------------------------------
// Set empty list widget text
// -----------------------------------------------------------------------------
// 
void CVcxHgVodListImplBase::SetEmptyListTextL( TInt aResourceId )
    {
    HBufC* emptylistText =
        CVcxHgVodUtils::LoadStringFromResourceLC( aResourceId );
    iScroller.SetEmptyTextL( *emptylistText );
    iScroller.DrawDeferred();
    CleanupStack::PopAndDestroy( emptylistText );
    }

// -----------------------------------------------------------------------------
// Set menu
// -----------------------------------------------------------------------------
// 
void CVcxHgVodListImplBase::SetMenuResourceId( TInt aResourceId )
    {
    iView.MenuBar()->SetMenuTitleResourceId( aResourceId );
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::ShowInformationPopupL( )
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::ShowInformationPopupL( 
    TBool aShow, 
    TInt aDownloadedTbns, 
    TInt aTotalTbns )
    {
    if( aShow && 
         iModel.VcAppState() != EStateServiceView  )
        {
        if( aDownloadedTbns == 0 && aTotalTbns == 0 )
            {       
            ShowInformationPopupL();
            }
        else if( aDownloadedTbns == 1 && aTotalTbns == 1 )
            {
            HBufC* thumbnailtext = StringLoader::LoadLC( R_VCXHG_UPDATED_THUMBNAILS_ONE );
            SetInfoPopupTextL( *thumbnailtext );
            CleanupStack::PopAndDestroy( thumbnailtext );
            ShowInformationPopupL();
            }
        else 
            {
            CArrayFix<TInt>* numbers = 
                new (ELeave) CArrayFixFlat<TInt>( KThumbnailDownloadTextNumberCount ); 
            CleanupStack::PushL( numbers );
            
            numbers->AppendL( aTotalTbns );
            numbers->AppendL( aDownloadedTbns );

            HBufC* thumbnailtext = StringLoader::LoadLC( R_VCXHG_UPDATED_THUMBNAILS, *numbers );
            SetInfoPopupTextL( *thumbnailtext );
            
            CleanupStack::PopAndDestroy( thumbnailtext );
            CleanupStack::PopAndDestroy( numbers );

            ShowInformationPopupL();
            }
        }
    else
        {
        HideInformationPopup();
        }
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::ShowInformationPopupL( )
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::ShowInformationPopupL()
    {
    if ( iModel.VcAppState() != EStateNoState && iView.IsForeground() )
        {
        if ( !iInformationPopup )
           {
           iInformationPopup = CVcxHgVodInformationPopup::NewL( iView, iModel );
           }
            
        iInformationPopup->ShowL(); 
        }
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::SetTextL( )
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::SetInfoPopupTextL( const TDesC& aInformationText )
    {
    if ( iInformationPopup )
        { 
        iInformationPopup->SetTextL( aInformationText );
        }
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::HideInformationPopup( )
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::HideInformationPopup()
    {
    if ( iInformationPopup )
        { 
        iInformationPopup->Hide();
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgVodListImplBase::HandleSizeChange()
    {
    if ( iInformationPopup )
        { 
        iInformationPopup->HandleSizeChange();
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgVodListImplBase::HandleForegroundEventL( TBool aForeground )
    {
    if ( iInformationPopup )
        { 
        iInformationPopup->HandleForegroundEventL( aForeground );
        }
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::SearchImplL()
// ---------------------------------------------------------
//
CVcxHgVodSearchImpl* CVcxHgVodListImplBase::SearchImpl()
    {
    return iSearchImpl;
    }


// ---------------------------------------------------------
// CVcxHgVodListImplBase::InitSearchImplL()
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::InitSearchImplL()
    {
    if ( !iSearchImpl )
        {
        iSearchImpl = CVcxHgVodSearchImpl::NewL( iModel, iView );
        }
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::ShowUpdateErrorNoteL()
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::ShowUpdateErrorNoteL( TInt aError )
    {
    HBufC* errText  = NULL;
    
    switch ( aError )
        {
        case KIptvRssParserError:
            errText = StringLoader::LoadLC( R_VCXHG_INVALID_DATA_RECEIVED );
            break;
        case EIptvDlConnectionFailed:
            errText = StringLoader::LoadLC( R_VCXHG_INVALID_ACCESS_POINT );
            break;
        case EIptvDlAuthFailed:
            errText = StringLoader::LoadLC( R_VCXHG_AUTH_FAILED );
            break;
        case EIptvDlBadUrl:
        case EIptvDlContentNotFound:
            errText = StringLoader::LoadLC( R_VCXHG_INVALID_ADDRESS );            
            break;
        case EIptvDlDiskFull:
            {
            errText = StringLoader::LoadLC( R_VCXHG_MEMORY_FULL );
            CAknQueryDialog* dlg = CAknQueryDialog::NewL( );
            dlg->ExecuteLD( R_VCXHG_CONFIRMATION_QUERY, *errText );
            CleanupStack::PopAndDestroy( errText );
            errText = NULL;
            }
            break;
        case EIptvDlProxyAuthFailed:
        case EIptvDlDestFileInUse:
        default:
            errText = StringLoader::LoadLC( R_VCXHG_CONTENT_UPDATE_FAILED );
            break;
        }

    if ( errText ) 
        {
        // Note: We need to show the note synchronously if we are handling
        // authentication problem. Otherwise program would crash when this
        // note and authentication query mix each other.
        if ( aError == EIptvDlAuthFailed || aError == EIptvDlConnectionFailed )
            {
            ShowErrorNoteL( *errText, ETrue );
            }
        else
            {
            ShowErrorNoteL( *errText );          
            }
        
        CleanupStack::PopAndDestroy( errText ); 
        }
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::HandleAuthenticationL()
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::HandleAuthenticationL(
    TBool aEpgUpdateFailed,
    CVcxNsContent* aContent )
    {
    HBufC* username = HBufC::NewLC( KIptvEpgServiceUserNameMaxLength );
    TPtr usernamePtr = username->Des();
    HBufC* passwd = HBufC::NewLC( KIptvEpgServicePasswordMaxLength );
    TPtr passwdPtr = passwd->Des();
    
    CAknMultiLineDataQueryDialog* dlg = 
        CAknMultiLineDataQueryDialog::NewL( usernamePtr, passwdPtr );    
    
    TInt dlgRes = dlg->ExecuteLD( R_VCXHG_AUTHENTICATION_QUERY );
    
    if ( !dlgRes )
        {
        User::Leave( KErrCancel );
        }

    if ( aEpgUpdateFailed )
        {
        MVcxNsServiceProviderIf* serviceProvider = iModel.GetServiceProviderL();
        if ( serviceProvider )
            {
            serviceProvider->SetAuthenticationDataL( *username, *passwd );
            }
        }
    else
        {
        if ( aContent )
            {
            aContent->SetUsernameL( *username );
            aContent->SetPasswordL( *passwd );
            }
        }

    CleanupStack::PopAndDestroy( passwd );
    CleanupStack::PopAndDestroy( username );
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::ShowErrorNoteL
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::ShowErrorNoteL( 
    const TDesC& aErrorText, 
    TBool aWaiting )
    {
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote( aWaiting );
    note->ExecuteLD( aErrorText );
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::SetMskL
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::SetMskL( TInt aResourceId )
    {
    if ( aResourceId != iCurrentMskResource )
        {
        iCurrentMskResource = aResourceId;
        if ( iView.ViewCba() )
            {
            iView.ViewCba()->SetCommandL( KVcxMskPosition, iCurrentMskResource );
            iView.ViewCba()->DrawDeferred();
            }
        }
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::HandleKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CVcxHgVodListImplBase::HandleKeyEventL( 
    const TKeyEvent& /*aEvent*/, 
    TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::HandleSelectL
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::HandleSelectL( TInt /*aIndex*/ )
    {
    // empty default impl.
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::HandleOpenL
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::HandleOpenL( TInt /*aIndex*/ )
    {
    // empty default impl.
    }

// ---------------------------------------------------------
// CVcxHgVodListImplBase::SetFocus
// ---------------------------------------------------------
//
void CVcxHgVodListImplBase::SetFocus( TBool aFocus )
    {
    if ( !iScroller.IsFocused() )
        {
        iScroller.SetFocus( aFocus );
        }
    }
