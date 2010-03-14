/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Branding Animation control
*
*/

// Version : %version: 9 %


// INCLUDE FILES
#include <eikenv.h>
#include <gulicon.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>
#include <mpxvideoplaybackcontrols.mbg>

#include "mpxvideoplaybackbrandinganimation.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideo_debug.h"


const TInt KMPXRealBrandingAnimationArrayCount = 17;    // Real Branding frames' count
const TInt KMPXGenericBrandingAnimationArrayCount = 10; // Generic Branding frames' count
const TInt KMPXBrandingAnimationFrameInterval = 100000; // Represents delay between frames

// ============================ MEMBER FUNCTIONS ===================================================

CMPXVideoPlaybackBrandingAnimation::CMPXVideoPlaybackBrandingAnimation(
    CMPXVideoPlaybackControlsController* aController, TBool aRealFormat )
    : iIconArray( KMPXRealBrandingAnimationArrayCount )
    , iRealFormat( aRealFormat )
    , iController( aController )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackBrandingAnimation::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackBrandingAnimation::ConstructL( TRect aRect )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackBrandingAnimation::ConstructL()"));

    //
    // set rect
    //
    iBrandingRect = TRect( 0, 0, aRect.Width(), aRect.Height() );
    SetRect( iBrandingRect );

    //
    // construct branding timer
    //
    iBrandingTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    //
    // create branding bitmaps
    //
    CreateBitmapsL();

}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackBrandingAnimation::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackBrandingAnimation*
CMPXVideoPlaybackBrandingAnimation::NewL( CMPXVideoPlaybackControlsController* aController,
                                          TRect aRect,
                                          TBool aRealFormat )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackBrandingAnimation::NewL()"));

    CMPXVideoPlaybackBrandingAnimation* self =
        new ( ELeave ) CMPXVideoPlaybackBrandingAnimation( aController, aRealFormat );

    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackBrandingAnimation::~CMPXVideoPlaybackBrandingAnimation()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackBrandingAnimation::~CMPXVideoPlaybackBrandingAnimation()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackBrandingAnimation::~CMPXVideoPlaybackBrandingAnimation()"));

    iIconArray.ResetAndDestroy();

    if ( iBrandingTimer )
    {
         iBrandingTimer->Cancel();
         delete iBrandingTimer;
         iBrandingTimer = NULL;
    }

}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackBrandingAnimation::CreateBitmapsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackBrandingAnimation::CreateBitmapsL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackBrandingAnimation::CreateBitmapsL()"));

    //
    // Create icons
    //
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TFileName iconsPath;
    iController->LocateBitmapFileL( iconsPath );

    TInt bitmapIndex;

    if ( iRealFormat )
    {
        //
        // first frame of Real Player Branding Animation
        //
        bitmapIndex = EMbmMpxvideoplaybackcontrolsQgn_graf_realplayer_splash_01;

        //
        // number of Real Player animation frames
        //
        iCount = KMPXRealBrandingAnimationArrayCount;
    }
    else
    {
        //
        // first frame of Generic Branding Animation
        //
        bitmapIndex = EMbmMpxvideoplaybackcontrolsQgn_graf_ring_wait_01;

        //
        // number of Generic animation frames
        //
        iCount = KMPXGenericBrandingAnimationArrayCount;
    }

    for ( TInt i = 0; i < iCount * 2; i = i + 2 )
    {
        //
        // Create icons
        //
        CGulIcon* icon = AknsUtils::CreateGulIconL( skin,
                                                    KAknsIIDQgnVideoPluginAnimBuffering,
                                                    iconsPath,
                                                    bitmapIndex + i ,
                                                    bitmapIndex + i + 1 );

        if ( icon )
        {
            //
            // push icon to the cleanup stack
            //
            CleanupStack::PushL( icon );

            //
            // Set icon size
            //
            AknIconUtils::SetSize( icon->Bitmap(), iBrandingRect.Size(), EAspectRatioPreserved );

            //
            // Append icon to array
            //
            iIconArray.AppendL( icon );

            //
            // pop icon to the cleanup stack
            //
            CleanupStack::Pop( icon );
        }
    }

    //
    // start branding timer
    //
    StartBrandingTimer();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackBrandingAnimation::CountComponentControls()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackBrandingAnimation::CountComponentControls() const
{
    return 0;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackBrandingAnimation::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackBrandingAnimation::Draw( const TRect& aRect ) const
{
    MPX_DEBUG(_L("CMPXVideoPlaybackBrandingAnimation::Draw()"));

    CWindowGc& gc = SystemGc();
    gc.SetClippingRect( aRect );

    if ( iController->SetBackgroundBlack() )
    {
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

    //
    // draw branding animation
    //
    gc.BitBltMasked( iBrandingRect.iTl,
                     iIconArray[ iCurrentIndex ]->Bitmap(),
                     TRect( iBrandingRect.Size() ),
                     iIconArray[ iCurrentIndex ]->Mask(),
                     ETrue );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackBrandingAnimation::StartBrandingTimer
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackBrandingAnimation::StartBrandingTimer()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackBrandingAnimation::StartBrandingTimer()"));

    //
    // set branding timer
    //
    if ( ! iBrandingTimer->IsActive() )
    {
        iBrandingTimer->Start(
            0,
            KMPXBrandingAnimationFrameInterval,
            TCallBack( CMPXVideoPlaybackBrandingAnimation::BrandingTimer, this ) );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackBrandingAnimation::BrandingTimer
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackBrandingAnimation::BrandingTimer( TAny* aPtr )
{
    static_cast<CMPXVideoPlaybackBrandingAnimation*>(aPtr)->HandleBrandingTimer();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackBrandingAnimation::HandleBrandingTimer
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackBrandingAnimation::HandleBrandingTimer()
{
    //
    // keep track of the current animation frame
    //
    iCurrentIndex = (++iCurrentIndex) % iCount;

    //
    // draw the current animation frame
    //
    if ( IsVisible() )
    {
        DrawNow();
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackBrandingAnimation::CancelBrandingTimer
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackBrandingAnimation::CancelBrandingTimer()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackBrandingAnimation::CancelBrandingTimer()"));

    if ( iBrandingTimer->IsActive() )
    {
        iBrandingTimer->Cancel();
    }
}

//  End of File
