/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Button control
*
*/

// Version : %version: 8 %



// INCLUDE FILES
#include <eikenv.h>
#include <gulicon.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>

#include "mpxvideoplaybackbutton.h"

// ============================ MEMBER FUNCTIONS ===================================================

CMPXVideoPlaybackButton::CMPXVideoPlaybackButton()
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButton::ConstructL( TRect aRect, const TDesC &aIconPath )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::ConstructL()"));

    iIConPath = aIconPath.AllocL();

    SetRect( aRect );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackButton* CMPXVideoPlaybackButton::NewL( TRect aRect, const TDesC &aIconPath)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::NewL()"));

    CMPXVideoPlaybackButton* self = new ( ELeave ) CMPXVideoPlaybackButton();

    CleanupStack::PushL( self );
    self->ConstructL( aRect, aIconPath );
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::~CMPXVideoPlaybackButton()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackButton::~CMPXVideoPlaybackButton()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::~CMPXVideoPlaybackButton()"));

    if ( iIConPath )
    {
        delete iIConPath;
        iIConPath = NULL;
    }

    if ( iDefaultButton )
    {
        delete iDefaultButton;
        iDefaultButton = NULL;
    }

    if ( iDimmedButton )
    {
        delete iDimmedButton;
        iDimmedButton = NULL;
    }

    if ( iPressedButton )
    {
        delete iPressedButton;
        iPressedButton = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::CreateDefaultButtonL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButton::CreateDefaultButtonL( const TAknsItemID &aID,
                                                    const TInt aFileIndex,
                                                    const TInt aFileMaskIndex )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::CreateDefaultButtonL()"));

    if ( iDefaultButton )
    {
        delete iDefaultButton;
        iDefaultButton = NULL;
    }

    iDefaultButton = CreateButtonL( aID, aFileIndex, aFileMaskIndex );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::CreateButtonL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButton::CreateDimmedButtonL( const TAknsItemID &aID,
                                                   const TInt aFileIndex,
                                                   const TInt aFileMaskIndex )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::CreateDimmedButtonL()"));

    if ( iDimmedButton )
    {
        delete iDimmedButton;
        iDimmedButton = NULL;
    }

    iDimmedButton = CreateButtonL( aID, aFileIndex, aFileMaskIndex );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::CreatePressedButtonL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButton::CreatePressedButtonL( const TAknsItemID &aID,
                                                    const TInt aFileIndex,
                                                    const TInt aFileMaskIndex )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::CreatePressedButtonL()"));

    if ( iPressedButton )
    {
        delete iPressedButton;
        iPressedButton = NULL;
    }

    iPressedButton = CreateButtonL( aID, aFileIndex, aFileMaskIndex );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::CreateButtonL()
// -------------------------------------------------------------------------------------------------
//
CGulIcon* CMPXVideoPlaybackButton::CreateButtonL( const TAknsItemID &aID,
                                                  const TInt aFileIndex,
                                                  const TInt aFileMaskIndex )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::CreateButtonL()"));

    CGulIcon* icon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                                                aID,
                                                iIConPath->Des(),
                                                aFileIndex,
                                                aFileMaskIndex );

    if ( icon )
    {
        AknIconUtils::SetSize( icon->Bitmap(),
                               Rect().Size(),
                               EAspectRatioNotPreserved );
    }

    return icon;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackButton::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButton::Draw( const TRect& aRect ) const
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::Draw()"));

    CWindowGc& gc = SystemGc();
    gc.SetClippingRect( aRect );

    if ( Window().DisplayMode() == EColor16MAP )
    {
        gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        gc.SetBrushColor( TRgb::Color16MAP( 255 ) );
        gc.Clear( aRect );
    }
    else if ( Window().DisplayMode() == EColor16MA )
    {
        gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        gc.SetBrushColor( TRgb::Color16MA( 0 ) );
        gc.Clear( aRect );
    }
    else
    {
        // draw a solid background so that the entire progress
        // bar is shown not just the area representing the
        // portion that has been played.
        gc.SetBrushColor( KRgbBlack );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( aRect );
        gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    }

    switch( iState )
    {
        case EMPXButtonNormal:
        {
            gc.BitBltMasked( Rect().iTl,
                             iDefaultButton->Bitmap(),
                             TRect( Rect().Size() ),
                             iDefaultButton->Mask(),
                             ETrue );
            break;
        }
        case EMPXButtonDimmed:
        {
            gc.BitBltMasked( Rect().iTl,
                             iDimmedButton->Bitmap(),
                             TRect( Rect().Size() ),
                             iDimmedButton->Mask(),
                             ETrue );
            break;
        }
        case EMPXButtonPressed:
        {
            gc.BitBltMasked( Rect().iTl,
                             iPressedButton->Bitmap(),
                             TRect( Rect().Size() ),
                             iPressedButton->Mask(),
                             ETrue );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::CountComponentControls()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackButton::CountComponentControls() const
{
    return 0;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::SetDimmed
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButton::SetDimmed( TBool aDimmed )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::SetDimmed() (%d)"), aDimmed);

    if ( aDimmed && iDimmedButton )
    {
        iState = EMPXButtonDimmed;
    }
    else if ( iState == EMPXButtonDimmed )
    {
        iState = EMPXButtonNormal;
    }

    if ( IsVisible() )
    {
        DrawNow();
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButton::SetPressed
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButton::SetPressed( TBool aPressed )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButton::SetPressed() (%d)"), aPressed);

    if ( aPressed && iPressedButton )
    {
        iState = EMPXButtonPressed;
    }
    else if ( iState == EMPXButtonPressed )
    {
        iState = EMPXButtonNormal;
    }

    if ( IsVisible() )
    {
        DrawDeferred();
    }
}

//  End of File
