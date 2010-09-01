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




#ifndef CIPTVEPGVODCLIENTIMPL_H
#define CIPTVEPGVODCLIENTIMPL_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"

// FORWARD DECLARATIONS
class CIptvEpgDb;
class CIptvVodContentCategoryBriefDetails;
class CIptvVodContentContentBriefDetails;
class CIptvVodContentContentFullDetails;
class CIptvEpgSession;
class CIptvMediaContent;
class CIptvSearchManager;

// CLASS DECLARATION
class CIptvEpgVodClientImpl : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        static CIptvEpgVodClientImpl* NewL( CIptvEpgDb& aEpgDb,
        									CIptvEpgSession& aSession,
        									CIptvSearchManager* aSearchManager );

        /**
         * Destructor.
         */
        ~CIptvEpgVodClientImpl();
                                                                                        
        /**
        * Get list of ECG categories. 
        * The ownership of the returned list is transferred to caller.
        * The order of categories is defined by database originated from service provider.
        * @since
        * @param aParentCategoryId Points to category ID which sub-categories should be listed. If NULL, the root is referenced.
        * @param aCategoryList Reference to a storage where pointer to created category list is returned.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        TInt GetEcgCategoryListL(TIptvCategoryId aParentCategoryId, RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList);

        /**
        * Get full detais of one category item.
        * The ownership of the returned data is transferred to caller.
        * @since                Engine 1.1
        * @param aCategoryId    Selected category ID identifier.
        * @param aCategory      Reference to a full details data returned.
        * @return               Error code, KErrNone if no errors.
        */
        TInt GetCategoryDetailsL(TIptvCategoryId aCategoryId, CIptvVodContentCategoryBriefDetails& aCategory);

        /**
        * Get parent category ID identifier. 
        * This method allows application to browse toward root category, then two consecutive calls are needed.
        * @since
        * @param aCategoryId Points to category ID which parent category is requested.
        * @param aParentId Reference to a storage where parent ID is returned.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        TInt GetParentCategoryL(TIptvCategoryId aCategoryId, TIptvCategoryId& aParentId);

        /**
        * Get list of ECG contents in the specified category in order defined by database originated from service provider.
        * The ownership of the returned list is transferred to caller.
        * @since
        * @param aCategoryId Selected category ID identifier
        * @param aSearchString A search string used to filter contents search. If NULL, no filtering issued.
        * @param aFrom The start index for listed contents. If NULL, start from the first one.
        * @param aAmount Amount of content elementes to be listes. If NULL, all starting from aFrom.
        * @param aTotalAmount Poiter to a variable where total amount of content is returned.
        * @param aEcgList Reference to returned ECG list items.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        TInt GetEcgListL(TIptvCategoryId aCategoryId, TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList);

        /**
        * Get list of all ECG contents in alphabetical order.
        * The ownership of the returned list is transferred to caller.
        * @since
        * @param aSearchString A search string used to filter contents search. If NULL, no filtering issued.
        * @param aFrom The start index for listed contents. If NULL, start from the first one.
        * @param aAmount Amount of content elementes to be listes. If NULL, all starting from aFrom.
        * @param aTotalAmount Poiter to a variable where total amount of content is returned.
        * @param aEcgList Reference to returned ECG list items.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        TInt GetEcgAllListL(TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList);

        /**
        * Get full detais of one content item.
        * The ownership of the returned data is transferred to caller.
        * @since
        * @param aContentId Selected content ID identifier.
        * @param aContentFullDetails Reference to returned full details data.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        TInt GetContentDetailsL(TIptvContentId aContentId, CIptvVodContentContentFullDetails& aContentFullDetails);
      
        /**
        * Get time when service was last updated.
        * @since
        * @param aTime Reference to TTime object.
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        TInt GetUpdateTimeL(TTime& aTime);

        // Engine 1.1
        TInt GetContentAccessListL(TUint32 aContentId, RPointerArray<CIptvMediaContent>& aContents);
        CIptvMediaContent* GetContentAccessL(TUint32 aContentId, TUint32 aIndex);
        TInt SetLastPositionL(TUint32 aId, TUint32 aIndex, TUint32 aPosition);
        TInt SetMpxIdL( TUint32 aContentId, TUint32 aCaIndex, TUint32 aMpxId ); 
        TBool IsVodUpdateNeededL();
    private:    // New functions           
        /**
        * C++ default constructor.
        */
        CIptvEpgVodClientImpl( CIptvEpgDb& aEpgDb,
        					   CIptvEpgSession& aSession,
        					   CIptvSearchManager* aSearchManager );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();
                                                  
    private:    // Data members 
               
        CIptvEpgDb& iEpgDb;
        
        CIptvSearchManager* iSearchManager;

        RArray<TUint32> iCategories;

        CIptvEpgSession& iSession;
    };

#endif  // CIPTVEPGVODCLIENTIMPL_H

// End of File
