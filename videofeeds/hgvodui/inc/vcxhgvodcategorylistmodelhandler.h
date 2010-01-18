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
* Description:    HG VOD*
*/




#ifndef VCXHGVODCATEGORYLISTMODELHANDLER_H
#define VCXHGVODCATEGORYLISTMODELHANDLER_H

#include <ganes/HgScrollBufferObserverIface.h>

#include "vcxhgvodmainviewcontainer.h"
#include "vcxnscategoryproviderif.h"
#include "vcxnscategoryproviderobserver.h"

class CHgScroller;
class CHgItem;
class CVcxHgVodMainView;
class CVcxNsUiEngine;
class CVcxNsCategory;
class CVcxHgVodCategoryListImpl;

// CLASS DECLARATION
/**
 *  
 *  
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
class CVcxHgVodCategoryListModelHandler : public CBase,
                                          public MVcxNsCategoryProviderObserver,
                                          public MHgScrollBufferObserver
    {

public:

    /**
     * Two-phased constructor.
     */
    static CVcxHgVodCategoryListModelHandler* NewL( 
            CVcxNsUiEngine& aDataProviderIf, 
            CVcxHgVodMainView& aView, 
            CHgScroller& aScroller,
            CVcxHgVodCategoryListImpl& aCategoryListImpl );
    
    /**
     * Two-phased constructor.
     */
    static CVcxHgVodCategoryListModelHandler* NewLC( 
            CVcxNsUiEngine& aDataProviderIf, 
            CVcxHgVodMainView& aView, 
            CHgScroller& aScroller,
            CVcxHgVodCategoryListImpl& aCategoryListImpl );

    /**
    * Destructor.
    */
    virtual ~CVcxHgVodCategoryListModelHandler();

    /**
    * Activate model
    * 
    */
    void DoActivateModelL( );
    
    /**
    * Deactivate model
    * 
    */
    void DoDeActivateModel( );
    
    /**
    * Get services and set to widget model.
    * 
    */
    void SetListContentL();
    
    /**
    * Get category provider interface pointer
    * 
    */
    MVcxNsCategoryProviderIf* CategoryProvider();
    
public:    

    /**
     * From MVcxNsCategoryProviderObserver
     * Notify category's data has been updated.
     */
    void CategoryUpdated( TInt aIndex );

    /**
     * From MVcxNsCategoryProviderObserver 
     * Application state or full view data needs to be updated.
     */
    void RefreshView();

    /**
     * From MVcxNsCategoryProviderObserver 
     * Application state needs to be updated.
     */
    void HandleAppStateChangedL();

    /**
     * From MVcxNsCategoryProviderObserver
     * Epg update is ongoing, show the notification.
     * When downloading and parsing the xml, both parameters are 0.
     * 
     * @param aDownloaded count of downloaded thumbnails.
     * @param aTotal Total count of thumbnails to be downloaded.
     */
    void ShowUpdatingNoteL( TBool aShow, TInt aDownloadedTbns, TInt aTotalTbns );

    /**
     * From MVcxNsCategoryProviderObserver
     * Handle error 
     * 
     * @param aError Error to handle in UI
     */
    void HandleUpdateErrorL( TInt aError );
    
private:    
    
    /**
     * Default constructor
     *
     */
    CVcxHgVodCategoryListModelHandler( 
                CVcxNsUiEngine& aDataProviderIf, 
                CVcxHgVodMainView& aView, 
                CHgScroller& aScroller,
                CVcxHgVodCategoryListImpl& aCategoryListImpl );

    /**
     * Constructor
     *
     */
    void ConstructL( );

    /**
     * Updates data of item in list.
	 * @param aIndex Item's index
     */
   void UpdateListItemL( TInt aIndex );
   
   /**
    * Refreshes view.
    */
   void RefreshViewL();
   
   /**
    * From MHgScrollBufferObserver
    * 
    * Buffer position changed. The items between the aBufferStart 
    * and aBufferEnd should be loaded as soon as possible.
    * 
    * @param aBufferStart The index of the first item in buffer.
    * @param aBufferEnd The index of the last item in buffer.
    * @param aDirection The direction of request.
    */
   void Request( TInt aBufferStart, TInt aBufferEnd, THgScrollDirection aDirection );
       
   /**
    * From MHgScrollBufferObserver
    * 
    * Buffer position changed. The items between the aBufferStart 
    * and aBufferEnd should be released.
    * 
    * @param aBufferStart The index of the first item in buffer.
    * @param aBufferEnd The index of the last item in buffer.
    */
   void Release( TInt aBufferStart, TInt aBufferEnd );
   
private: // data
    
    /**
     * Ref to Ui Engine interface.
     * 
     */
    CVcxNsUiEngine& iDataProviderIf;
    
    /**
     * Reference to vod main view
     *
     */
    CVcxHgVodMainView& iView;
    
    /**
     * Pointer to category provide if in UI engine
     * 
     */
    MVcxNsCategoryProviderIf* iCategoryProvider;
    
    /**
     * Pointer to array that holds categories in UI engine
     * Not own.
     */
    RPointerArray<CVcxNsCategory>* iCategories;
    
    /**
     * Ref to scroller
     * 
     */
    CHgScroller& iScroller;
    
    /**
     * Ref to list implementation
     * 
     */
    CVcxHgVodCategoryListImpl& iListImpl;
    };

#endif // VCXHGVODCATEGORYLISTMODELHANDLER_H
