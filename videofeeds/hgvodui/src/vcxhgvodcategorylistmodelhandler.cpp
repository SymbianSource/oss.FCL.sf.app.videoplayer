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
* Description:    HG VOD*
*/




#include "CIptvUtil.h"

#include <ganes/HgScroller.h>
#include <ganes/HgItem.h>
#include <vcxhgvodui.rsg>

#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodcategorylistimpl.h"
#include "vcxhgvodcategorylistmodelhandler.h"
#include "vcxnsuiengine.h"
#include "vcxnscategoryproviderif.h"
#include "vcxnscategory.h"
#include "vcxhgvodutils.h"
#include "vcxhgvodui.hrh"

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListModelHandler::~CVcxHgVodServiceListModelHandler()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodCategoryListModelHandler::~CVcxHgVodCategoryListModelHandler()
    {
    if ( iCategoryProvider )
        {
        iCategoryProvider->DeRegisterObserver( this );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgVodServiceListImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgVodCategoryListModelHandler* CVcxHgVodCategoryListModelHandler::NewL( 
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodCategoryListImpl& aCategoryListImpl )
    {
    CVcxHgVodCategoryListModelHandler* self = 
        CVcxHgVodCategoryListModelHandler::NewLC( 
                    aDataProviderIf, 
                    aView,
                    aScroller, 
                    aCategoryListImpl );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListModelHandler::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgVodCategoryListModelHandler* CVcxHgVodCategoryListModelHandler::NewLC(
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodCategoryListImpl& aCategoryListImpl )
    {
    CVcxHgVodCategoryListModelHandler* self = 
        new (ELeave) CVcxHgVodCategoryListModelHandler( 
                aDataProviderIf, aView, aScroller, aCategoryListImpl );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListModelHandler::CVcxHgVodCategoryListModelHandler()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodCategoryListModelHandler::CVcxHgVodCategoryListModelHandler(
    CVcxNsUiEngine& aDataProviderIf,
    CVcxHgVodMainView& aView, 
    CHgScroller& aScroller,
    CVcxHgVodCategoryListImpl& aCategoryListImpl )
    : iDataProviderIf ( aDataProviderIf ),
      iView( aView ),
      iScroller ( aScroller ),
      iListImpl ( aCategoryListImpl )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListModelHandler::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodCategoryListModelHandler::ConstructL()
    {
    iCategoryProvider = iDataProviderIf.GetCategoryProviderL();
    }

// -----------------------------------------------------------------------------
// Activate model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodCategoryListModelHandler::DoActivateModelL( )
    {    
    iCategoryProvider->RegisterObserver( this );
    iScroller.EnableScrollBufferL( *this, KScrollerBufferSize, KScrollerBufferTreshold );
    iScroller.Reset();
    SetListContentL();
    iScroller.ControlEnv()->WsSession().ClearAllRedrawStores();
    iScroller.MakeVisible( ETrue );
    iScroller.DrawDeferred();
    }

// -----------------------------------------------------------------------------
// Deactivate model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodCategoryListModelHandler::DoDeActivateModel( )
    {
    if ( iCategoryProvider )
        {
        iCategoryProvider->SetCategoryHighlight( iScroller.SelectedIndex() );
        iCategoryProvider->DeRegisterObserver( this );
        }
    
    iScroller.DisableScrollBuffer();
    iScroller.MakeVisible( EFalse );
    }

// -----------------------------------------------------------------------------
// Set content for model
// -----------------------------------------------------------------------------
// 
void CVcxHgVodCategoryListModelHandler::SetListContentL()
    {
    TInt highlight = 0;
    iCategories = NULL;
    iCategories = &(iCategoryProvider->GetCategoriesL( highlight ));

    if ( iCategories )
        {
        RefreshViewL();
        
        if ( iCategories->Count() > 0 )
            {
            iScroller.SetSelectedIndex( highlight );
            }
        }
    }

// -----------------------------------------------------------------------------
//  Get category provider interface
// -----------------------------------------------------------------------------
// 
MVcxNsCategoryProviderIf* CVcxHgVodCategoryListModelHandler::CategoryProvider()
    {
    return iCategoryProvider;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CVcxHgVodCategoryListModelHandler::CategoryUpdated( TInt /*aIndex*/ )
    {

    }

// -----------------------------------------------------------------------------
// Refresh view, all contents updated
// -----------------------------------------------------------------------------
// 
void CVcxHgVodCategoryListModelHandler::RefreshView()
    {
    if ( iDataProviderIf.VcAppState() == EStateCategoryView )
        {
        TRAPD( err, iCategories = &(iCategoryProvider->GetCategorysL()) );
        
        if ( err == KErrNone )
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
        }
    }

// -----------------------------------------------------------------------------
// Application state change initialized by data provider.
// -----------------------------------------------------------------------------
// 
void CVcxHgVodCategoryListModelHandler::HandleAppStateChangedL()
    {
    iView.ChangeListViewL();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListModelHandler::ShowUpdatingNoteL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodCategoryListModelHandler::ShowUpdatingNoteL( 
    TBool aShow, 
    TInt aDownloadedTbns, 
    TInt aTotalTbns )
    {
    iListImpl.ShowInformationPopupL( aShow, aDownloadedTbns, aTotalTbns );
    }

// -----------------------------------------------------------------------------
// CVcxHgVodCategoryListModelHandler::HandleUpdateErrorL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodCategoryListModelHandler::HandleUpdateErrorL( TInt aError )
    {
    iListImpl.ShowUpdateErrorNoteL( aError );
    
    if ( aError == EIptvDlAuthFailed )
        {
        TRAPD( err, iListImpl.HandleAuthenticationL( ETrue ) );

        if ( err == KErrNone && iCategoryProvider )
            {
            //if no error in authentication, refresh contents.
            iCategoryProvider->RefreshCategorysL();
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
//
// -----------------------------------------------------------------------------
// 
void CVcxHgVodCategoryListModelHandler::UpdateListItemL( TInt aIndex )
    {   
    if ( iDataProviderIf.VcAppState() == EStateCategoryView &&
         aIndex >= 0 && iScroller.ItemCount() > aIndex && 
         iCategories && iCategories->Count() > aIndex )
        {    
        CVcxNsCategory& category = *(*iCategories)[ aIndex ];
        
        HBufC* name = CVcxHgVodUtils::ResolveCategoryNameLC( category );
        HBufC* info = CVcxHgVodUtils::ResolveCategoryInfoLC( category );
    
        CHgItem& item = iScroller.ItemL( aIndex );
        
        item.SetTitleL( *name );
        item.SetTextL( *info );
                
        CleanupStack::PopAndDestroy( info );
        CleanupStack::PopAndDestroy( name );
        
        iScroller.RefreshScreen( aIndex );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CVcxHgVodCategoryListModelHandler::RefreshViewL()
    {
    iScroller.Reset();
    
    if ( iCategories && iCategories->Count() > 0 )
        {
        iScroller.ResizeL( iCategories->Count() );
        }
    
    iScroller.DrawDeferred();
    iListImpl.SetMskL( R_VCXHG_VOD_MSK_BUTTON_OPEN );
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgVodCategoryListModelHandler::Request( TInt aBufferStart, 
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
void CVcxHgVodCategoryListModelHandler::Release( TInt /*aBufferStart*/, 
    TInt /*aBufferEnd*/ )
    {
    // No implementation required.
    }
