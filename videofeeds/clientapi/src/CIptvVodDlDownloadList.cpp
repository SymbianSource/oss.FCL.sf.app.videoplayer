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
* Description:    Contains list of download items*
*/





// INCLUDE FILES
#include <s32mem.h>
#include "IptvDebug.h"

#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvVodDlDownloadList.h"
#include "CIptvVodDlDownloadListItem.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::CIptvVodDlDownloadList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvVodDlDownloadList::CIptvVodDlDownloadList()
    {
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVodDlDownloadList::ConstructL()
    {
    iList.Reset();
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlDownloadList* CIptvVodDlDownloadList::NewL()
    {
    CIptvVodDlDownloadList* self = new( ELeave ) CIptvVodDlDownloadList;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

    
// Destructor
EXPORT_C CIptvVodDlDownloadList::~CIptvVodDlDownloadList()
    {
    TInt i;
    TInt count = iList.Count();    
    
    //free all heap objects
    for(i = 0; i < count; i++)
        {
        delete iList[i];
        iList[i] = NULL;
        }
        
    //free pointers
    iList.Close();
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadList::InternalizeL(RReadStream& aStream)
    {
    TUint32 itemCount = aStream.ReadUint32L();
    TUint32 i;
    for(i = 0; i < itemCount; i++)
        {
        CIptvVodDlDownloadListItem* item = CIptvVodDlDownloadListItem::NewL();
        CleanupStack::PushL(item); // 1->
        item->InternalizeL(aStream); 
        iList.AppendL(item);
        CleanupStack::Pop(item); // <-1
        }    
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlDownloadList::ExternalizeL(RWriteStream& aStream)
    {
    aStream.WriteUint32L(iList.Count());
    TUint32 i;
    for(i = 0; i < iList.Count(); i++)
        {
        iList[i]->ExternalizeL(aStream);
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvVodDlDownloadList::CountExternalizeSize()
    {
    TUint32 i, size = 4;
    for(i = 0; i < iList.Count(); i++)
        {
        size += iList[i]->CountExternalizeSize();
        }
    return size;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::GetDownload
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlDownloadListItem* CIptvVodDlDownloadList::GetListItem(CIptvMyVideosGlobalFileId& aGlobalFileId)
    {
    TInt i;
    for(i = 0; i < iList.Count(); i++)
        {
        if(iList[i]->iGlobalFileId->iDrive == aGlobalFileId.iDrive &&
           iList[i]->iGlobalFileId->iFileId == aGlobalFileId.iFileId &&
           iList[i]->iGlobalFileIdIsValid)
            {
            return iList[i];
            }
        }

    IPTVLOGSTRING3_LOW_LEVEL("CIptvVodDlDownloadList:: could not find download with file ID %d, drive %d",
                              aGlobalFileId.iFileId,
                              aGlobalFileId.iDrive);

    return NULL;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::GetDownload
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlDownloadListItem* CIptvVodDlDownloadList::GetListItem(TUint32 aServiceId, TUint32 aContentId)
    {
    TInt i;
    for(i = 0; i < iList.Count(); i++)
        {
        if(iList[i]->iServiceId == aServiceId &&
           iList[i]->iContentId == aContentId)
            {
            return iList[i];
            }
        }

    IPTVLOGSTRING3_LOW_LEVEL("CIptvVodDlDownloadList:: could not find download with service ID %d, content ID %d",
                              aServiceId,
                              aContentId);

    return NULL;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::Delete
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvVodDlDownloadList::Delete(CIptvMyVideosGlobalFileId& aGlobalFileId)
    {
    TInt i;
    for(i = 0; i < iList.Count(); i++)
        {
        if(iList[i]->iGlobalFileId->iDrive == aGlobalFileId.iDrive &&
           iList[i]->iGlobalFileId->iFileId == aGlobalFileId.iFileId &&
           iList[i]->iGlobalFileIdIsValid)
            {
            delete iList[i];
            iList.Remove(i);
            iList.Compress();
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlDownloadList::Delete
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvVodDlDownloadList::Delete(TUint32 aServiceId, TUint32 aContentId)
    {
    TInt i;
    for(i = 0; i < iList.Count(); i++)
        {
        if(iList[i]->iServiceId == aServiceId &&
           iList[i]->iContentId == aContentId)
            {
            delete iList[i];
            iList.Remove(i);
            iList.Compress();
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
