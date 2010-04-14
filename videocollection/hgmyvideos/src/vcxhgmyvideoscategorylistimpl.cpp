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
* Description:      Implementation of UI functionality for category list.*
*/




// INCLUDE FILES
#include <ganes/HgScroller.h>
#include <vcxhgmyvideos.rsg>
#include <myvideosindicator.h>
#include <ganes/HgDoubleGraphicList.h>

#include "vcxhgmyvideoscategorylistimpl.h"
#include "vcxhgmyvideoscategorymodelhandler.h"
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideosmainview.h"
#include "vcxhgmyvideoscollectionclient.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryListImpl* CVcxHgMyVideosCategoryListImpl::NewL(
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    {
    CVcxHgMyVideosCategoryListImpl* self = 
        CVcxHgMyVideosCategoryListImpl::NewLC( aModel, aView, aScroller );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryListImpl* CVcxHgMyVideosCategoryListImpl::NewLC(
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    {
    CVcxHgMyVideosCategoryListImpl* self = 
        new (ELeave) CVcxHgMyVideosCategoryListImpl( aModel, aView, aScroller );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::CVcxHgMyVideosCategoryListImpl()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryListImpl::CVcxHgMyVideosCategoryListImpl( 
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    : CVcxHgMyVideosListBase( aModel, aView, aScroller ) 
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::ConstructL()
    {
    CVcxHgMyVideosListBase::ConstructL();
    iCategoryModel = CVcxHgMyVideosCategoryModelHandler::NewL( iModel, *iScroller );

    // Load indicator
    CGulIcon* ind = iCategoryModel->VideoIndicatorL().VideoOverlayIndicatorL();
    if ( ind )
        {
        CleanupStack::PushL( ind );
        // Ownership of the icon is transferred
        static_cast<CHgDoubleGraphicList*>( iScroller )->SetIconOverlayIndicator( ind );
        CleanupStack::Pop( ind );
        }

    // Set default icon as empty
    iScroller->SetDefaultIconL( iCategoryModel->CreateEmptyHgListIconL() );

    iScroller->SetFlags( CHgScroller::EHgScrollerKeyMarkingDisabled );
    iScroller->SetSelectionObserver( *this );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::~CVcxHgMyVideosCategoryListImpl()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryListImpl::~CVcxHgMyVideosCategoryListImpl()
    {
    delete iCategoryModel;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::DoListActivateL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::DoListActivateL( TInt aIndex )
    {  
    iCategoryModel->DoModelActivateL();
    CVcxHgMyVideosListBase::DoListActivateL( aIndex );
    iCategoryModel->GetCategoryListL();
    iScroller->SetFocus( ETrue );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::DoListDeactivate()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::DoListDeactivate()
    {    
    CVcxHgMyVideosListBase::DoListDeactivate();
    iCategoryModel->DoModelDeactivate();
    iScroller->SetFocus( EFalse );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::Highlight() 
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosCategoryListImpl::Highlight()
    {
    return iCategoryModel->Highlight();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::HandleSelectL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryListImpl::HandleSelectL( TInt /* aIndex */ )
    {    
    iView.DynInitMskL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::HandleOpenL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryListImpl::HandleOpenL( TInt /*aIndex*/ )
    {
    // Reset the visible selection of the list.
    iScroller->DrawDeferred();

    TInt highlight = Highlight();
    TInt categoryId = iCategoryModel->ResolveCategoryId( highlight );
    
    switch ( categoryId )
        {
        case KVcxMvcCategoryIdAll:
        case KVcxMvcCategoryIdDownloads:
        case KVcxMvcCategoryIdCaptured:
        case KVcxMvcCategoryIdOther:
            {
            if ( highlight >= 0 && iModel.TouchSupport() )
                {
                iView.ActivateVideoListL( iCategoryModel->ResolveCategoryId( highlight ) );
                }
            }
            break;
            
        case KCategoryIdLastWatched:
            {
            iCategoryModel->PlayLastWatchedVidedoL();
            }
            break;
            
        case KCategoryIdExtraItem1:
        case KCategoryIdExtraItem2:
        case KCategoryIdExtraItem3:
            {
            // Handle item according to type (open web link or start app).
            
            TVcxHgMyVideosCategoryItemType itemType;
            TUid appUid;
            TBuf<64> urlString;
            iCategoryModel->HandleExtraItemSelectionL( categoryId, itemType, appUid, urlString );
            
            if ( itemType == TVcxHgMyVideosCategoryItemTypeUid )
                {
                iView.LaunchAppL( appUid );
                }
            else if ( itemType == TVcxHgMyVideosCategoryItemTypeUrl )
                {
                iView.LaunchBrowserL( urlString );
                }
            }
            break;
            
        default:
            ASSERT( 0 );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::SetEmptyListTextL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::SetEmptyListTextL()
    {
    CVcxHgMyVideosListBase::SetEmptyListTextByResourceL( R_VCXHGMYVIDEOS_OPENING_CATEGORIES );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCategoryListImpl::SetTitleTextL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::SetTitleTextL( TInt /*aIndex*/ )
    {
    CVcxHgMyVideosListBase::SetTitleTextByResourceL( R_VCXHGMYVIDEOS_MY_VIDEOS_TITLE );
    }
