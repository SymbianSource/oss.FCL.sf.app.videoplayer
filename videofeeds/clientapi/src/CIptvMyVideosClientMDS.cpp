/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Implements My Videos Client API.*
*/




#include <e32svr.h>
#include <s32mem.h>

#include "IptvClientServerCommon.h"
#include "iptvvideostoragemdsadaptation.h"
#include "IptvDebug.h"
#include "CIptvMyVideosClient.h"
#include "MIptvMyVideosClientObserver.h"
#include "IptvServer.pan"
#include "CIptvMyVideosFolder.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvMyVideosVideoBriefDetails.h"
#include "CIptvMyVideosVideoFullDetails.h"
#include "CIptvMyVideosContentUpdateObserver.h"
#include "CIptvMyVideosTotalVideoLengthObserver.h"
#include "CIptvMyVideosCopyOrMoveObserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CIptvMyVideosClient
// -----------------------------------------------------------------------------
//
CIptvMyVideosClient::CIptvMyVideosClient(
    MIptvMyVideosClientObserver& aClientObserver )
  : CActive( EPriorityStandard ),
    iClientObserver( aClientObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::ConstructL
// -----------------------------------------------------------------------------
//
void CIptvMyVideosClient::ConstructL( TBool aMaintenance )
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::ConstructL -- Enter");

    iMdsAdaptation = CIptvVideoStorageMDSAdaptation::NewL(
            iClientObserver,
            aMaintenance );

    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::ConstructL -- Exit");
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvMyVideosClient* CIptvMyVideosClient::NewL(
        MIptvMyVideosClientObserver& aClientObserver,
        TBool aMaintenance /*= EFalse*/)
    {
    CIptvMyVideosClient* self =
            CIptvMyVideosClient::NewLC( aClientObserver, aMaintenance );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvMyVideosClient* CIptvMyVideosClient::NewLC(
        MIptvMyVideosClientObserver& aClientObserver,
        TBool aMaintenance /*= EFalse*/)
    {
    CIptvMyVideosClient* self =
            new (ELeave) CIptvMyVideosClient(aClientObserver);
    CleanupStack::PushL(self);
    self->ConstructL( aMaintenance );
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::~CIptvMyVideosClient
// -----------------------------------------------------------------------------
//
CIptvMyVideosClient::~CIptvMyVideosClient()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::~CIptvMyVideosClient -- Enter");

    // Cancel own requests, there shouldn't be any though.
    Cancel();

    delete iMdsAdaptation;

    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::~CIptvMyVideosClient -- Exit");
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetFolderListL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetFolderListL(
        CIptvMyVideosGlobalFolderId& /*aParentFolderId*/,
        TUint32 /*aFrom*/,
        TUint32 /*aAmount*/,
        TUint32& /*aTotalAmount*/,
        RPointerArray<CIptvMyVideosFolder>& /*aFolderList*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetFolderListL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetParentFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetParentFolderL(
        CIptvMyVideosGlobalFolderId& /*aFolderId*/,
        CIptvMyVideosGlobalFolderId& /*aParentFolder*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetParentFolderL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetFolderNameL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetFolderNameL(
        CIptvMyVideosGlobalFolderId& /*aFolderId*/,
        TDes& /*aName*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetFolderNameL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DeleteFolderReqL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteFolderReqL(
        CIptvMyVideosGlobalFolderId& /*aFolderId*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DeleteFolderReqL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DeleteFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteFolderL(
        CIptvMyVideosGlobalFolderId& /*aFolderId*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DeleteFolderL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetVideoListL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetVideoListL(
        CIptvMyVideosGlobalFolderId& aParentFolderId,
        TUint32 aFrom,
        TUint32 aAmount,
        TUint32& aTotalAmount,
        RPointerArray<CIptvMyVideosVideoBriefDetails>& aVideoList)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetVideoListL");

    return iMdsAdaptation->GetVideoListL(
            aParentFolderId, aFrom, aAmount, aTotalAmount, aVideoList);
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetVideoDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetVideoDetailsL(
        CIptvMyVideosGlobalFileId& aFileId,
        CIptvMyVideosVideoFullDetails& aFullDetails)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetVideoDetailsL");

    return iMdsAdaptation->GetVideoDetailsL( aFileId, aFullDetails );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetVideoDetailsForPathL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetVideoDetailsForPathL(
        const TDesC& aLocalPath,
        CIptvMyVideosVideoFullDetails& aFullDetails)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetVideoDetailsForPathL");

    return iMdsAdaptation->GetVideoDetailsForPathL( aLocalPath, aFullDetails );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::GetTotalVideoLengthRequestL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetTotalVideoLengthRequestL()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::GetTotalVideoLengthRequestL");

    return iMdsAdaptation->GetTotalVideoLengthRequestL();
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::SetVideoDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::SetVideoDetailsL(
        CIptvMyVideosVideoFullDetails& aVideoFullDetails)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::SetVideoDetailsL()");

    return iMdsAdaptation->SetVideoDetailsL( aVideoFullDetails );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DeleteVideoReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteVideoReqL(
        CIptvMyVideosGlobalFileId& aFileId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DeleteVideoReqL");

    return iMdsAdaptation->DeleteVideoReqL( aFileId );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DeleteVideoL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteVideoL(
        CIptvMyVideosGlobalFileId& aFileId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DeleteVideoL()");

    return iMdsAdaptation->DeleteVideoL( aFileId );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CopyReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CopyReqL(
        CIptvMyVideosGlobalFileId& aSourceFileId,
        CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CopyReqL()");

    return iMdsAdaptation->CopyReqL( aSourceFileId, aTargetFolderId );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CopyL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CopyL(
        CIptvMyVideosGlobalFileId& aSourceFileId,
        CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CopyL()");

    return iMdsAdaptation->CopyL( aSourceFileId, aTargetFolderId );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CopyReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CopyReqL(
        CIptvMyVideosGlobalFolderId& /*aSourceFolderId*/,
        CIptvMyVideosGlobalFolderId& /*aTargetFolderId*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CopyReqL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CopyL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CopyL(
        CIptvMyVideosGlobalFolderId& /*aSourceFolderId*/,
        CIptvMyVideosGlobalFolderId& /*aTargetFolderId*/,
        TUint32& /*aFailed*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CopyL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::MoveReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::MoveReqL(
        CIptvMyVideosGlobalFileId& aSourceFileId,
        CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::MoveReqL()");

    return iMdsAdaptation->MoveReqL( aSourceFileId, aTargetFolderId );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::MoveL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::MoveL(
        CIptvMyVideosGlobalFileId& aSourceFileId,
        CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::MoveL()");

    return iMdsAdaptation->MoveL( aSourceFileId, aTargetFolderId );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::MoveReqL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::MoveReqL(
    CIptvMyVideosGlobalFolderId& /*aSourceFolderId*/,
    CIptvMyVideosGlobalFolderId& /*aTargetFolderId*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::MoveReqL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::MoveL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::MoveL(
        CIptvMyVideosGlobalFolderId& /*aSourceFolderId*/,
        CIptvMyVideosGlobalFolderId& /*aTargetFolderId*/,
        TUint32& /*aFailed*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::MoveL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CancelCopyOrMoveL()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CancelCopyOrMoveL()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CancelCopyOrMoveL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::RenameDatabaseFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::RenameDatabaseFolderL(
        CIptvMyVideosGlobalFolderId& /*aFolderId*/,
        TDesC& /*aName*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::RenameDatabaseFolderL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::NewDatabaseFolderL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::NewDatabaseFolderL(
        CIptvMyVideosGlobalFolderId& /*aParentFolderId*/,
        TDesC& /*aName*/,
        CIptvMyVideosGlobalFolderId& /*aNewFolderId*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::NewDatabaseFolderL not supported");

    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::CreateVideoL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::CreateVideoL(
        CIptvMyVideosVideoFullDetails& aFullDetails,
        TVideoType aVideoType,
        TUint32 aSizeEstimateInKiloBytes)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::CreateVideoL");

    return iMdsAdaptation->CreateVideoL(
            aFullDetails, aVideoType, aSizeEstimateInKiloBytes );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::EnsureFreeSpaceL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::EnsureFreeSpaceL(
        CIptvMyVideosGlobalFileId& aFileId,
        TUint32 aRequiredSpaceInKiloBytes)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::EnsureFreeSpaceL (single drive)");

    return iMdsAdaptation->EnsureFreeSpaceL(
            aFileId, aRequiredSpaceInKiloBytes );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::EnsureFreeSpaceL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::EnsureFreeSpaceL(
        CIptvMyVideosGlobalFileId& aCurrentFileId,
        TUint32 aRequiredSpaceInKiloBytes,
        CIptvMyVideosGlobalFileId& aNewFileId)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::EnsureFreeSpaceL (multi drive)");

    return iMdsAdaptation->EnsureFreeSpaceL(
            aCurrentFileId, aRequiredSpaceInKiloBytes, aNewFileId );
    }

// -----------------------------------------------------------------------------
// Count total video length and total file size (sum of all videos).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::GetTotalVideoLengthL(
    TIptvPlayTime& aTotalPlayTime,
    TIptvFileSize& aTotalFileSize )
    {
    return iMdsAdaptation->GetTotalVideoLengthL(
        aTotalPlayTime,
        aTotalFileSize );
    }

// -----------------------------------------------------------------------------
// Deletes all videos which are under downloading.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvMyVideosClient::DeleteAllDownloadsL()
    {
    return iMdsAdaptation->DeleteAllDownloadsL();
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::RunL
// -----------------------------------------------------------------------------
//
void CIptvMyVideosClient::RunL()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::RunL");
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::DoCancel
// -----------------------------------------------------------------------------
//
void CIptvMyVideosClient::DoCancel()
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::DoCancel");
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosClient::RunError
// -----------------------------------------------------------------------------
//
TInt CIptvMyVideosClient::RunError(TInt /*aError*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("My Videos Mgr ## CIptvMyVideosClient::RunError");
    return KErrNone;
    }
