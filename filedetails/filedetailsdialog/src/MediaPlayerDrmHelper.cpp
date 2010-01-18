/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   DRM helper for Media files.*
*/



// INCLUDE FILES
#include    <DRMHelper.h>
#include    <bamdesca.h>
#include    <bautils.h>
#include    <coemain.h>
#include    <StringLoader.h>
#include    <aknnotewrappers.h>
#include    <data_caging_path_literals.hrh>

#include    "MediaPlayerDrmHelper.h"
#include    <MPFileDetails.h>

#include    <drmuihandling.h>
#include    <drmautomatedusage.h>
#include    <Oma2Agent.h>

#include    "mpxvideo_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::CMediaPlayerDrmHelper(aDrmHelper,aUIController)
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMediaPlayerDrmHelper::CMediaPlayerDrmHelper(
                          CDRMHelper* aDrmHelper,
                          CMPVideoPlayerUIController* /*aUIController*/ )
    : iDrmHelper(aDrmHelper)
    , iEnv(CCoeEnv::Static())
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::CMediaPlayerDrmHelper()"));
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::CMediaPlayerDrmHelper(aDrmHelper)
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMediaPlayerDrmHelper::CMediaPlayerDrmHelper( CDRMHelper* aDrmHelper )
    : iDrmHelper(aDrmHelper)
    , iEnv( CCoeEnv::Static() )
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::CMediaPlayerDrmHelper()"));
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMediaPlayerDrmHelper::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::ConstructL()"));
    iDrmUiHandling = DRM::CDrmUiHandling::NewL(iEnv);
    iDrmUtility = &iDrmUiHandling->GetUtility();
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::NewL(aDrmHelper,aUIController)
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMediaPlayerDrmHelper*
CMediaPlayerDrmHelper::NewL( CDRMHelper* aDrmHelper,
                             CMPVideoPlayerUIController* aUIController )
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::NewL()"));

    CMediaPlayerDrmHelper* self =
        new (ELeave) CMediaPlayerDrmHelper(aDrmHelper,aUIController);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::NewL(aDrmHelper,aUIController)
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMediaPlayerDrmHelper* CMediaPlayerDrmHelper::NewL( CDRMHelper* aDrmHelper )
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::NewL()"));

    CMediaPlayerDrmHelper* self =
        new (ELeave) CMediaPlayerDrmHelper(aDrmHelper);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::~CMediaPlayerDrmHelper
// Destructor
// -----------------------------------------------------------------------------
//
CMediaPlayerDrmHelper::~CMediaPlayerDrmHelper()
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::~CMediaPlayerDrmHelper()"));

    delete iInfoUrl;
    delete iFileName;
    delete iPreviewUri;
    delete iRightsConstraints;
    delete iDrmUiHandling;
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::LoadDrmUsageRightsInfoL
// -----------------------------------------------------------------------------
//
void
CMediaPlayerDrmHelper::LoadDrmUsageRightsInfoL( const TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::LoadDrmUsageRightsInfoL()"));


    if ( BaflUtils::FileExists(iEnv->FsSession(), aFileName) )
    {
        iExpired = EFalse;
        delete iInfoUrl;
        iInfoUrl = NULL;
        delete iPreviewUri;
        iPreviewUri = NULL;
        delete iRightsConstraints;
        iRightsConstraints = NULL;
        delete iFileName;
        iFileName = NULL;

        iFileName = aFileName.AllocL();

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        RFile64 fileHandle;
#else
        RFile fileHandle;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        TUint fileMode = EFileShareReadersOrWriters |
                         EFileStream |
                         EFileRead;

        CreateFileHandleLC( fileHandle, *iFileName, fileMode );
        LoadDrmUsageRightsInfoL( fileHandle );
        CleanupStack::PopAndDestroy(); // fileHandle
    }
    else
    {
        User::Leave(KErrNotFound);
    }
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::GetDrmUsageRightsStatus
// -----------------------------------------------------------------------------
//
void
CMediaPlayerDrmHelper::GetDrmUsageRightsStatus(TMediaPlayerRightsStatus& aStatus)
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::GetDrmUsageRightsStatus()"));

    if ( ! iProtected )
    {
        aStatus = EMediaPlayerFull;
    }
    // This check ensures that the non-OMA DRM is assigned the correct status.
    // If content is not OMA DRM then currently the only other choice is WM DRM.
    else if ( iProtected && ! iOMAProtected )
    {
        if ( iExpired )
        {
            aStatus = KMediaPlayerWmdrmExpired;
        }
        else
        {
            aStatus = KMediaPlayerWmdrmValid;
        }
    }
    else if ( iExpired )
    {
        aStatus = EMediaPlayerExpired;
    }
    else if ( iRightsConstraints )
    {
        if ( iRightsConstraints->IsPreview() )
        {
            aStatus = EMediaPlayerPreview;
        }
        else if ( iRightsConstraints->FullRights() )
        {
            aStatus = EMediaPlayerFull;
        }
        else
        {
            TTime time;
            TTime startTime;

            time.HomeTime();
            TInt error = GetDrmStartTime(startTime);

            // Check that usage time has already started
            if (!error && time < startTime)
            {
                aStatus = EMediaPlayerExpired;
            }
            else
            {
                aStatus = EMediaPlayerRestricted;
            }
        }
    }
    else
    {
        aStatus = EMediaPlayerMissing;
    }

    MPX_DEBUG(_L(
        "CMediaPlayerDrmHelper::GetDrmUsageRightsStatus() ret %d"),
        aStatus);
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::GetDrmStartTime
// -----------------------------------------------------------------------------
//
TInt CMediaPlayerDrmHelper::GetDrmStartTime(TTime& aStartTime)
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::GetDrmStartTime()"));

    TInt error = KErrNone;

    if (!iProtected || !iOMAProtected)
    {
        error = KErrNotFound;
    }
    else if (iRightsConstraints)
    {
        TRAP(error, iRightsConstraints->GetStartTimeL(aStartTime));
    }
    else
    {
        error = KErrNotReady;
    }

    return error;
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::DrmHelper
// -----------------------------------------------------------------------------
//
CDRMHelper* CMediaPlayerDrmHelper::DrmHelper()
{
    MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::DrmHelper()"));
    return iDrmHelper;
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::LoadDrmUsageRightsInfoL
// -----------------------------------------------------------------------------
//
void CMediaPlayerDrmHelper::LoadDrmUsageRightsInfoL( 
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                                     RFile64& aFile )
#else
                                                     RFile& aFile )
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
{
   MPX_ENTER_EXIT(_L("CMediaPlayerDrmHelper::LoadDrmUsageRightsInfoL()"));

    iExpired = EFalse;
    delete iInfoUrl;
    iInfoUrl = NULL;
    delete iPreviewUri;
    iPreviewUri = NULL;
    delete iRightsConstraints;
    iRightsConstraints = NULL;
    delete iFileName;
    iFileName = NULL;

    //
    //  Get the agent information and protection status
    //
    TPtrC agentId;
    DRM::TDrmProtectionStatus protectionStatus;
    iDrmUtility->GetDrmInfoL( aFile, agentId, protectionStatus );
    iProtected = ( protectionStatus == DRM::EUProtected );
    iOMAProtected = ( agentId.Match(KOmaDrm2AgentName) != KErrNotFound );

    // Check to make sure that DRM helper is accessed only for OMA DRM
    if ( iProtected && iOMAProtected )
    {
        CDRMHelperRightsConstraints* ignore1 = NULL;
        CDRMHelperRightsConstraints* ignore2 = NULL;
        CDRMHelperRightsConstraints* ignore3 = NULL;

        // No replacement for this currently available via DRM Utility
        TRAPD( error, iDrmHelper->GetRightsDetailsL( aFile,
                                                     ContentAccess::EPlay,
                                                     iExpired,
                                                     iDrmSendingAllowed,
                                                     iRightsConstraints,
                                                     ignore1,
                                                     ignore2,
                                                     ignore3 ) );

        // Delete ignored constraints
        delete ignore1;
        ignore1 = NULL;
        delete ignore2;
        ignore2 = NULL;
        delete ignore3;
        ignore3 = NULL;

        if ( error == KErrCANoPermission )
        {
            iExpired = ETrue;
        }
        else if (error != KErrCANoRights)
        {
            User::LeaveIfError( error );
        }
    }
    // get the attributes for protected WMDRM content
    else if ( iProtected && ! iOMAProtected )
    {
        CContent* content = CContent::NewLC(aFile);

        TInt value;
        TInt err;

        err = content->GetAttribute( ECanPlay, value );

        if ( err == KErrNone )
        {
            iExpired = ( ! value );
            MPX_DEBUG(_L("   iExpired = %d"), iExpired);
        }
        else
        {
        MPX_DEBUG(_L("   ECanPlay failed"));
        }

        err = content->GetAttribute( EIsForwardable, value );

        if ( err == KErrNone )
        {
            iDrmSendingAllowed = value;
            MPX_DEBUG(_L("   iDrmSendingAllowed = %d"), iDrmSendingAllowed);
        }
        else
        {
        MPX_DEBUG(_L("   EIsForwardable failed"));
        }

        CleanupStack::PopAndDestroy( content );
    }
    else
    {
        iExpired = EFalse;
        iDrmSendingAllowed = ETrue;
    }
}

// -----------------------------------------------------------------------------
// CMediaPlayerDrmHelper::CreateFileHandleLC
// -----------------------------------------------------------------------------
//
void CMediaPlayerDrmHelper::CreateFileHandleLC( 
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                                RFile64& aHandle,
#else
                                                RFile& aHandle,
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                                const TDesC& aName,
                                                TUint aFileMode )
{
    // Get the file server session
    RFs fs = CEikonEnv::Static()->FsSession();
    User::LeaveIfError(aHandle.Open(fs, aName, aFileMode));
    CleanupClosePushL(aHandle);
}

//  End of File
