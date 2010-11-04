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
* Description: Media Details Viewer
*
*/


// Version : %version:  ou1cpsw#19 %


// INCLUDE FILES
#include <eikenv.h>
#include <eiklabel.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <StringLoader.h>
#include <AknsDrawUtils.h>
#include <gulicon.h>
#include <gdi.h>
#include <e32base.h>

#include <mpxfiledetailscntlpluginif.h>
#include <mpxvideoplaybackcontrols.rsg>
#include <mpxvideoplaybackcontrols.mbg>
#include <avkon.rsg>

#include "mpxvideoplaybackcontrol.hrh"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackmediadetailsviewer.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackcontainer.h"

#include <AknTasHook.h> // For testability hooks

// Constants
_LIT(KHeadingValueSeperator, ": ");
_LIT(KWidthHeightSeparator, " \xd7 ");
_LIT(KLeftMargin, " " );

const TInt KMediaDetailsViewerMaxItems = 11;  // used to determine the height of each item
const TInt KMediaDetailsViewerItemMargin = 3;
const TInt KMediaDetailsViewerVisibleCharacters = 30;
const TInt KMediaDetailsViewerFilenameMedium = 45;
const TInt KMediaDetailsViewerOffsetDivisorMedium = 12;
const TInt KMediaDetailsViewerOffsetDivisorMax = 20;
const TInt KMediaDetailsViewerMaxBufLen = 255;

// UID of this interface
const TUid KFileDetailsCntlPluginUid = {0x2002A59D};

const TUint32 KScrollTimerDelayTickCounts = 2000;   // 2000 milliseconds
const TUint32 KInvalidTick = 0;

// ============================ MEMBER FUNCTIONS ===================================================

CMPXVideoPlaybackMediaDetailsViewer::CMPXVideoPlaybackMediaDetailsViewer(
    CMPXVideoPlaybackControlsController* aController )
    : iController( aController )
    , iDrmDetailsLaunched( EFalse )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::ConstructL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMediaDetailsViewer::ConstructL()"));

    iViewerRect = CalculateViewerRectL();
    SetRect( TRect(0, 0, iViewerRect.Width(), iViewerRect.Height()) );

    CreateLabelsL();
    SkinChangeL();

    iBackgroundBitmap = new ( ELeave ) CFbsBitmap();
    TSize bitmapSize = Rect().Size();
    User::LeaveIfError( iBackgroundBitmap->Create( bitmapSize, EColor16MA ) );

    iScrollingTextTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iScrollingTextTimer->Start(
                0,
                175000,
                TCallBack( CMPXVideoPlaybackMediaDetailsViewer::ScrollTimer, this ) );

    AKNTASHOOK_ADDL( this, "CMPXVideoPlaybackMediaDetailsViewer" );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackMediaDetailsViewer* CMPXVideoPlaybackMediaDetailsViewer::NewL(
                                                CMPXVideoPlaybackControlsController* aController )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMediaDetailsViewer::NewL()"));

    CMPXVideoPlaybackMediaDetailsViewer* self = new ( ELeave ) CMPXVideoPlaybackMediaDetailsViewer( aController );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::~CMPXVideoPlaybackMediaDetailsViewer()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackMediaDetailsViewer::~CMPXVideoPlaybackMediaDetailsViewer()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMediaDetailsViewer::~CMPXVideoPlaybackMediaDetailsViewer()"));

    AKNTASHOOK_REMOVE();
    
    if ( iClipnameLabel )
    {
        delete iClipnameLabel;
        iClipnameLabel = NULL;
    }

    if ( iTitleLabel )
    {
        delete iTitleLabel;
        iTitleLabel = NULL;
    }

    if ( iArtistLabel )
    {
        delete iArtistLabel;
        iArtistLabel = NULL;
    }

    if ( iFormatLabel )
    {
        delete iFormatLabel;
        iFormatLabel = NULL;
    }

    if ( iResolutionLabel )
    {
        delete iResolutionLabel;
        iResolutionLabel = NULL;
    }

    if ( iDurationLabel )
    {
        delete iDurationLabel;
        iDurationLabel = NULL;
    }

    if ( iBitrateLabel )
    {
        delete iBitrateLabel;
        iBitrateLabel = NULL;
    }

    if ( iLicenseLabel )
    {
        delete iLicenseLabel;
        iLicenseLabel = NULL;
    }

    if ( iBackgroundBitmap )
    {
        delete iBackgroundBitmap;
        iBackgroundBitmap = NULL;
    }

    if ( iScrollingTextTimer )
    {
        iScrollingTextTimer->Cancel();
        delete iScrollingTextTimer;
        iScrollingTextTimer = NULL;
    }

    if ( iAdditionalLabel )
    {
        delete iAdditionalLabel;
        iAdditionalLabel = NULL;
    }

    if ( iAdditionalString )
    {
        delete iAdditionalString;
        iAdditionalString = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::HandlePointerEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMediaDetailsViewer::HandlePointerEventL()"));

    switch ( aPointerEvent.iType )
    {
        case TPointerEvent::EButton1Down:
        {
            break;
        }
        case TPointerEvent::EButton1Up:
        {
            TRect expandedRect;  //make it a bit easier to click the License label

            if ( iLicenseLabel )
            {
                expandedRect = TRect(iLicenseLabel->Rect().iTl,
                                     TPoint(iLicenseLabel->Rect().iBr.iX,
                                            iLicenseLabel->Rect().iBr.iY * 2 ) );
            }

            if ( iLicenseLabel && expandedRect.Contains(aPointerEvent.iPosition) )
            {
                LaunchDRMDetailsL();
            }
            else
            {
                iController->HandleEventL( EMPXControlCmdCloseFileDetailsViewer );
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::LaunchDRMDetailsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::LaunchDRMDetailsL()
{
    if ( ! iDrmDetailsLaunched )
    {
        iDrmDetailsLaunched = ETrue;
        iController->iContainer->HandleCommandL( EMPXPbvLaunchDRMDetails );
        iDrmDetailsLaunched = EFalse;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::SkinChangeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::SkinChangeL()
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::Draw( const TRect& aRect ) const
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMediaDetailsViewer::Draw()"));

    CWindowGc& gc = SystemGc();
    gc.SetClippingRect( aRect );

    this->DrawableWindow()->SetCornerType(EWindowCorner5);

    TRAP_IGNORE( UpdateBackgroundBitmapL() );
    gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
    gc.BitBlt( Rect().iTl, iBackgroundBitmap );

    TRAP_IGNORE( FormatLabelsL() );

    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::FormatLabelsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::FormatLabelsL() const
{
    // FORMAT LABELS


    TInt labelWidth = Rect().iBr.iX;
    TInt labelHeight = iController->iContainer->Rect().iBr.iY / KMediaDetailsViewerMaxItems;
    TInt rowsAdded = 0;

    // License
    if ( iController->FileDetails()->iDrmProtected )
    {
        iLicenseLabel->SetExtent(
                             TPoint(Rect().iTl.iX, Rect().iTl.iY + ( labelHeight * rowsAdded) ) ,
                             TSize( labelWidth, labelHeight )
                             );
        HBufC* heading  = iEikonEnv->AllocReadResourceL( R_MPX_LICENSE_HEADING );
        CleanupStack::PushL( heading );

        TBuf<KMediaDetailsViewerMaxBufLen> licenseField;
        licenseField.Append( KLeftMargin );
        licenseField.Append( *heading );
        iLicenseLabel->OverrideColorL( EColorLabelText, KRgbDarkBlue );
        iLicenseLabel->SetTextL( licenseField );
        iLicenseLabel->SetAllMarginsTo( KMediaDetailsViewerItemMargin );
        iLicenseLabel->SetLabelAlignment( ELayoutAlignCenter );
        iLicenseLabel->SetUnderlining( ETrue );
        iLicenseLabel->MakeVisible( ETrue );
        rowsAdded++;

        CleanupStack::PopAndDestroy( heading );
    }

    // Title
    HBufC* fileTitle = iController->FileDetails()->GenerateFileTitleL();

    if ( fileTitle && fileTitle->Length() )
    {
        // Title gets populated by UpdateTitle method
        iTitleLabel->SetExtent(
                          TPoint( Rect().iTl.iX, Rect().iTl.iY + ( labelHeight * rowsAdded ) ) ,
                          TSize( labelWidth, labelHeight )
                          );
        iTitleLabel->SetAllMarginsTo( KMediaDetailsViewerItemMargin );
        iTitleLabel->MakeVisible( ETrue );
        rowsAdded++;
    }

    delete fileTitle;

    // Filename
    HBufC* fileName = iController->FileDetails()->GenerateFileNameL();

    if ( fileName && fileName->Length() )
    {
        // file name gets populated by UpdateFilename method
        iClipnameLabel->SetExtent(
                             TPoint( Rect().iTl.iX, Rect().iTl.iY + ( labelHeight * rowsAdded ) ) ,
                             TSize( labelWidth, labelHeight )
                             );
        iClipnameLabel->SetAllMarginsTo( KMediaDetailsViewerItemMargin );
        iClipnameLabel->MakeVisible( ETrue );
        rowsAdded++;
    }

    delete fileName;

    // Mime Type (Format)
    if ( iController->FileDetails()->iMimeType && iController->FileDetails()->iMimeType->Length() )
    {
        iFormatLabel->SetExtent(
                          TPoint( Rect().iTl.iX, Rect().iTl.iY + ( labelHeight * rowsAdded ) ) ,
                          TSize( labelWidth, labelHeight )
                          );
        HBufC* heading  = iEikonEnv->AllocReadResourceL( R_MPX_FORMAT_HEADING );
        CleanupStack::PushL( heading );

        TBuf<KMediaDetailsViewerMaxBufLen> formatField;
        formatField.Append( KLeftMargin );
        formatField.Append( *heading );
        formatField.Append( KHeadingValueSeperator );
        TInt allowLen = KMediaDetailsViewerMaxBufLen-formatField.Length();
        formatField.Append( ( iController->FileDetails()->iMimeType )->Left( allowLen ) );
        iFormatLabel->SetTextL( formatField );
        iFormatLabel->SetAllMarginsTo( KMediaDetailsViewerItemMargin );
        iFormatLabel->MakeVisible( ETrue );
        rowsAdded++;

        CleanupStack::PopAndDestroy( heading );
    }

    // Resolution
    if ( iController->FileDetails()->iVideoHeight &&  iController->FileDetails()->iVideoWidth )
    {
        iResolutionLabel->SetExtent(
                               TPoint( Rect().iTl.iX, Rect().iTl.iY + ( labelHeight * rowsAdded ) ) ,
                               TSize( labelWidth, labelHeight )
                               );
        HBufC* heading  = iEikonEnv->AllocReadResourceL( R_MPX_RESOLUTION_HEADING );
        CleanupStack::PushL( heading );

        TBuf<KMediaDetailsViewerMaxBufLen> resolutionField;
        resolutionField.Append( KLeftMargin );
        resolutionField.Append( *heading );
        resolutionField.Append( KHeadingValueSeperator );
        resolutionField.AppendNum( iController->FileDetails()->iVideoWidth );
        resolutionField.Append( KWidthHeightSeparator );
        resolutionField.AppendNum( iController->FileDetails()->iVideoHeight);
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( resolutionField );

        iResolutionLabel->SetTextL( resolutionField );
        iResolutionLabel->SetAllMarginsTo(KMediaDetailsViewerItemMargin);
        iResolutionLabel->MakeVisible( ETrue );
        rowsAdded++;

        CleanupStack::PopAndDestroy( heading );
    }

    // Duration
    if ( iController->FileDetails()->iDuration > 0 )
    {
        iDurationLabel->SetExtent(
                             TPoint( Rect().iTl.iX, Rect().iTl.iY + ( labelHeight * rowsAdded) ) ,
                             TSize( labelWidth, labelHeight )
                             );
        HBufC* heading  = iEikonEnv->AllocReadResourceL( R_MPX_DURATION_HEADING );
        CleanupStack::PushL( heading );

        TBuf<KMediaDetailsViewerMaxBufLen> durationField;
        durationField.Append( KLeftMargin );
        durationField.Append( *heading );
        durationField.Append( KHeadingValueSeperator );

        TInt64 durationInSeconds =  iController->FileDetails()->iDuration / 1000;

        HBufC* unitFormatString;

        if ( durationInSeconds < 3600 )  // one hour
        {
            unitFormatString = StringLoader::LoadLC(R_QTN_TIME_DURAT_MIN_SEC);
        }
        else
        {
            unitFormatString = StringLoader::LoadLC(R_QTN_TIME_DURAT_LONG);
        }

        TBuf<64> dur;
        TTime durTime = TTime( durationInSeconds * 1000000 );
        durTime.FormatL( dur, *unitFormatString );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( dur );

        durationField.Append( dur );
        iDurationLabel->SetTextL( durationField );
        iDurationLabel->SetAllMarginsTo( KMediaDetailsViewerItemMargin );
        iDurationLabel->MakeVisible( ETrue );
        rowsAdded++;

        CleanupStack::PopAndDestroy( unitFormatString ); // unitFormatString
        CleanupStack::PopAndDestroy( heading );
    }

    // Bitrate
    if ( iController->FileDetails()->iBitRate > 0 )
    {
        iBitrateLabel->SetExtent(
                          TPoint( Rect().iTl.iX, Rect().iTl.iY + ( labelHeight * rowsAdded ) ) ,
                          TSize( labelWidth, labelHeight )
                          );
        HBufC* heading  = iEikonEnv->AllocReadResourceL( R_MPX_BITRATE_HEADING );
        CleanupStack::PushL( heading );

        TBuf<KMediaDetailsViewerMaxBufLen> bitrateField;
        bitrateField.Append( KLeftMargin );
        bitrateField.Append( *heading );
        bitrateField.Append( KHeadingValueSeperator );

        HBufC* formattedBitrate =
             StringLoader::LoadLC(R_MPX_BITRATE_UNITS,iController->FileDetails()->iBitRate / 1000 );

        bitrateField.Append( *formattedBitrate );
        iBitrateLabel->SetTextL( bitrateField );
        iBitrateLabel->SetAllMarginsTo( KMediaDetailsViewerItemMargin );
        iBitrateLabel->MakeVisible( ETrue );
        rowsAdded++;

        CleanupStack::PopAndDestroy( formattedBitrate ); // formattedBitrate
        CleanupStack::PopAndDestroy( heading );
    }

    // Artist
    if ( iController->FileDetails()->iArtist && iController->FileDetails()->iArtist->Length() )
    {
        iArtistLabel->SetExtent(
                          TPoint( Rect().iTl.iX, Rect().iTl.iY + ( labelHeight * rowsAdded ) ) ,
                          TSize( labelWidth, labelHeight )
                          );
        HBufC* heading  = iEikonEnv->AllocReadResourceL( R_MPX_ARTIST_HEADING );
        CleanupStack::PushL( heading );

        TBuf<KMediaDetailsViewerMaxBufLen> artistField;
        artistField.Append( KLeftMargin );
        artistField.Append( *heading );
        artistField.Append( KHeadingValueSeperator );
        TInt allowLen = KMediaDetailsViewerMaxBufLen-artistField.Length();
        artistField.Append( ( iController->FileDetails()->iArtist )->Left( allowLen ) );
        iArtistLabel->SetTextL( artistField );
        iArtistLabel->SetAllMarginsTo( KMediaDetailsViewerItemMargin );
        iArtistLabel->MakeVisible( ETrue );
        rowsAdded++;

        CleanupStack::PopAndDestroy( heading );
    }

    // Additional Label
    if ( iAdditionalString && iAdditionalString->Length() )
    {
        iAdditionalLabel->SetExtent(
                               TPoint( Rect().iTl.iX, Rect().iTl.iY + ( labelHeight * rowsAdded ) ) ,
                               TSize( labelWidth, labelHeight )
                               );

        TBuf<KMediaDetailsViewerMaxBufLen> titleField;
        titleField.Append( KLeftMargin );
        TInt allowLen = KMediaDetailsViewerMaxBufLen-titleField.Length();
        titleField.Append( iAdditionalString->Left( allowLen ) );
        iAdditionalLabel->SetTextL( titleField );
        iAdditionalLabel->SetAllMarginsTo( KMediaDetailsViewerItemMargin );
        iAdditionalLabel->MakeVisible( ETrue );
        rowsAdded++;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::CountComponentControls()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackMediaDetailsViewer::CountComponentControls() const
{
    return 9;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::ComponentControl
// -------------------------------------------------------------------------------------------------
//
CCoeControl* CMPXVideoPlaybackMediaDetailsViewer::ComponentControl(TInt aIndex) const
{
    CCoeControl* control = NULL;

    switch ( aIndex )
    {
        case 0:
            control = iClipnameLabel;
            break;
        case 1:
            control = iTitleLabel;
            break;
        case 2:
            control = iArtistLabel;
            break;
        case 3:
            control = iFormatLabel;
            break;
        case 4:
            control = iResolutionLabel;
            break;
        case 5:
            control = iDurationLabel;
            break;
        case 6:
            control = iBitrateLabel;
            break;
        case 7:
            control = iLicenseLabel;
            break;
        case 8:
            control = iAdditionalLabel;
            break;
        default:
            break;
    }

    return control;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::Reset
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::Reset()
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::CreateLabelsL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::CreateLabelsL()
{
    // Licence / DRM
    iLicenseLabel = new (ELeave) CEikLabel;
    iLicenseLabel->SetContainerWindowL( *this );
    iLicenseLabel->SetTextL( KNullDesC );

    // Filename
    iClipnameLabel = new (ELeave) CEikLabel;
    iClipnameLabel->SetContainerWindowL( *this );
    iClipnameLabel->SetTextL( KNullDesC );
    iClipnameLabel->SetLabelAlignment( ELayoutAlignBidi );

    // Title
    iTitleLabel = new (ELeave) CEikLabel;
    iTitleLabel->SetContainerWindowL( *this );
    iTitleLabel->SetTextL( KNullDesC );
    iTitleLabel->SetLabelAlignment( ELayoutAlignBidi );

    // Artist
    iArtistLabel = new (ELeave) CEikLabel;
    iArtistLabel->SetContainerWindowL( *this );
    iArtistLabel->SetTextL( KNullDesC );
    iArtistLabel->SetLabelAlignment( ELayoutAlignBidi );

    // Format
    iFormatLabel = new (ELeave) CEikLabel;
    iFormatLabel->SetContainerWindowL( *this );
    iFormatLabel->SetTextL( KNullDesC );
    iFormatLabel->SetLabelAlignment( ELayoutAlignBidi );

    // Resolution
    iResolutionLabel = new (ELeave) CEikLabel;
    iResolutionLabel->SetContainerWindowL( *this );
    iResolutionLabel->SetTextL( KNullDesC );
    iResolutionLabel->SetLabelAlignment( ELayoutAlignBidi );

    // Duration
    iDurationLabel = new (ELeave) CEikLabel;
    iDurationLabel->SetContainerWindowL( *this );
    iDurationLabel->SetTextL( KNullDesC );
    iDurationLabel->SetLabelAlignment( ELayoutAlignBidi );

    // Bitrate
    iBitrateLabel = new (ELeave) CEikLabel;
    iBitrateLabel->SetContainerWindowL( *this );
    iBitrateLabel->SetTextL( KNullDesC );
    iBitrateLabel->SetLabelAlignment( ELayoutAlignBidi );

    // Additional Labels
    iAdditionalLabel = new (ELeave) CEikLabel;
    iAdditionalLabel->SetContainerWindowL( *this );
    iAdditionalLabel->SetTextL( KNullDesC );
    iAdditionalLabel->SetLabelAlignment( ELayoutAlignBidi );
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::UpdateBackgroundBitmapL
// Update background bitmap
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::UpdateBackgroundBitmapL() const
{
    TSize bitmapSize = Rect().Size();

    CFbsBitmapDevice* bgBitmapDevice = CFbsBitmapDevice::NewL( iBackgroundBitmap );
    CleanupStack::PushL( bgBitmapDevice );

    CFbsBitGc* bgBitGc( NULL );
    User::LeaveIfError( bgBitmapDevice->CreateContext( bgBitGc ) );
    CleanupStack::PushL( bgBitGc );

    bgBitGc->SetBrushColor( TRgb(0x708090) );  // slate gray
    bgBitGc->Clear();

    CleanupStack::PopAndDestroy( bgBitGc );
    CleanupStack::PopAndDestroy( bgBitmapDevice );

    // Dither alpha channel for semitransparency
    //
    // Scan line length in 32-bit words
    TInt scanLineLengthWord = CFbsBitmap::ScanLineLength( bitmapSize.iWidth, EColor16MA) / 4;

    iBackgroundBitmap->LockHeap();
    TUint32* dataAddr = reinterpret_cast<TUint32*>( iBackgroundBitmap->DataAddress() );

    for ( TInt y = 0; y < bitmapSize.iHeight; y++ )
    {
        for ( TInt x = 0; x < bitmapSize.iWidth; x++ )
        {
            if ( ( y + x ) & 1 )
            {
                // Make this pixel transparent
                dataAddr[y * scanLineLengthWord + x] = 0x00000000;
            }
        }
    }

    iBackgroundBitmap->UnlockHeap();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::ScrollTimer
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackMediaDetailsViewer::ScrollTimer( TAny* aPtr )
{
    TRAP_IGNORE(
            static_cast<CMPXVideoPlaybackMediaDetailsViewer*>(aPtr)->HandleScrollTimerL()
            );
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackMediaDetailsViewer::HandleScrollTimerL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::HandleScrollTimerL()
{
    MPX_ENTER_EXIT( _L( "CMPXVideoPlaybackMediaDetailsViewer::HandleScrollTimerL" ) );

    if ( iFilenameScroller.IsUpdateNeeded() )
    {
        UpdateFilenameL();
    }

    if ( iTitleScroller.IsUpdateNeeded() )
    {
        UpdateTitleL();
    }

    if ( !iFilenameScroller.IsScrollNeeded() && !iTitleScroller.IsScrollNeeded() )
    {
        iScrollingTextTimer->Cancel();   // no need to keep the timer active
    }

    if ( iScrolledTextUpdated )
    {
        iScrolledTextUpdated = EFalse;
        DrawNow();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::NumOfItemsShownInViewer
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackMediaDetailsViewer::NumOfItemsShownInViewerL()
{
    TInt numOfItems = 0;

    if ( iController->FileDetails()->iDrmProtected )
    {
        numOfItems++;
    }

    if ( iController->FileDetails()->iClipName )
    {
        numOfItems++;
    }

    if ( iController->FileDetails()->iMimeType )
    {
        numOfItems++;
    }

    if ( iController->FileDetails()->iVideoHeight &&
         iController->FileDetails()->iVideoWidth )
    {
        numOfItems++;
    }

    if ( iController->FileDetails()->iDuration > 0 )
    {
        numOfItems++;
    }

    if ( iController->FileDetails()->iBitRate > 0 )
    {
        numOfItems++;
    }

    HBufC* title = NULL;
    TRAP_IGNORE ( title = iController->FileDetails()->GenerateFileTitleL() );

    if ( title )
    {
        numOfItems++;
        delete title;
    }

    if ( iController->FileDetails()->iArtist )
    {
        numOfItems++;
    }

    // Additional File Details
    CMPXFileDetailsPlugin* addPlugin( NULL );

    // Use the interface to load the ecom plugin
    TRAPD ( err, addPlugin = CMPXFileDetailsPlugin::NewL( KFileDetailsCntlPluginUid ) );

    // If plugin loaded successfully
    if ( ! err )
    {
        TPtrC addLabel;
        addLabel.Set( addPlugin->GetAdditionalLabelLC( iController->FileDetails()->iFourCCCode ) );
        iAdditionalString = addLabel.AllocL();

        if ( iAdditionalString->Length() )
        {
            numOfItems++;
        }

        CleanupStack::PopAndDestroy();  // addLabel
    }

    delete addPlugin;

    return numOfItems;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::CalculateViewerRect
// -------------------------------------------------------------------------------------------------
//
TRect CMPXVideoPlaybackMediaDetailsViewer::CalculateViewerRectL()
{
    TInt numOfItems = NumOfItemsShownInViewerL();
    TRect viewerRect;

    // optimize viewer's width for the clipname
    TInt horizontalOffset = 0;
    TInt filenameLength = iController->FileDetails()->iClipName->Des().Length();

    if ( filenameLength <= KMediaDetailsViewerFilenameMedium )
    {
        horizontalOffset =
            iController->iContainer->Rect().iBr.iX / KMediaDetailsViewerOffsetDivisorMedium;
    }
    else
    {
        horizontalOffset =
            iController->iContainer->Rect().iBr.iX / KMediaDetailsViewerOffsetDivisorMax;
    }

    TInt eachItemHeight = iController->iContainer->Rect().iBr.iY / KMediaDetailsViewerMaxItems;

    TInt verticalHalf = iController->iContainer->Rect().iBr.iY / 2;

    TInt tLX = (iController->iContainer->Rect().iTl.iX) + horizontalOffset;
    TInt bRX = (iController->iContainer->Rect().iBr.iX) - horizontalOffset;

    TInt tLY = verticalHalf - ( eachItemHeight * numOfItems/2 );

    // pad for the margins on top and bottom of viewer
    TInt bRY =
        verticalHalf + ( eachItemHeight * numOfItems/2 ) + ( KMediaDetailsViewerItemMargin * 2 );

    viewerRect = TRect( tLX, tLY, bRX, bRY );

    return viewerRect;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::UpdateFilenameL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::UpdateFilenameL()
{
    MPX_ENTER_EXIT( _L( "CMPXVideoPlaybackMediaDetailsViewer::UpdateFilenameL" ) );

    HBufC* fileName = iController->FileDetails()->GenerateFileNameL();
    CleanupStack::PushL( fileName );

    if ( fileName && fileName->Length() )
    {
        HBufC* heading  = iEikonEnv->AllocReadResourceL( R_MPX_FILENAME_HEADING );
        CleanupStack::PushL( heading );

        TBuf<KMediaDetailsViewerMaxBufLen> filenameField;
        filenameField.Append( KLeftMargin );
        filenameField.Append( *heading );
        filenameField.Append( KHeadingValueSeperator );

        iFilenameScroller.ScrollText( *fileName, filenameField );

        iClipnameLabel->SetTextL( filenameField );

        iScrolledTextUpdated = ETrue;

        CleanupStack::PopAndDestroy( heading );
    }

    CleanupStack::PopAndDestroy( fileName );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::UpdateTitleL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::UpdateTitleL()
{
    MPX_ENTER_EXIT( _L( "CMPXVideoPlaybackMediaDetailsViewer::UpdateTitleL" ) );

    HBufC* fileTitle = iController->FileDetails()->GenerateFileTitleL();
    CleanupStack::PushL( fileTitle );

    if ( fileTitle && fileTitle->Length() )
    {
        HBufC* heading  = iEikonEnv->AllocReadResourceL( R_MPX_TITLE_HEADING );
        CleanupStack::PushL( heading );

        TBuf<KMediaDetailsViewerMaxBufLen> titleField;
        titleField.Append( KLeftMargin );
        titleField.Append( *heading );
        titleField.Append( KHeadingValueSeperator );

        iTitleScroller.ScrollText( *fileTitle, titleField );

        iTitleLabel->SetTextL( titleField );

        iScrolledTextUpdated = ETrue;

        CleanupStack::PopAndDestroy( heading );
    }

    CleanupStack::PopAndDestroy( fileTitle );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::ViewerRect
// -------------------------------------------------------------------------------------------------
//
TRect CMPXVideoPlaybackMediaDetailsViewer::ViewerRect()
{
    return iViewerRect;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::TTextScroller
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::TTextScroller()
    : iDelayBeginningTick( KInvalidTick )
    , iTextScrollPos( 0 )
    , iDelay( EFalse )
    , iScroll( ETrue )
    , iSrcTextLen( 0 )
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::IsScrollNeeded
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::IsScrollNeeded()
{
    return iScroll;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::IsUpdateNeeded
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::IsUpdateNeeded()
{
    MPX_ENTER_EXIT( _L( "CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::IsUpdateNeeded" ) );

    // skip the scrolling operation if the loop for delay is going on
    TBool skipForTimerDelay = EFalse;

    // add a delay after each complete scrolling
    if ( iScroll && iDelayBeginningTick != KInvalidTick )
    {
        if ( ( User::NTickCount() - iDelayBeginningTick ) >= KScrollTimerDelayTickCounts )
        {
            iDelayBeginningTick = KInvalidTick;
        }
        else
        {
            skipForTimerDelay = ETrue;
        }
    }

    // start delay
    if ( iScroll && !skipForTimerDelay && iDelay )
    {
        iDelay = EFalse;
        iDelayBeginningTick = User::NTickCount();
        skipForTimerDelay = ETrue;
    }

    return iScroll && !skipForTimerDelay;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::ScrollText
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::ScrollText(
        const TDesC& aSrcText,
        TDes& aDesText )
{
    MPX_ENTER_EXIT( _L( "CMPXVideoPlaybackMediaDetailsViewer::TTextScroller::ScrollText" ) );

    if ( 0 == iSrcTextLen )
    {
        iSrcTextLen = aSrcText.Length();
    }

    ASSERT( aSrcText.Length() == iSrcTextLen );
    if ( aSrcText.Length() >= KMediaDetailsViewerVisibleCharacters )
    {
        aDesText.Append( aSrcText.Mid( iTextScrollPos,
                KMediaDetailsViewerVisibleCharacters ) );

        if ( iTextScrollPos ==  ( iSrcTextLen - KMediaDetailsViewerVisibleCharacters ) )
        {
            iTextScrollPos = 0;
            iDelay = ETrue;
        }
        else
        {
            iTextScrollPos++;
        }
        iScroll = ETrue;

    }
    else
    {
        aDesText.Append( aSrcText );
        iScroll = EFalse;
    }
}

//  End of File
