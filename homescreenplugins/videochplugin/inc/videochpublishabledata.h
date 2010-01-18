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
* Description:    CVcxNsChPublishableData class declaration*
*/




#ifndef VCXNSCHPUBLISHABLEDATA_H
#define VCXNSCHPUBLISHABLEDATA_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <bautils.h>
#include "videocontentharvesterplugin.hrh"


class CIptvResourceLoader;
class CLiwDefaultMap;
class TLiwVariant;
class CVcxNsPublishableDataItem;
class CIptvLastWatchedApi;
template <class T, class K> class RHashMap;


const TInt KTvVideoMaxValueKeyLen = 24;

_LIT8( KTvVideoCount,           "tvvideocountvalue" );
_LIT8( KTvVideoName,            "tvvideonamevalue" );
_LIT8( KTvVideoIconPath,        "tvvideoiconpath" );
_LIT8( KTvVideoPath,            "tvvideopath" );
_LIT8( KTvVideoId,              "tvvideoidvalue" );
_LIT8( KTvVideoLastWatchedType, "tvvideolastwtype" );
_LIT8( KTvVideoHasNewVideos,    "tvvideohasnewvideos" );


// CLASS DECLARATION

/**
 * Class for maintaining publishable data for matrix menu's
 * tvvideo -suite 
 *
 *  @lib vcxnscontentharvesterplugin.dll
 */
class CVcxNsChPublishableData : public CBase
    {
public: // Constructors and destructor

    /**
     * Destructor.
     */
    ~CVcxNsChPublishableData();

    /**
     * Two-phased constructor.
     */
    static CVcxNsChPublishableData* NewL();

    /**
     * Two-phased constructor.
     */
    static CVcxNsChPublishableData* NewLC();

private: 
    
    /**
     * Symbian 2nd phase constructor
     * 
     */
    void ConstructL();

    /**
     * Constructor for performing 1st stage construction
     */
    CVcxNsChPublishableData();

    
public: // new methods
    
    /**
     * Method saves publishable integer -type data into local map.   
     * 
     * @param TVCxNsCPContentType content type to where to save
     * @param aValueKey& value's key name where to save
     * @param aValue value to save
     * 
     * @leave KErrNotFound: if item with provided content type or value data 
     *                      item with provided key not found.
     * @leave KErrNoMemory: if memory could not be allocated to store the 
     *                       copies of aKey and aValue
     * 
     */
    void SetDataL( TVCxNsCPContentType aContentType, 
                   TPtrC8 aValueKey,  
                   TInt aValue );
    
    /**
     * Method saves publishable descriptor -type data into local map.   
     * 
     * @param TVCxNsCPContentType content type to where to save
     * @param aValueKey& value's key name where to save
     * @param aValue value to save
     * 
     * @leave KErrNotFound: if item with provided content type or value data 
     *                      item with provided key not found.
     * @leave KErrNoMemory: if memory could not be allocated to store the 
     *                       copies of aKey and aValue
     * 
     */
    void SetDataL( TVCxNsCPContentType aContentType, 
                   TPtrC8 aValueKey,  
                   const TDesC& aValue );
    
    /**
     * Method returns integer -type data from local map.   
     * 
     * @param TVCxNsCPContentType content type from where to get
     * @param aValueKey& value's key name  from where to get
     * 
     * @return TInt 
     * 
     * @leave KErrNotFound: if item with provided content type or value data 
     *                      item with provided key not found.
     * 
     */
    TInt GetIntDataL( TVCxNsCPContentType aContentType, 
                      TPtrC8 aValueKey );
    
    /**
     * Method returns descriptor -type data from local map.   
     * 
     * @param TVCxNsCPContentType content type from where to get
     * @param aValueKey& value's key name  from where to get
     * 
     * @return TInt 
     * 
     * @leave KErrNotFound: if item with provided content type or value data 
     *                      item with provided key not found.
     * 
     */
    const TDesC& GetDesCDataL( TVCxNsCPContentType aContentType, 
                               TPtrC8 aValueKey );
    
    /**
     * Method returns setted contenty type name from provided 
     * content type -item. 
     * 
     * @param TVCxNsCPContentType content type from where to get
     * 
     * @return TPtrC 
     * 
     * @leave KErrNotFound: if item with provided content type or value data 
     *                      item with provided key not found.
     * 
     */
    TPtrC ItemContentTypeNameL( TVCxNsCPContentType aContentType );
    
    /**
     * Method is used to create a CP publishable data for the tvvideosuite
     * for the Matrix -menu.
     * Caller must provide CLiwDefaultMap used and tell the content type 
     * it want's the data to be created to. Possible content types are:
     * - EVCxNsCPContentLastWatched: creating content for "last watched" item
     * --- map item with key "tvvideonamevalue" contains text containing iLastWatchedName
     * --- map item with key "tvvideoiconpath" contains iLastWatchedIconPath 
     * --- map item with key "tvvideolastwType" contains iLastWSourceType as textual format 
     *  
     *  - EVCxNsCPContentLiveTV: creating content for "Live TV" item
     * --- map item with key "tvvideonamevalue" contains text containing iLiveTvServiceProvName
     * --- map item with key "tvvideoiconpath" contains iLiveTvServiveProvIconPath
     * 
     * - EVCxNsCPContentIPTV: creating content for "Internet TV" item
     * --- map item with key "tvvideocountvalue" contains value of iIPTVServiceCount
     * 
     * - EVCxNsCPContentMyVideos: creating content for "My videos" item
     * --- map item with key "tvvideonamevalue" contains text from iLastNewVideoName  
     * --- map item with key "tvvideocountvalue" contains value from iNewVideoCount
     * 
     * - EVCxNsCPContentFeeds: creating content for "Video feeds" item
     *  --- map item with key "tvvideocountvalue" contains value from  iFeedCount
     *   
     * If caller tries to create any other content types, method leaves with KErrNotFound.
     * 
     * @param TVCxNsCPContentType content type to where to create content
     * @param CLiwDefaultMap& reference to map to where content is save to
     * 
     */
    void FillCPDataMapL( TVCxNsCPContentType aContentType, CLiwDefaultMap& aDataMap );
    
    
    /**
     * Method is used to extract data from given data map gotten 
     * from CP database
     * 
     * 
     * @param TVCxNsCPContentType content type to extract
     * @param CLiwDefaultMap& reference to map from where content is extracted
     * 
     */
    void ReadCPDataMapL( TVCxNsCPContentType aContentType, 
                         CLiwDefaultMap& aDataMap );
    
    /**
     * Method is used to clear data related to content type 
     * from CP database
     * 
     * 
     * @param TVCxNsCPContentType content type of data to clear
     * 
     */
    void ClearData( TVCxNsCPContentType aContentType );
    
    /**
     * Method checks if physical file with provided path exists
     * 
     * 
     * @param aFilePath file path to check
     * 
     * @return TBool ETrue if file exists
     */
    TBool FileExists( const TDesC& aFilePath );
 
    /**
     * Method returns iLastWatched
     * 
     * @return CIptvLastWatchedApi&
     */
    CIptvLastWatchedApi& LastWatchedApi();
    
private:
    
    /**
     * Helper method to check wether item with provided 
     * content type exists and if it does, returns a pointer to it
     * 
     * 
     * @param TVCxNsCPContentType content type of data check and get
     * 
     * @return CVcxNsPublishableDataItem*
     * 
     * @leave KErrNotFound if item does not exist
     * 
     */
    CVcxNsPublishableDataItem* GetItemL( TVCxNsCPContentType aContentType );
     
private: // DATA
    
    
    /**
     * Map containing all content related data, owned
     */
    RHashMap< TInt, CVcxNsPublishableDataItem* >* iData;
        
    /**
     * File server for checking file existence, owned
     */
    RFs iFs;
    
    /**
     * IPTV last watched api. owned.
     */
    CIptvLastWatchedApi* iLastWatched;

    };

#endif // VCXNSCHPUBLISHABLEDATA_H
