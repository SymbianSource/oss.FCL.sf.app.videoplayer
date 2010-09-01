/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for handling groups of IAPs.*
*/





#ifndef __CIPTVIAPLIST_H__
#define __CIPTVIAPLIST_H__

// INCLUDE FILES
#include <s32mem.h>
#include "TIptvIap.h"
#include "CIptvUtil.h"
#include <e32base.h>

// CLASS DECLARATION
/**
* Class for handling groups of IAPs.
* @lib IptvUtil.lib
*/
class CIptvIapList: public CBase
    {
    public:
 
        /**
        * Constructor.
        */
        IMPORT_C static CIptvIapList* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C ~CIptvIapList();
            
        /**
         * Adds Internet access point.\n
         * Maximum of KIptvSmServicesDbMaxIaps access points are allowed,\n
         * function call fails if tried to add more.\n
         *
         * @param  aIap IAP to add.
         * @return      System wide error code.
         */
        IMPORT_C TInt AddIap(TIptvIap& aIap);

        /**
         * Adds Internet access point.\n
         * If you add more than KIptvSmServicesDbMaxIaps IAPs with this method,
         * don't send them to Service Manager.
         *
         * @param aIap       IAP to add.
         * @param aIgnoreMax If ETrue, ignores KIptvSmServicesDbMaxIaps limit.
         * @return           System wide error code.
         */
        IMPORT_C TInt AddIap(TIptvIap& aIap, TBool aIgnoreMax);
        
        /**
         * Deletes Internet access point.\n
         *
         * @param aIndex Index of the IAP to delete.
         * @return       KErrNone on success, KErrNotFound if not found.
         */
        IMPORT_C TInt DeleteIap(TUint8 aIndex);

        /**
         * Deletes Internet access point.\n
         *
         * @param aIap IAP to delete, iId member is used to identify IAPs.
         * @return     KErrNone on success, KErrNotFound if not found.
         */
        IMPORT_C TInt DeleteIap(TIptvIap& aIap);

        /**
         * Modifies Internet access point.
         *
         * @param aIndex  Index of the access point to modify.
         * @param aIap    New access point.
         * @return        System wide error code.
         */		
        IMPORT_C TInt ModifyIap(TUint8 aIndex, TIptvIap& aIap);
        
        /**
         * Returns Internet access point. Leaves with KErrOverFlow
         * if index is out of range.
         *
         * @param aIndex Index of the IAP on the list.
         * @return       Reference to TIptvIap.
         */
        IMPORT_C TIptvIap& IapL( TUint8 aIndex );

        /**
         * Gets IAP from aIndex position. 
         *
         * @param aIndex Index of the IAP on the list.
         * @param aIap   IAP values are written here.
         * @return Returns KErrOverFlow if aIndex is out of bounds.
         */
        IMPORT_C TInt GetIap( TUint8 aIndex, TIptvIap& aIap );
        
        /**
         * Gets Internet access point count.
         *
         * @return Internet access point count.
         */
        IMPORT_C TUint8 Count();
        
        /**
         * Externalizes this object to aStream.
         *
         * @param aStream
         */
        IMPORT_C void ExternalizeL(RDesWriteStream& aStream) const;

        /**
         * Internalizes this object from aStream.
         *
         * @param aStream
         */
        IMPORT_C void InternalizeL(RDesReadStream& aStream);

        /**
         * Counts the memory block needed for the externalize call.
         *
         * @return Memory size (in bytes) needed for the externalize.
         */        
        IMPORT_C TUint32 CountExternalizeSize();

        /**
         * Set this object from aIapList (Copies data).
         *
         * @param aIapList
         */                
        IMPORT_C void SetL(CIptvIapList& aIapList);
        
        /**
         * Sorts the list, the highest priority to index 0, 2nd highest to 1, and so on.
         */
        IMPORT_C void SortByPriorityL();
        
        /**
         * Finds the first occurance of IAP which has aIapId as ID.
         *
         * @param aIapId IAP ID in CommsDB.
         * @param aIndex Index of the found IAP
         * @return       System-wide error code.
         */
        IMPORT_C TInt FindIap(TUint32 aIapId, TUint8& aIndex);

    private:
    
        /**
         * Does the leaving part of the construction.
         */
        void ConstructL();

        /**
         * Gets index which has the highest priority IAP in it.
         *
         * @return List index
         */                
        TUint8 GetHighestPriorityIapIndexL();

        /*
        * Constructor.
        */
        CIptvIapList();
        
    private:
    
        /*
        * Array of IAPs.
        */
        RArray<TIptvIap> iIapList;
    };

#endif // __CIPTVIAPLIST_H__