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
* Description:      My Videos UI main view container.*
*/




// INCLUDE FILES
#include <vcxhgmyvideos.rsg>
#include "vcxhgmyvideosmainview.h"
#include "vcxhgmyvideosmainviewcontainer.h"
#include "vcxhgmyvideos.hrh"
#include <csxhelp/iptv.hlp.hrh>

const TUid KUidIptvMainApp = { 0x102750E2 };

// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewContainer::CVcxHgMyVideosMainViewContainer()
// ----------------------------------------------------------------------------
// 
CVcxHgMyVideosMainViewContainer::CVcxHgMyVideosMainViewContainer(
    CVcxHgMyVideosMainView& aParentView )
    : iParentView( aParentView )
	{
	}
  
// ----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewContainer::NewL()
// ----------------------------------------------------------------------------
//
CVcxHgMyVideosMainViewContainer* CVcxHgMyVideosMainViewContainer::NewL(
    CVcxHgMyVideosMainView& aParentView )
	{
    CVcxHgMyVideosMainViewContainer* self = NewLC( aParentView );
    CleanupStack::Pop( self );
    return self;
	}

// ----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewContainer::NewLC()
// ----------------------------------------------------------------------------
// 
CVcxHgMyVideosMainViewContainer* CVcxHgMyVideosMainViewContainer::NewLC(
    CVcxHgMyVideosMainView& aParentView )
	{
    CVcxHgMyVideosMainViewContainer* self = 
        new (ELeave) CVcxHgMyVideosMainViewContainer( aParentView );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// ----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewContainer::~CVcxHgMyVideosMainViewContainer()
// ----------------------------------------------------------------------------
//
CVcxHgMyVideosMainViewContainer::~CVcxHgMyVideosMainViewContainer()
    {
    }

// ----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewContainer::ConstructL()
// ----------------------------------------------------------------------------
//
void CVcxHgMyVideosMainViewContainer::ConstructL()
	{
	CEikAppUi* appUi = static_cast<CEikAppUi*>( ControlEnv()->AppUi() );
    SetRect( appUi->ClientRect() );
    }

// ----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewContainer::OfferKeyEventL()
// ----------------------------------------------------------------------------
//
TKeyResponse CVcxHgMyVideosMainViewContainer::OfferKeyEventL( 
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

// ----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewContainer::GetHelpContext()
// ----------------------------------------------------------------------------
//
void CVcxHgMyVideosMainViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor   = KUidIptvMainApp;
    aContext.iContext = KIPTV_HLP_STORAGE;
    }
