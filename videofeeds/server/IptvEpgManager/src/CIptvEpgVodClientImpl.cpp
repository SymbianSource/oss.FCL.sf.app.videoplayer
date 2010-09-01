/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/




// INCLUDE FILES
#include <e32base.h>
#include "IptvDebug.h"
#include "CIptvMediaContent.h"
#include "CIptvEpgVodClientImpl.h"
#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvEpgDb.h"
#include "CIptvEpgSession.h"
#include "CIptvSearchManager.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvEpgVodClientImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvEpgVodClientImpl::ConstructL()
    {       
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::ConstructL");        
    }

// ---------------------------------------------------------
// CIptvEpgVodClientImpl::NewL
// Two-phased constructor.
// Create instance of concrete implementation
// ---------------------------------------------------------
//
CIptvEpgVodClientImpl* CIptvEpgVodClientImpl::NewL( CIptvEpgDb& aEpgDb,
													CIptvEpgSession& aSession,
													CIptvSearchManager* aSearchManager )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::NewL");  

    CIptvEpgVodClientImpl* self = new(ELeave) CIptvEpgVodClientImpl( aEpgDb,
    																 aSession,
    																 aSearchManager );

    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvEpgVodClientImpl::~CIptvEpgVodClientImpl
// Destructor
// ---------------------------------------------------------
//
CIptvEpgVodClientImpl::~CIptvEpgVodClientImpl()
    {	
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::~CIptvEpgVodClientImpl");  

    iCategories.Close();
    }

// ---------------------------------------------------------
// CIptvEpgVodClientImpl::CIptvEpgVodClientImpl
// C++ default constructor
// ---------------------------------------------------------
//
CIptvEpgVodClientImpl::CIptvEpgVodClientImpl( CIptvEpgDb& aEpgDb,
											  CIptvEpgSession& aSession,
											  CIptvSearchManager* aSearchManager ) : 
    iEpgDb( aEpgDb ),
    iSearchManager( aSearchManager ),
    iSession( aSession )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::CIptvEpgVodClientImpl");  
    }

// ---------------------------------------------------------
// CIptvEpgVodClientImpl::GetEcgCategoryList
// Database query method
// ---------------------------------------------------------
//  
TInt CIptvEpgVodClientImpl::GetEcgCategoryListL(TIptvCategoryId aParentCategoryId, RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::GetEcgCategoryListL");  
    
    iSession.InitializeL();
    TInt error = iEpgDb.GetEcgCategoryListL(aParentCategoryId, aCategoryList);

    if ( iSearchManager )
        {
        CIptvVodContentCategoryBriefDetails* searchCategory = iSearchManager->SearchCategoryContentBriefDetailsL();
        if( searchCategory )
        	{
        	CleanupStack::PushL( searchCategory );
        	aCategoryList.AppendL( searchCategory );
        	CleanupStack::Pop( searchCategory );
        	}
        }

    return error;
    }

// ---------------------------------------------------------
// CIptvEpgVodClientImpl::GetCategoryDetails
// Database query method
// ---------------------------------------------------------
//  
TInt CIptvEpgVodClientImpl::GetCategoryDetailsL(TIptvCategoryId aCategoryId, CIptvVodContentCategoryBriefDetails& aCategory)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::GetCategoryDetailsL");  

    iSession.InitializeL();
    TInt error = iEpgDb.GetCategoryDetailsL(aCategoryId, aCategory);
    return error;
    }

// -----------------------------------------------------------------------------
// CIptvEpgVodClientImpl::GetParentCategory
// -----------------------------------------------------------------------------
TInt CIptvEpgVodClientImpl::GetParentCategoryL(TIptvCategoryId aCategoryId, TIptvCategoryId& aParentId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::GetParentCategoryL");  

    iSession.InitializeL();
    TInt error = iEpgDb.GetParentCategoryL(aCategoryId, aParentId);

    if( error == KErrNotSupported && iSearchManager ) // Not found from the database
    	{
    	// Let's ask from search manager if we have a match for category
    	error = iSearchManager->GetParentCategory( aCategoryId, aParentId );
    	}

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvEpgVodClientImpl::GetEcgList
// -----------------------------------------------------------------------------
TInt CIptvEpgVodClientImpl::GetEcgListL(TIptvCategoryId aCategoryId, TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::GetEcgListL");  

    iSession.InitializeL();
    TInt error = KErrNone;

    if( aCategoryId == KIptvRssSearchCategoryId && iSearchManager )
    	{
        iSearchManager->GetSearchContentBriefDetailsL( aCategoryId,
                                                       aSearchString,
                                                       aFrom,
                                                       aAmount,
                                                       aTotalAmount,
                                                       aEcgList );
    	}
    else
    	{
	    error = iEpgDb.GetEcgListL(aCategoryId, aSearchString, aFrom, aAmount, aTotalAmount, aEcgList);
    	}

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvEpgVodClientImpl::GetEcgAllList
// -----------------------------------------------------------------------------
TInt CIptvEpgVodClientImpl::GetEcgAllListL(TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::GetEcgAllListL");  

    iSession.InitializeL();
    TInt error = iEpgDb.GetEcgAllListL(aSearchString, aFrom, aAmount, aTotalAmount, aEcgList);
    return error;
    }

// -----------------------------------------------------------------------------
// CIptvEpgVodClientImpl::GetContentDetails
// -----------------------------------------------------------------------------
TInt CIptvEpgVodClientImpl::GetContentDetailsL(TIptvContentId aContentId, CIptvVodContentContentFullDetails& aContentFullDetails)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::GetContentDetailsL");  

    iSession.InitializeL();
    TInt error = iEpgDb.GetContentDetailsL(aContentId, aContentFullDetails);

    if( iSearchManager )
    	{
    	error = iSearchManager->GetContentDetailsL( aContentId, aContentFullDetails );
    	}

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvEpgVodClientImpl::GetUpdateTimeL
// -----------------------------------------------------------------------------
TInt CIptvEpgVodClientImpl::GetUpdateTimeL(TTime& aTime)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::GetUpdateTimeL");
    
    TTime interval;
    TInt error = iEpgDb.GetServiceInformationL(aTime, interval);
    return error;
    }

// -----------------------------------------------------------------------------
// CIptvEpgVodClientImpl::GetContentAccessListL
// -----------------------------------------------------------------------------
TInt CIptvEpgVodClientImpl::GetContentAccessListL(TUint32 aContentId, RPointerArray<CIptvMediaContent>& aContents)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::GetContentAccessListL");

    TInt error = iEpgDb.GetContentAccessListL(aContentId, aContents);
    
    if ( iSearchManager )
        {
        iSearchManager->GetContentAccessListL( aContentId, aContents );
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvEpgVodClientImpl::GetContentAccessL
// -----------------------------------------------------------------------------
CIptvMediaContent* CIptvEpgVodClientImpl::GetContentAccessL(TUint32 aContentId, TUint32 aIndex)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::GetContentAccessL");
    CIptvMediaContent* content = iEpgDb.GetContentAccessL( aContentId, aIndex );

    if( !content && iSearchManager )
    	{
    	content = iSearchManager->GetContentAccessL( aContentId, aIndex );
    	}

    return content;
    }

// -----------------------------------------------------------------------------
// CIptvEpgVodClientImpl::SetLastPositionL
// -----------------------------------------------------------------------------
TInt CIptvEpgVodClientImpl::SetLastPositionL(TUint32 aId, TUint32 aIndex, TUint32 aPosition)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::SetLastPositionL");
    
    
    
    return iEpgDb.SetLastPositionL(aId, aIndex, aPosition);
    }

// -----------------------------------------------------------------------------
// CIptvEpgVodClientImpl::SetLastPositionL
// -----------------------------------------------------------------------------
TInt CIptvEpgVodClientImpl::SetMpxIdL( TUint32 aContentId, TUint32 aCaIndex, TUint32  aMpxId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodClientImpl::SetMpxIdL");

    return iEpgDb.SetMpxIdL( aContentId, aCaIndex, aMpxId );
    }

TBool CIptvEpgVodClientImpl::IsVodUpdateNeededL()
    {
     return iSession.IsVodUpdateNeededL();
    }

// End of File
