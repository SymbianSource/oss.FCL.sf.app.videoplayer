/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Object to offer some common utility functions to Iptv engine testing*
*/




// INCLUDE FILES
#include <e32Base.h>
#include "mmf/common/mmfcontrollerpluginresolver.h"

#include "CIptvTestMVApiUtil.h"
#include "VCXTestLog.h"
#include "CIptvMyVideosVideoFullDetails.h"
#include "CIptvMyVideosGlobalFolderId.h"
#include "CIptvMyVideosFolder.h"

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
// CIptvTestMVApiUtil::CIptvTestMVApiUtil
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvTestMVApiUtil::CIptvTestMVApiUtil()
    {
    }

// -----------------------------------------------------------------------------
// CIptvTestMVApiUtil::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvTestMVApiUtil::ConstructL()
    {
    iIptvMyVideosClient = CIptvMyVideosClient::NewL(*this);
    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvTestMVApiUtil* CIptvTestMVApiUtil::NewL()
    {
    VCXLOGLO1(">>>CIptvTestMVApiUtil::NewL");
    CIptvTestMVApiUtil* self = new( ELeave ) CIptvTestMVApiUtil;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	VCXLOGLO1("<<<CIptvTestMVApiUtil::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::NewL
// Destructor
// -----------------------------------------------------------------------------
//    
CIptvTestMVApiUtil::~CIptvTestMVApiUtil()
    {
    VCXLOGLO1(">>>CIptvTestMVApiUtil::~CIptvTestMVApiUtil");
    if(iIptvMyVideosClient)
        {
        delete iIptvMyVideosClient;
        iIptvMyVideosClient=NULL;        
        }
    
    VCXLOGLO1("<<<CIptvTestMVApiUtil::~CIptvTestMVApiUtil");
    }
    
// -----------------------------------------------------------------------------
// CIptvTestMVApiUtil::
// returns name and size of the video
// 
// -----------------------------------------------------------------------------
//
TBool CIptvTestMVApiUtil::GetVideoInfo(CIptvMyVideosGlobalFileId& aFileId,TUint32& aServiceId, TDes& aVideoName, TDes& aFileName)
    {
    VCXLOGLO1(">>>CIptvTestMVApiUtil::GetVideoInfo");
    TBool rv = FALSE;
    
    CIptvMyVideosVideoFullDetails* p = NULL;
    
    p = CIptvMyVideosVideoFullDetails::NewL();
    CleanupStack::PushL( p );
    
    if(p)
        {
        VCXLOGLO1("CIptvTestMVApiUtil Asking videodetails");
        if(KErrNone == iIptvMyVideosClient->GetVideoDetailsL(aFileId, *p))
            {
            VCXLOGLO1("CIptvTestMVApiUtil videodetails received");	
            rv = TRUE;
            aFileName.Append( p->VideoPath() );        
            aVideoName.Append( p->Name() );
            aServiceId = p->iServiceId;
            }    
        }
        
    if(FALSE == rv)
        {
        aFileName.Append(_L(""));
        aVideoName.Append(_L(""));
        }
    
    CleanupStack::Pop();
    VCXLOGLO1("<<<CIptvTestMVApiUtil::GetVideoInfo");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvTestMVApiUtil::GetMyVideosVideoDetailsL
// -----------------------------------------------------------------------------
//    
TInt CIptvTestMVApiUtil::GetMyVideosVideoDetailsL(CIptvMyVideosGlobalFileId& aFileId, CIptvMyVideosVideoFullDetails& aVideoDetails)
	{
    VCXLOGLO1(">>>CIptvTestMVApiUtil::GetMyVideosVideoDetailsL");
    
    TInt ret = iIptvMyVideosClient->GetVideoDetailsL(aFileId, aVideoDetails);
    
    VCXLOGLO1("<<<CIptvTestMVApiUtil::GetMyVideosVideoDetailsL");
    
	return ret;
	}

// -----------------------------------------------------------------------------
// CIptvTestMVApiUtil::ReallycleanL
// Cleanup function.
// -----------------------------------------------------------------------------
//
TInt CIptvTestMVApiUtil::ReallycleanL()
    {
    VCXLOGLO1(">>>CIptvTestMVApiUtil::ReallycleanL");
    
    TUint32 videoCount;
    
    RPointerArray<CIptvMyVideosVideoBriefDetails> videoList;
    RPointerArray<CIptvMyVideosFolder> folderList;
    
    CleanupResetAndDestroyPushL(videoList);
    
    CIptvMyVideosGlobalFolderId* root = CIptvMyVideosGlobalFolderId::NewRootFolderL();
    CleanupStack::PushL(root);
    
	User::LeaveIfError(iIptvMyVideosClient->GetVideoListL(*root, 0, 0, videoCount, videoList));

	for(TInt i=0;i<videoList.Count();i++)
	   	{
   		iIptvMyVideosClient->DeleteVideoL(*videoList[i]->iFileId);
	   	}
    
    CleanupStack::PopAndDestroy(root);
    CleanupStack::PopAndDestroy(&videoList);
        
    VCXLOGLO1("<<<CIptvTestMVApiUtil::ReallycleanL");

    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CIptvTestMVApiUtil::DeleteVideoL
// -----------------------------------------------------------------------------
//    
TInt CIptvTestMVApiUtil::DeleteVideoL(CIptvMyVideosGlobalFileId& aFileId)
	{
    VCXLOGLO1(">>>CIptvTestMVApiUtil::DeleteVideoL");
    
    TInt ret = iIptvMyVideosClient->DeleteVideoL(aFileId);
    
    VCXLOGLO1("<<<CIptvTestMVApiUtil::DeleteVideoL");
    
	return ret;
	}
    
// -----------------------------------------------------------------------------
// CIptvTestMVApiUtil::DeleteResponse
// -----------------------------------------------------------------------------
//    
void CIptvTestMVApiUtil::DeleteResponse(TInt /*aStatus*/)
    {
	VCXLOGLO1(">>>CIptvTestMVApiUtil::DeleteResponse<<<");
    }
    

// -----------------------------------------------------------------------------
// CIptvTestMVApiUtil::ContentsUpdated
// -----------------------------------------------------------------------------
//    
void CIptvTestMVApiUtil::ContentsUpdated(TIptvContentUpdatedEvent /* aEvent */ )
    {
	VCXLOGLO1(">>>CIptvTestMVApiUtil::ContentsUpdated<<<");    
    }    
    
//  End of File  
