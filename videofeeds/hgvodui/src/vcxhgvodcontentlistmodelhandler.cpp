/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    HG VOD*
*/




#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <vcxhgvodui.rsg>
#include <ganes/HgScroller.h>
#include <ganes/HgItem.h>
#include <ganes/HgDoubleGraphicList.h>

#include "CIptvUtil.h"
#include "vcxhgvodcontentlistimpl.h"
#include "vcxhgvodcontentlistmodelhandler.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxnsuiengine.h"
#include "vcxnsserviceproviderif.h"
#include "vcxnscategoryproviderif.h"
#include "vcxnscontentproviderif.h"
#include "vcxnsservice.h"
#include "vcxnscategory.h"
#include "vcxnscontent.h"
#include "vcxhgvodutils.h"
#include "vcxhgvodui.hrh"
#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodvideodetailsdialog.h"
#include "vcxhgvodthumbnailmanager.h"

const TInt KFullPercents = 100;
const TInt KFullPercentsToList = 99;

TInt FindContentIndex( TUint32 aId, RPointerArray<CVcxNsContent>* aArray )
    {
    TInt index( KErrNotFound );
    if ( aArray )
        {
        for ( TInt i = 0; i < aArray->Count(); i++ )
            {
            if ( (*aArray)[i]->GetContentId() == aId )
                {
                index = i;
                break;
                }
            }
        }
    
    return index;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListModelHandler::~CVcxHgVodServiceListModelHandler()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodContentListModelHandler::~CVcxHgVodContentListModelHandler()
    {
    if ( iContentProvider )
        {
        iContentProvider->DeRegisterObserver( this );
        }
    
    delete iTnManager;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgVodContentListModelHandler* CVcxHgVodContentListModelHandler::NewL( 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodContentListImpl& aContentListImpl )
    {
    CVcxHgVodContentListModelHandler* self = 
        CVcxHgVodContentListModelHandler::NewLC( 
                    aDataProviderIf, 
                    aView,
                    aScroller, 
                    aContentListImpl );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListModelHandler::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodContentListModelHandler* CVcxHgVodContentListModelHandler::NewLC(
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodContentListImpl& aContentListImpl )
    {
    CVcxHgVodContentListModelHandler* self = 
        new (ELeave) CVcxHgVodContentListModelHandler( 
                aDataProviderIf, aView, aScroller, aContentListImpl );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListModelHandler::CVcxHgVodContentListModelHandler()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodContentListModelHandler::CVcxHgVodContentListModelHandler(
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodContentListImpl& aContentListImpl )
    : iDataProviderIf ( aDataProviderIf ),
      iView ( aView ),
      iScroller ( aScroller ), 
      iListImpl ( aContentListImpl )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListModelHandler::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListModelHandler::ConstructL()
    {
    iContentProvider = iDataProviderIf.GetContentProviderL();
    iTnManager = CVcxHgVodThumbnailManager::NewL( iView.EikonEnv().FsSession() );
    }

// -----------------------------------------------------------------------------
// Activate model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::DoActivateModelL()
    {
    iContentProvider->RegisterObserver( this );
    iScroller.EnableScrollBufferL( *this, KScrollerBufferSize, KScrollerBufferTreshold ); 
    iScroller.Reset();
    iScroller.ControlEnv()->WsSession().ClearAllRedrawStores();
    iScroller.MakeVisible( ETrue );
    iScroller.DrawNow();
    iScroller.ControlEnv()->WsSession().Flush();
    
    TRAP_IGNORE( SetListContentL() );
    }

// -----------------------------------------------------------------------------
// Deactivate model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::DoDeActivateModel( )
    {
    if ( iContentProvider )
        {
        iContentProvider->SetContentHighlight( iScroller.SelectedIndex() );
        iContentProvider->DeRegisterObserver( this );
        }
    
    iTnManager->Reset();
    iScroller.DisableScrollBuffer();
    iScroller.MakeVisible( EFalse );
    }

// -----------------------------------------------------------------------------
// Set content for model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::SetListContentL()
    {
    TInt highlight = 0;
    iContents = NULL;
    iContents = &(iContentProvider->GetContentsL( highlight ));
    if ( iContents )
        {
        RefreshViewL();
        
        if ( iContents->Count() > 0 )
            {
            iScroller.SetSelectedIndex( highlight );
            }
        }

    UpdateViewDetailsL();
    }

// -----------------------------------------------------------------------------
//  Get category provider interface
// -----------------------------------------------------------------------------
// 
MVcxNsContentProviderIf* CVcxHgVodContentListModelHandler::ContentProvider()
    {
    return iContentProvider;
    }
 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::ContentUpdated( TInt aIndex )
    {
    TRAP_IGNORE( UpdateListItemL( aIndex ) );
    TRAP_IGNORE( HandleCommandUpdatesL() );
    }

// -----------------------------------------------------------------------------
// Handle command updates
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::HandleCommandUpdatesL()
    {
    iListImpl.CheckMskL();
    }

// -----------------------------------------------------------------------------
// Refresh view, all contents updated
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::RefreshView()
    {
    if ( iDataProviderIf.VcAppState() == EStateContentView )
        {
        iContents = NULL;
        TRAP_IGNORE( iContents = &(iContentProvider->GetContentsL()) );
        if ( iContents )
            {
            TInt highlight = iScroller.SelectedIndex();
            
            TRAP_IGNORE( RefreshViewL() );
            
            if ( iScroller.ItemCount() > 0 )
                {
                if ( highlight < 0 )
                    {
                    highlight = 0;
                    }
                else if ( highlight >= iScroller.ItemCount() )
                    {
                    highlight = iScroller.ItemCount() - 1;
                    }

                iScroller.SetSelectedIndex( highlight );
                }
            }
        
        TRAP_IGNORE( UpdateViewDetailsL() );
        }
    }

// -----------------------------------------------------------------------------
// Update view details
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::UpdateViewDetailsL()
    {
    iListImpl.UpdateTitlePaneL();
    iListImpl.CheckMskL();
    
    if ( iContents && iContents->Count() <= 0 
            && iDataProviderIf.ActiveCategory() == KVcxNsSearchCategoryId )
        {
        //When activating search list view and there's no content, 
        //search result is 0 and show no videos found.
        iListImpl.SetEmptyListTextL( R_VCXHG_NO_VIDEOS_FOUND );
        }
    else if ( iContents && iContents->Count() <= 0 )
        {
        iListImpl.SetEmptyListTextL( R_VCXHG_VOD_NO_DATA );
        }
    }

// -----------------------------------------------------------------------------
// Application state change initialized by data provider.
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::HandleAppStateChangedL()
    {
    iView.ChangeListViewL();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListModelHandler::ShowUpdatingNoteL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListModelHandler::ShowUpdatingNoteL( 
    TBool aShow,
    TInt aDownloadedTbns, 
    TInt aTotalTbns )
    {
    iListImpl.ShowInformationPopupL( aShow, aDownloadedTbns, aTotalTbns );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListModelHandler::HandleUpdateErrorL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListModelHandler::HandleUpdateErrorL( TInt aError )
    {
    iListImpl.ShowUpdateErrorNoteL( aError );
    
    if ( aError == EIptvDlAuthFailed )
        {
        TRAPD( err, iListImpl.HandleAuthenticationL( ETrue ) );
        
        if ( err == KErrNone && iContentProvider )
            {
            //if no error in authentication, refresh contents.
            iContentProvider->RefreshContentsL();
            return;
            }
        }

    if ( iScroller.ItemCount() <= 0 )
        {
        iListImpl.SetEmptyListTextL( R_VCXHG_VOD_NO_DATA );
        }
    
    iScroller.DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodContentListModelHandler::HandleDownloadErrorL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListModelHandler::HandleDownloadErrorL( 
    TIptvDlError aError,
    CVcxNsContent& aContent )
    {
    // 
    // EIptvDlConnectionFailed - Is this handling needed for UI side anymore as connection is 
    //                           handled in collection side? Is Resume done in collection side also?    
    
    HBufC* textFromResource = NULL;

    switch ( aError )
        {
        case EIptvDlContentNotFound:
            textFromResource = StringLoader::LoadLC( R_VCXHGVOD_ERR_VIDEO_MISSING_FROM_SERVER );
            break;
            
        case EIptvDlAuthFailed:
            iListImpl.HandleAuthenticationL( EFalse, &aContent );
            iContentProvider->ResumeDownloadL( aContent );
            break;
        case EIptvDlDiskFull:
            {
            textFromResource = StringLoader::LoadLC( R_VCXHGVOD_ERR_MEMORY_FULL );
            CAknQueryDialog* dlg = CAknQueryDialog::NewL( );
            dlg->ExecuteLD( R_VCXHG_CONFIRMATION_QUERY, *textFromResource );
            CleanupStack::PopAndDestroy( textFromResource );
            textFromResource = NULL;
            }
            break;

        case EIptvDlBadUrl:
            textFromResource = StringLoader::LoadLC( R_VCXHGVOD_ERR_INVALID_ADDRESS );
            break;

        case EIptvResumingFromBeginning:
            {
            textFromResource = StringLoader::LoadLC( R_VCXHGVOD_RESUME_NOT_POSSIBLE );
            CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
            note->ExecuteLD( *textFromResource );
            CleanupStack::PopAndDestroy( textFromResource );     
            textFromResource = NULL;
            }
            break;
            
        case EIptvDlGeneral:
        default:
            {
            textFromResource = StringLoader::LoadLC( 
                    R_VCXHGVOD_ERR_DOWNLOAD_FAILED, aContent.GetName() );
            }
            break;
        }

    if ( textFromResource )
        {
        iListImpl.ShowErrorNoteL( *textFromResource );
        CleanupStack::PopAndDestroy( textFromResource );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::MvtoThumbnailReady( 
    CGulIcon* aIcon,
    TInt64 aClientId )
    {
    // error ignored, as there is no error handling that can be done if this leaves
    TRAP_IGNORE( DoThumbnailReadyL( aIcon, aClientId ) );
    }

// -----------------------------------------------------------------------------
// Generate text for second line
// -----------------------------------------------------------------------------
//
HBufC* CVcxHgVodContentListModelHandler::GenerateContextInfoTextLC( 
    CVcxNsContent& aContent )
    {
    // FormatLengthAndSizeL could return new HBufC*, no need for preallocated.
    HBufC* formattedString = HBufC::NewLC( 100 ); 
    TPtr formattedStringPtr = formattedString->Des();
    CVcxNsContent::TVcxContentDlStatus dlStatus = aContent.GetDlStatus();

    // If not in download list, size and length to list if available
    if ( ( dlStatus == CVcxNsContent::EVcxContentDlStateNotFound )
         || ( dlStatus == CVcxNsContent::EVcxContentDlStateNotDownloadable ) )
        {
        CVcxHgVodUtils::FormatLengthAndSizeL( 
            aContent.GetLength(), aContent.GetSize(), formattedStringPtr );
        }
    else
        {
        // Content is being downloaded, show the status.
        TInt progress = aContent.GetDlProgress();
        if ( progress >= KFullPercents )
            {
            // always show 99% as highest progress before changing to "downloaded"
            progress = KFullPercentsToList;
            }
        CVcxHgVodUtils::FormatDownloadStateL( dlStatus, progress, 
                                              formattedStringPtr );
        }
    
    return formattedString;
    }

// -----------------------------------------------------------------------------
// Get content information of currently selected video and pass it to video details dialog
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::ShowVideoDetailsL( TInt aSelected )
    {
    if ( iContents && aSelected >= 0 && aSelected < iContents->Count() )
        {
        CVcxNsContent* content = iContentProvider->GetFullDetailsL( aSelected );
        iVideoDetails->ShowVideoDetailsDialogL( *content );
        }  
    }

// -----------------------------------------------------------------------------
// Check if model is empty
// -----------------------------------------------------------------------------
// 
TBool CVcxHgVodContentListModelHandler::IsEmptyModel()
    {
    return iScroller.ItemCount() <= 0;
    }

// -----------------------------------------------------------------------------
// Update list 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::RefreshViewL()
    {
    iScroller.Reset();
    
    if ( iContents && iContents->Count() > 0 )
        {
        iScroller.ResizeL( iContents->Count() );
        }
    iScroller.DrawDeferred();
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListModelHandler::Request( TInt aBufferStart, 
    TInt aBufferEnd, THgScrollDirection /*aDirection*/ )
    {
    for ( TInt i = aBufferStart; i <= aBufferEnd; i++ )
        {
        TRAP_IGNORE( UpdateListItemL( i ) );
        }
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgVodContentListModelHandler::Release( TInt /*aBufferStart*/, 
    TInt /*aBufferEnd*/ )
    {
    // No implementation required.
    }

// -----------------------------------------------------------------------------
// Update single content
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::UpdateListItemL( TInt aIndex )
    {
    if ( iDataProviderIf.VcAppState() == EStateContentView &&
         aIndex >= 0 && iContents && iContents->Count() > aIndex &&
         iScroller.ItemCount() > aIndex )
        {
        CHgItem& item = iScroller.ItemL( aIndex );
        CVcxNsContent& content = *(*iContents)[aIndex];
    
        item.SetTitleL( content.GetName() );
    
        HBufC* info = GenerateContextInfoTextLC( content );    

        if ( info )
            {
            item.SetTextL( *info );
            }
    
        CleanupStack::PopAndDestroy( info );
    
        CVcxNsContent::TVcxContentDlStatus dlStatus = content.GetDlStatus();
    
        if ( !( item.Icon() ) ||
             ( dlStatus != CVcxNsContent::EVcxContentDlStateDownloading ) )
            {
            item.SetIcon( NULL );
            const TDesC& iconPath = content.GetIconPath();

            if ( iconPath.Length() > 0 )
                {
                iTnManager->AddThumbnailRequestL( *this, 
                                                  content.GetIconPath(), 
                                                  CHgDoubleGraphicList::PreferredImageSize(), 
                                                  content.GetContentId() );
                }
            }
        iScroller.RefreshScreen( aIndex );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodContentListModelHandler::DoThumbnailReadyL( 
    CGulIcon* aIcon, 
    TInt64 aClientId )
    {
    CleanupStack::PushL( aIcon );
    TInt index = FindContentIndex( (TUint32)aClientId, iContents );
    
    if ( index >= 0 && iScroller.ItemCount() > index )
        {
        iScroller.ItemL( index ).SetIcon( aIcon );
        iScroller.RefreshScreen( index );
        CleanupStack::Pop( aIcon );
        }
    else
        {
        // For some reason the scroller list was update after image conversion
        // started, and the icon is no longer needed.
        CleanupStack::PopAndDestroy( aIcon );
        }
    }
