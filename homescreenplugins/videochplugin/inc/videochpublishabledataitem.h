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
* Description:    CVcxNsPublishableDataItem class declaration*
*/




#ifndef _VCXNSCHPUBLISHABLEDATAITEM_H
#define _VCXNSCHPUBLISHABLEDATAITEM_H 

// INCLUDES
#include <e32std.h>
#include <e32cmn.h>
#include "videochpublishabledata.h"


// FORWARD DECLARATIONS
class CLiwDefaultMap;
template <class T, class K> class RHashMap;


// CLASS DECLARATION
/**
 * Class contains data and services for handling data of
 * single item in the tvvideo menu suite 
 */

class CVcxNsPublishableDataItem: public CBase
    {
    
public:

    /**
     * Two-phased constructor. Provided content type
     * should not be locally defined. The object does not
     * own the actual string. 
     * 
     * @param aContentType Desc pointer to contentype of this item
     * 
     * @return  CVcxNsPublishableDataItem*
     */
    static CVcxNsPublishableDataItem* NewL( TPtrC aContentType );
    
    /**
     * Two-phased constructor. Provided content type
     * should not be locally defined. The object does not
     * own the actual string. 
     * 
     * @param aContentType Desc pointer to contentype of this item
     * 
     * @return  CVcxNsPublishableDataItem*
     */
    static CVcxNsPublishableDataItem* NewLC( TPtrC aContentType );
    
    /**
     * Destructor
     * 
     */
    virtual ~CVcxNsPublishableDataItem();


private:
    
    /**
     * Symbian 2nd phase constructor
     * 
     */
    void ConstructL();

    /**
     * C++ constructor
     * 
     */
    CVcxNsPublishableDataItem( TPtrC aContentType );
    
public:
    
    /**
     * returns a desc pointer to content type desc 
     * 
     * @return  TPtrC
     */
    TPtrC ContentType() const;
    
    /**
     * Method sets integer type data into  iIntData map
     * 
     * @param aValueKey& value's key name where to save
     * @param aValue value to save
     * 
     * @leave KErrNotFound: if item with provided content type or value data 
     *                      item with provided key not found.
     * @leave KErrNoMemory: if memory could not be allocated to store the 
     *                       copies of aKey and aValue
     */
    void SetDataL( TPtrC8 aKey, TInt aValue );
      
    /**
     * Method sets integer type data into iDescData map
     * 
     * @param aValueKey& value's key name where to save
     * @param aValue value to save
     * 
     * @leave KErrNotFound: if item with provided content type or value data 
     *                      item with provided key not found.
     * @leave KErrNoMemory: if memory could not be allocated to store the 
     *                       copies of aKey and aValue
     */
    void SetDataL( TPtrC8 aKey, const TDesC& aData );
    
    /**
     * Method returns integer -type data from local map.   
     * 
     * @param aValueKey& value's key name  from where to get
     * 
     * @return TInt 
     * 
     * @leave KErrNotFound: if item with provided content type or value data 
     *                      item with provided key not found.
     * 
     */
    TInt GetIntDataL( TPtrC8 aKey ) const;
    
    /**
     * Method returns descriptor -type data from local map.   
     * 
     * @param aValueKey& value's key name  from where to get
     * 
     * @return TInt 
     * 
     * @leave KErrNotFound: if item with provided content type or value data 
     *                      item with provided key not found.
     * 
     */
    const TDesC& GetDesCDataL( TPtrC8 aKey ) const;
    
    /**
     * Method copies key-value pairs from the local maps into 
     * provided reference map.   
     * 
     * @param aDataMap  map where to save data
     * 
     */
    void FillCPDataMapL( CLiwDefaultMap& aDataMap );
   
    /**
     * Method reads key-value pairs from provided reference map
     * and saves them to local maps.    
     * 
     * @param aDataMap  map where to copy data
     *  
     */
    void ReadCPDataMapL( CLiwDefaultMap& aDataMap );
    
    /**
     * Method clears data. All descs sizes are set to zero and integers
     * set to 0.     
     *  
     */
    void ClearData();
    
private:
    
    /**
     * Internal helper method to extract data to liwvariant from the liwmap.
     * 
     * Method leaves if data provided by the key is not found
     * 
     * @param aKey key of data item
     * @param aDataMap a map where to search data
     * @param aVariant variant item from where to fetch tha actual data
     * 
     */
    TInt ExtractVariant( const TDesC8& aKey,  
                         CLiwDefaultMap& aDataMap, 
                         TLiwVariant& aVariant );
       
        
private: // DATA
     
    /**
     * Desc pointer to external string containing content type txt.
     * Not Owned.
     */
    TPtrC iContentType;
    
    /**
     * Map containing integer type data.
     */
    RHashMap< TBuf8< KTvVideoMaxValueKeyLen >, TInt >*   iIntData;
      
    /**
     * Map containing desc type data.
     */
    RHashMap< TBuf8< KTvVideoMaxValueKeyLen >, HBufC* >* iDescData;
            
    };

#endif // _VCXNSCHPUBLISHABLEDATAITEM_H 
