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




#ifndef MIPTVVODCONTENT_H
#define MIPTVVODCONTENT_H

// INCLUDES
#include "CIptvUtil.h"

// FORWARD DECLARATIONS
class CIptvVodContentCategoryBriefDetails;
class CIptvVodContentContentBriefDetails;
class CIptvVodContentContentFullDetails;
class CIptvMediaContent;

// CLASS DECLARATION

class MIptvVodContent
    {
    public:

        /**
        * Get list of ECG categories. 
        * The order of categories is defined by database originated from service provider.
        * @since
        * @param aParentCategoryId Points to category ID which sub-categories should be listed. Use KIptvVodContentCategoryRootId if the root is referenced.
        * @param aCategoryList Reference to a storage where pointer to created category list is returned.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        virtual TInt GetEcgCategoryListL(TIptvCategoryId aParentCategoryId, RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList) = 0;

        /**
        * Get full detais of one category item.
        * @since                Engine 1.1
        * @param aCategoryId    Selected category ID identifier.
        * @param aCategory      Reference to a full details data returned.
        * @return               Error code, KErrNone if no errors.        
        */
        virtual TInt GetCategoryDetailsL(TIptvCategoryId aCategoryId, CIptvVodContentCategoryBriefDetails& aCategory) = 0;

        /**
        * Get parent category ID identifier. 
        * This method allows application to browse toward root category, then two consecutive calls are needed.
        * Note that root is KIptvVodContentCategoryRootId.
        * @since
        * @param aCategoryId Points to category ID which parent category is requested.
        * @param aParentId Reference to a storage where parent ID is returned.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        virtual TInt GetParentCategoryL(TIptvCategoryId aCategoryId, TIptvCategoryId& aParentId) = 0;

        /**
        * Get list of ECG contents in the specified category in order defined by database originated from service provider.
        * @since
        * @param aCategoryId Selected category ID identifier. Use KIptvVodContentCategoryRootId if the root is referenced.
        * @param aSearchString A search string used to filter contents search. If empty string, no filtering issued.
        * @param aFrom The start index for listed contents.
        * @param aAmount Amount of content elementes to be listes. If zero, all starting from aFrom.
        * @param aTotalAmount Poiter to a variable where total amount of content is returned.
        * @param aEcgList Reference to returned ECG list items.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        virtual TInt GetEcgListL(TIptvCategoryId aCategoryId, TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList) = 0;
        
        /**
        * Get list of all ECG contents in alphabetical order.
        * @since
        * @param aSearchString A search string used to filter contents search. If empty string, no filtering issued.
        * @param aFrom The start index for listed contents.
        * @param aAmount Amount of content elementes to be listes. If zero, all starting from aFrom.
        * @param aTotalAmount Poiter to a variable where total amount of content is returned.
        * @param aEcgList Reference to returned ECG list items.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        virtual TInt GetEcgAllListL(TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList) = 0;

        /**
        * Get full detais of one content item.
        * @since
        * @param aContentId Selected content ID identifier.
        * @param aContentFullDetails Reference to returned full details data.
        * @return Error code, KErrNone if no errors, 
        * KErrCouldNotConnect if server is needs permission for connection, otherwise one of the system wide error codes.
        */
        virtual TInt GetContentDetailsL(TIptvContentId aContentId, CIptvVodContentContentFullDetails& aContentFullDetails) = 0;

        /**
        * Set IAP for service.
        * @since
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        virtual TInt SetIapL(const TUint32 aIap) = 0;

        /**
        * Update Ecg.
        * @since
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        virtual TInt UpdateEcgL() = 0;

        /**
        * Get time when service was last updated.
        * @since
        * @param aTime Reference to TTime object.
        * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        virtual TInt GetUpdateTimeL(TTime& aTime) = 0;

        /** 
        * Get content access objects belonging to given content.
        * @since    Engine 1.1
        * @param    aContentId  Content id
        * @param    aCAList     Content access list.
        * @return   Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        virtual TInt GetContentAccessListL(TUint32 aContentId, RPointerArray<CIptvMediaContent>& aCAList) = 0;

        /** 
        * Set last play position to content access.
        * @since    Engine 1.1
        * @param    aContentId  Content id
        * @param    aIndex      Content access index (id).
        * @param    aPosition   Position.
        * @return   Error code, KErrNone if no errors, otherwise one of the system wide error codes.
        */
        virtual TInt SetLastPositionL(TUint32 aContentId, TUint32 aIndex, TUint32 aPosition) = 0;

        /** 
        * Set Mpx media id
        * @param    aContentId Content id
        * @param    aCaIndex   Content access index (id).
        * @param    aMpxId     Mpx id.
        * @return              Error code.
        */
        virtual TInt SetMpxIdL( TUint32 aContentId,
                                TUint32 aCaIndex,
                                TUint32 aMpxId ) = 0;
    };

#endif  // MIPTVVODCONTENT_H

// End of File
