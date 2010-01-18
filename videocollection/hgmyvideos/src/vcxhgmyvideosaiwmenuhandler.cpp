/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class to manage AIW provided menu items*
*/


#include <AiwServiceHandler.h>
#include <eikmenup.h>
#include <mediarecognizer.h>
#include <vcxhgmyvideos.rsg>

#include "vcxhgmyvideosaiwmenuhandler.h"
#include "vcxhgmyvideos.hrh"
#include "IptvDebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::CHgMyVideosAiwMenuHandler()
// ---------------------------------------------------------------------------
//
CHgMyVideosAiwMenuHandler::CHgMyVideosAiwMenuHandler()
    {
    IPTVLOGSTRING_LOW_LEVEL(
            "CHgMyVideosAiwMenuHandler::CHgMyVideosAiwMenuHandler()");
    }

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::~CHgMyVideosAiwMenuHandler()
// ---------------------------------------------------------------------------
//
CHgMyVideosAiwMenuHandler::~CHgMyVideosAiwMenuHandler()
    {
    IPTVLOGSTRING_LOW_LEVEL(
            "CHgMyVideosAiwMenuHandler::~CHgMyVideosAiwMenuHandler()");

    delete iMediaRecognizer;
    delete iAiwServiceHandler;
    delete iPath;
    delete iMimeType;
    }

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::AttachMenuL()
// ---------------------------------------------------------------------------
//
void CHgMyVideosAiwMenuHandler::AttachMenuL( TInt aMenuPaneId )
    {
    IPTVLOGSTRING2_LOW_LEVEL(
            "CHgMyVideosAiwMenuHandler::AttachMenuL(%d)", aMenuPaneId );

    if ( ! iAiwServiceHandler )
        {
        iAiwServiceHandler = CAiwServiceHandler::NewL();
        }

    iAiwServiceHandler->AttachMenuL(
            aMenuPaneId, R_VCXHGMYVIDEOS_AIW_ASSIGN_INTEREST );
    }

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::DetachMenu()
// ---------------------------------------------------------------------------
//
void CHgMyVideosAiwMenuHandler::DetachMenu( TInt aMenuPaneId )
    {
    IPTVLOGSTRING2_LOW_LEVEL(
            "CHgMyVideosAiwMenuHandler::DetachMenu(%d)", aMenuPaneId );

    iAiwServiceHandler->DetachMenu(
            aMenuPaneId, R_VCXHGMYVIDEOS_AIW_ASSIGN_INTEREST );
    }

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::TryHandleSubmenuL()
// ---------------------------------------------------------------------------
//
TBool CHgMyVideosAiwMenuHandler::TryHandleSubmenuL( CEikMenuPane* aMenuPane )
    {
    IPTVLOGSTRING_LOW_LEVEL("CHgMyVideosAiwMenuHandler::HandleSubmenuL()");
    return iAiwServiceHandler->HandleSubmenuL( *aMenuPane );
    }

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::InitializeMenuPaneL()
// ---------------------------------------------------------------------------
//
void CHgMyVideosAiwMenuHandler::InitializeMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane,
        const TDesC& aPath,
        const TDesC& aMimeType )
    {
    IPTVLOGSTRING_LOW_LEVEL("CHgMyVideosAiwMenuHandler::InitializeMenuPaneL()");

    // Let provider add its menu items to the view menu.
    // If placeholder has been deleted, no AIW option is displayed.
    // Note! Filehandles are not used, only file paths.
    CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();

    FillAiwParamsL( paramList, aPath, aMimeType );

    iAiwServiceHandler->InitializeMenuPaneL(
            *aMenuPane,
            aResourceId,
            EVcxHgMyVideosCmdAiwBase,
            paramList );
    }

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::IsAiwCommand()
// ---------------------------------------------------------------------------
//
TBool CHgMyVideosAiwMenuHandler::IsAiwCommand( TInt aCommand )
    {
    TBool res = EFalse;

    if ( iAiwServiceHandler )
        {
        res = iAiwServiceHandler->ServiceCmdByMenuCmd( aCommand ) != KAiwCmdNone;
        }

    IPTVLOGSTRING2_LOW_LEVEL("CHgMyVideosAiwMenuHandler::IsAiwCommand(%d)", res);

    return res;
    }

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::HandleAiwCommandL()
// ---------------------------------------------------------------------------
//
void CHgMyVideosAiwMenuHandler::HandleAiwCommandL(
        TInt aCommand,
        const TDesC& aPath,
        const TDesC& aMimeType )
    {
    IPTVLOGSTRING_LOW_LEVEL("CHgMyVideosAiwMenuHandler::HandleAiwCommandL()");

    //
    //  Save the command parameters so they can be used to cancel
    //  menu if needed
    //
    iCommand = aCommand;

    HBufC* newPath = aPath.AllocL();
    if ( iPath )
        {
        delete iPath;
        iPath = NULL;
        }
    iPath = newPath;

    HBufC* newMimeType = aMimeType.AllocL();
    if ( iMimeType )
        {
        delete iMimeType;
        iMimeType = NULL;
        }
    iMimeType = newMimeType;

    CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();
    FillAiwParamsL( paramList, aPath, aMimeType );

    iMenuActive = ETrue;
    // Call AIW service handler
    iAiwServiceHandler->ExecuteMenuCmdL(
            aCommand,
            paramList,
            iAiwServiceHandler->OutParamListL() );
    iMenuActive = EFalse;
    }

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::IsMenuActive()
// ---------------------------------------------------------------------------
//
TBool CHgMyVideosAiwMenuHandler::IsMenuActive()
    {
    return iMenuActive;
    }

// ---------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::FillAiwParamsL()
// ---------------------------------------------------------------------------
//
void CHgMyVideosAiwMenuHandler::FillAiwParamsL(
        CAiwGenericParamList& aAiwParamsList,
        const TDesC& aPath,
        const TDesC& aMimeType )
    {
    IPTVLOGSTRING_LOW_LEVEL("CHgMyVideosAiwMenuHandler::FillAiwParamsL()");

    HBufC* path = aPath.AllocLC();
    TAiwVariant varMime( aMimeType );
    if ( ! aMimeType.Length() )
        {
        if ( ! iMediaRecognizer )
            {
            iMediaRecognizer = CMediaRecognizer::NewL();
            }
        varMime.Set( iMediaRecognizer->MimeTypeL( *path ) );
        }

    TAiwGenericParam paramMime( EGenericParamMIMEType, varMime );
    aAiwParamsList.AppendL( paramMime );

    TAiwVariant varFile( *path );
    TAiwGenericParam paramFile( EGenericParamFile, varFile );
    aAiwParamsList.AppendL( paramFile );

    CleanupStack::PopAndDestroy( path );
    }

//------------------------------------------------------------------------------
// CHgMyVideosAiwMenuHandler::CloseOpenAiwMenuL()
//------------------------------------------------------------------------------
//
void CHgMyVideosAiwMenuHandler::CloseOpenAiwMenuL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CHgMyVideosAiwMenuHandler::CloseOpenAiwMenuL()");

    if ( iMenuActive )
        {
        CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();

        FillAiwParamsL( paramList, iPath->Des(), iMimeType->Des() );

        //  Call AIW service handler to cancel the menu
        iAiwServiceHandler->ExecuteMenuCmdL(
                iCommand,
                paramList,
                iAiwServiceHandler->OutParamListL(),
                KAiwOptCancel );

        // Reset the member variables so command will not be reset
        iMenuActive = EFalse;
        iCommand    = 0;
        delete iPath;
        iPath = NULL;
        delete iMimeType;
        iMimeType = NULL;
        }
    }

//  End of File
