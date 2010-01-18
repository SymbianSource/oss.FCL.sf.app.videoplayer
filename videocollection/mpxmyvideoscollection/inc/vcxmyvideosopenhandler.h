/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Handles collection Open operation related functionality*
*/



#ifndef VCXMYVIDEOSOPENHANDLER_H
#define VCXMYVIDEOSOPENHANDLER_H

class CVcxMyVideosCollectionPlugin;
class MMPXCollectionPluginObserver;
class CVcxMyVideosMdsDb;

/**
 */
NONSHARABLE_CLASS( CVcxMyVideosOpenHandler ) : public CBase
    {
    friend class CVcxMyVideosVideoCache;
    
public:
    /**
    * Two-phased constructor
    */
    static CVcxMyVideosOpenHandler* NewL( CVcxMyVideosCollectionPlugin& aCollection,
            CVcxMyVideosVideoCache& aCache,
            CVcxMyVideosMdsDb& aMds );

    /**
    * Destructor
    */
    virtual ~CVcxMyVideosOpenHandler();

public:

    /**
    * Handle collection Open operation.
    *
    * @param aPath  Path to open.
    */
    void OpenL( const CMPXCollectionPath& aPath );

    /**
    * Leaving version of HandleCreateVideoListResp.
    * Handler function for list fetching events. This callback is called as a response
    * to iCache.CreateVideoListL().
    *
    * @param aVideoList           List of videos fetched from MDS.
    * @param aNewItemsStartIndex  Start position for the new items added.
    * @param aComplete            ETrue if list is complete and no more items are expected,
    *                             EFalse if there are more to come.
    */
    void DoHandleCreateVideoListRespL(
            CMPXMedia* aVideoList, TInt aNewItemsStartIndex, TBool aComplete );
            
private:
    /**
    * 2nd-phase Constructor
    */
    void ConstructL();

    /**
    * Default Constructor
    */
    CVcxMyVideosOpenHandler( CVcxMyVideosCollectionPlugin& aCollection,
            CVcxMyVideosVideoCache& aCache,
            CVcxMyVideosMdsDb& aMds );

private: // data

    /**
    * Collection plugin main class. Owner of this object.
    */
    CVcxMyVideosCollectionPlugin& iCollection;
        
    /**
    * Videos cache, owned by iCollection. Reference is stored here
    * just to avoid one pointer access.
    */
    CVcxMyVideosVideoCache& iCache;
    
    /**
    * Object for accessing MDS database, owned by iCollection. Reference is stored here
    * just to avoid one pointer access.
    */
    CVcxMyVideosMdsDb& iMds;

    /**
    * The category ids being opened are stored here. These values are used
    * to filter list when items arrive from MDS. KVcxMvcCategoryIdAll category is not listed here.
    * This is in sync with iVideoListsBeingOpened.
    */
    RArray<TInt> iCategoryIdsBeingOpened;
    
    /**
    * Video lists for categories being opened are stored here.
    * This is in sync with iCategoryIdsBeingOpened.
    */
    RArray<CMPXMedia*> iVideoListsBeingOpened;
    };

#endif // VCXMYVIDEOSACTIVETASK_H
