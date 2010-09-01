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
* Description:    HG VOD Main view container implementation*
*/




#include <aknViewAppUi.h>
#include <AknsDrawUtils.h>
#include "vcxnsuiengine.h"
#include "vcxhgvodmainview.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodui.hrh"
#include "vcxhgvodlistimplbase.h"

// -----------------------------------------------------------------------------
// CVcxHgVodMainViewContainer::CVcxHgVodMainViewContainer()
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgVodMainViewContainer::CVcxHgVodMainViewContainer( 
	CAknViewAppUi* aAppUi,
	CVcxHgVodMainView& aParent )
  : iAppUi( aAppUi ),
    iParentView( aParent )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgVodMainViewContainer::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgVodMainViewContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    
    // sends window to back of the z-order
    Window().SetOrdinalPosition( KErrNotFound );
    
    SetRect( aRect );
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CVcxHgVodMainViewContainer::~CVcxHgVodMainViewContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgVodMainViewContainer::~CVcxHgVodMainViewContainer()
    {
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TInt CVcxHgVodMainViewContainer::CountComponentControls() const
    {
    return 0;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
CCoeControl* CVcxHgVodMainViewContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        default:
            {
            return NULL;    
            }        
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TKeyResponse CVcxHgVodMainViewContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    TKeyResponse response( EKeyWasNotConsumed );
    response = iParentView.HandleKeyEventL( aKeyEvent, aType );
    
    if ( response == EKeyWasNotConsumed )
        {
        return CCoeControl::OfferKeyEventL( aKeyEvent, aType );
        }
    return response;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CVcxHgVodMainViewContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    
    // Drawing skin
    MAknsControlContext* background = AknsDrawUtils::ControlContextOfParent( this );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();        

    if ( !background || !AknsDrawUtils::Background( skin, background, this, gc, aRect ) )
        {
        // The background was not drawn, drawing black background.
        gc.SetBrushColor( KRgbBlack );
        gc.Clear( aRect );
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CVcxHgVodMainViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    iParentView.ActiveList()->GetHelpContext( aContext );
    }

