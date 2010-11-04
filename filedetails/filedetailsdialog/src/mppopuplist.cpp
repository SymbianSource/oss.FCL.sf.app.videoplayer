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
* Description:   CMPPopupList.cpp*
*/


// Version : %version: 15 %

#include <aknpopuplayout.h>
#include <eiklbx.h>
#include <MPFileDetails.rsg>
#include "mppopuplist.h"
#include "mpxvideo_debug.h"


#include <drmuihandling.h>
#include <drmutility.h>
#include <drmerrorhandling.h>
using namespace DRM;


// -----------------------------------------------------------------------------
// CMPPopupList::NewL()
// -----------------------------------------------------------------------------
//
CMPPopupList* CMPPopupList::NewL( CEikListBox* aListBox,
                                  const TDesC& aFilePath,
                                  TBool aDrmProtected,
                                  TBool aReadingDCFOnly )
{
    MPX_ENTER_EXIT(_L("CMPPopupList::NewL()"));

    CMPPopupList* self= new (ELeave) CMPPopupList();
    CleanupStack::PushL(self);
    self->ConstructL( aListBox, aFilePath, aDrmProtected, aReadingDCFOnly );
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CMPPopupList::NewL()
// -----------------------------------------------------------------------------
//
CMPPopupList* CMPPopupList::NewL( CEikListBox* aListBox,
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                  RFile64& aFile,
#else
                                  RFile& aFile,
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                  TBool aDrmProtected,
                                  TBool aReadingDCFOnly )
{
    MPX_DEBUG(_L("CMPPopupList::NewL (%d)"),aDrmProtected);

    CMPPopupList* self= new (ELeave) CMPPopupList();
    CleanupStack::PushL(self);
    self->ConstructL( aListBox, aFile, aDrmProtected, aReadingDCFOnly );
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CMPPopupList::ConstructL()
// -----------------------------------------------------------------------------
//
void CMPPopupList::ConstructL( CEikListBox* aListBox,
							   const TDesC& aFilePath,
							   TBool aDrmProtected,
							   TBool aReadingDCFOnly )
{
    MPX_ENTER_EXIT(_L("CMPPopupList::ConstructL()"));

    iFilePath = aFilePath.AllocL();
    iDrmProtected = aDrmProtected;
    iReadingDCFOnly = aReadingDCFOnly;
    iListBox = aListBox;
    TInt cbaResource;

    //
	// Construct the softkeys corresponding to Drm or non-Drm status
	//
    if ( iDrmProtected && !iReadingDCFOnly )
    {
		cbaResource = R_MP_DRM_FILE_DETAILS_SOFTKEYS_OK_VIEW__OK;
	}
	else
	{
		cbaResource = R_AVKON_SOFTKEYS_OK_EMPTY__OK;
	}

	//
	// let base class construct the pop-up list
	//
    CAknPopupList::ConstructL( aListBox, cbaResource, AknPopupLayouts::EMenuDoubleWindow );
}

// CMPPopupList::ConstructL()
// -----------------------------------------------------------------------------
//
void CMPPopupList::ConstructL( CEikListBox* aListBox,
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                               RFile64& aFile,
#else
                               RFile& aFile,
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                               TBool aDrmProtected,
                               TBool aReadingDCFOnly )
{
    MPX_ENTER_EXIT(_L("CMPPopupList::ConstructL()"));

    User::LeaveIfError( iFileHandle.Duplicate( aFile ) );

    iDrmProtected = aDrmProtected;
    iReadingDCFOnly = aReadingDCFOnly;
    iListBox = aListBox;
    TInt cbaResource;

    //
    // Construct the softkeys corresponding to Drm or non-Drm status
    //
    if ( iDrmProtected && !iReadingDCFOnly )
    {
        cbaResource = R_MP_DRM_FILE_DETAILS_SOFTKEYS_OK_VIEW__OK;
    }
    else
    {
        cbaResource = R_AVKON_SOFTKEYS_OK_EMPTY__OK;
    }

    //
    // let base class construct the pop-up list
    //
    CAknPopupList::ConstructL( aListBox, cbaResource, AknPopupLayouts::EMenuDoubleWindow );
}

// -----------------------------------------------------------------------------
// CMPPopupList::~CMPPopupList
// -----------------------------------------------------------------------------
//
CMPPopupList::~CMPPopupList()
{
MPX_ENTER_EXIT(_L("CMPPopupList::~CMPPopupList()"));

    if( iFileHandle.SubSessionHandle() )
    {
        iFileHandle.Close();
    }

    if( iFilePath )
    {
        delete iFilePath;
    }
}

// -----------------------------------------------------------------------------
// CMPPopupList::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CMPPopupList::HandleListBoxEventL( CEikListBox*  /* aListBox   */ ,
                                        TListBoxEvent /* aEventType */ )
{
MPX_ENTER_EXIT(_L("CMPPopupList::HandleListBoxEventL()"));

    //
	// Do nothing except update softkeys according to DRM or non-DRM status
    //
    UpdateSoftkeysL();
}

// -----------------------------------------------------------------------------
// CMPPopupList::UpdateSoftkeysL
// -----------------------------------------------------------------------------
//
void CMPPopupList::UpdateSoftkeysL()
{
  MPX_ENTER_EXIT(_L("CMPPopupList::UpdateSoftkeysL()"));

    if ( iDrmProtected &&
		 !iReadingDCFOnly )
    {
        //
        // When item focus in pop-up details dlg is on Drm item (i.e. License),
        // LSK is 'Ok' and RSK is 'View'
        //
        ButtonGroupContainer()->SetCommandSetL(R_MP_DRM_FILE_DETAILS_SOFTKEYS_OK_VIEW__OK);
    }
    else
    {
        //
        // When item focus in pop-up details dlg is on all other non-Drm items (i.e. Title),
        // LSK is 'Ok' and RSK is empty
        //
        ButtonGroupContainer()->SetCommandSetL(R_AVKON_SOFTKEYS_OK_EMPTY__OK);
    }
}

// -----------------------------------------------------------------------------
// CMPPopupList::LaunchDrmInfoL
// -----------------------------------------------------------------------------
//
void CMPPopupList::LaunchDrmInfoL()
{
    MPX_ENTER_EXIT(_L("CMPPopupList::LaunchDrmInfoL()"));
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    TInt64 size(0);
#else
    TInt size(0);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    TInt err(0);
    TInt handleExists(0);

    if ( iFileHandle.SubSessionHandle() )
    {
        err = iFileHandle.Size( size );
    }

    handleExists =(!err && size )? ETrue:EFalse;

    if(!handleExists)
    {
        TUint fileMode = EFileShareReadersOrWriters | EFileStream | EFileRead;
        RFs fs = CEikonEnv::Static()->FsSession();

        User::LeaveIfError(iFileHandle.Open(fs, iFilePath->Des() , fileMode));
    }

    if ( iDrmProtected && ( handleExists || iFilePath ) )
    {
        CDrmUiHandling* drmUiHandler = CDrmUiHandling::NewLC();
        TRAPD( err, drmUiHandler->ShowDetailsViewL(iFileHandle) );

        if( err != KErrNone )
        {
            drmUiHandler->GetErrorHandler().HandleErrorL( iFileHandle,
                                                          ContentAccess::EPlay,
                                                          err,
                                                          NULL);
        }
        CleanupStack::PopAndDestroy(1); //  drmUiHandler
    }
}

// -----------------------------------------------------------------------------
// CMPPopupList::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CMPPopupList::ProcessCommandL(TInt aCommandId)
{
	MPX_ENTER_EXIT(_L("CMPPopupList::ProcessCommandL()"));

	//
    // Respond to softkey events.
	// MP Popup only handles the 'View' softkey event,
	// all other softkey events are handled by Avkon Pop-up
	//
	switch (aCommandId)
	{
		case EAknSoftkeyView:
            LaunchDrmInfoL();
		    break;
		default:
			CAknPopupList::ProcessCommandL(aCommandId);
			break;
	}
}

// -----------------------------------------------------------------------------
// CMPPopupList::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CMPPopupList::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	MPX_ENTER_EXIT(_L("CMPPopupList::OfferKeyEventL()"));

    TKeyResponse ret = EKeyWasNotConsumed;

    if(aKeyEvent.iCode == EKeyEscape)
    {
        CancelPopup();
    }
    else
    {
        // 
        // pass all remaining events to the list box class
        //
        ret = iListBox->OfferKeyEventL(aKeyEvent, aType);
        
        //
        // Update softkeys according to DRM or non-DRM status
        //
        UpdateSoftkeysL();
    }	

    return ret;
}

// -----------------------------------------------------------------------------
// CMPPopupList::ReConstructDrmSoftkeysL
// -----------------------------------------------------------------------------
//
void CMPPopupList::ReConstructDrmSoftkeysL( TBool aDrmProtected )
{
    MPX_ENTER_EXIT(_L("CMPPopupList::ReConstructDrmSoftkeysL()"));

    //
    // This method is needed to re-evaluate DRM protection since it may
    // be inaccurate for playlist during construction time.
    // Hence, the related softkeys need to be re-constructed here.
    //

    iDrmProtected = aDrmProtected;

    if ( iDrmProtected && !iReadingDCFOnly )
    {
        ButtonGroupContainer()->SetCommandSetL(R_MP_DRM_FILE_DETAILS_SOFTKEYS_OK_VIEW__OK);
    }
}

// End of File
