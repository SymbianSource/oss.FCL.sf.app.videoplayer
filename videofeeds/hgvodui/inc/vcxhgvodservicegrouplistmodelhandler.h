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




#ifndef VCXHGVODSERVICEGROUPLISTMODELHANDLER_H_
#define VCXHGVODSERVICEGROUPLISTMODELHANDLER_H_

#include <ganes/HgScrollBufferObserverIface.h>

#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodthumbnailobserver.h"
#include "vcxnsserviceproviderobserver.h"
#include "vcxnsservice.h"

class CHgScroller;
class CVcxHgVodMainViewContainer;
class CVcxHgVodMainView;
class CVcxHgVodThumbnailManager;
class MVcxNsServiceProviderIf;
class CVcxNsUiEngine;
class CVcxNsService;
class CVcxHgVodServiceGroupListImpl;
   
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
NONSHARABLE_CLASS( CVcxHgVodServiceGroupListModelHandler ) : public CBase,
                                              public MVcxNsServiceProviderObserver,
                                              public MVcxHgVodThumbnailObserver,
                                              public MHgScrollBufferObserver                                              
    {

public:

    /**
     * Two-phased constructor.
     */
    static CVcxHgVodServiceGroupListModelHandler* NewL( 
                CVcxNsUiEngine& aDataProviderIf, 
                CVcxHgVodMainView& aView, 
                CHgScroller& aScroller,
                CVcxHgVodServiceGroupListImpl& aServiceGroupListImpl );
    
    /**
     * Two-phased constructor.
     */
    static CVcxHgVodServiceGroupListModelHandler* NewLC( 
                CVcxNsUiEngine& aDataProviderIf, 
                CVcxHgVodMainView& aView, 
                CHgScroller& aScroller,
                CVcxHgVodServiceGroupListImpl& aServiceGroupListImpl );

    /**
    * Destructor.
    */
    virtual ~CVcxHgVodServiceGroupListModelHandler();

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

public:    

    /**
     * Get service provider
     * 
     */
    MVcxNsServiceProviderIf* ServiceProvider(); 

    /**
     * Get service provider
     * @return highlighted item index
    */
    TInt GetSelected();

    /**
     * Check if there are any services in the list
     * 
     */
    TBool IsEmptyModel();
    
    /**
     * From MVcxNsServiceProviderObserver
     * 
     */
    void RefreshView();
    
    /**
     * @return Count of list items.
     */
    TInt ItemCount();

    /**
     * From MVcxHgVodThumbnailObserver
     * Notifies about completed thumbnail loading.
     *
     * @param aIcon Pointer to the created icon, ownership is transferred.
     * @param aClientId ServiceId of the thumbnail.
     */  
    void MvtoThumbnailReady( CGulIcon* aIcon, TInt64 aClientId );
    
private:    
    /**
     * Default constructor
     * @param aDataProviderIf Data provider interface.
     * @param aView Reference to view
     * @param aAlfEnv Reference to alf environment
     * @param aListWidget Reference to list widget
     * @param aServiceGroupListImpl List implementation
     */
    CVcxHgVodServiceGroupListModelHandler( 
            CVcxNsUiEngine& aDataProviderIf, 
            CVcxHgVodMainView& aView, 
            CHgScroller& aScroller,
            CVcxHgVodServiceGroupListImpl& aServiceGroupListImpl );

    /**
     * Constructor
     *
     */
    void ConstructL( );

    /**
     * From MVcxNsServiceProviderObserver
     * Notify service has been added.
     *
     * @param aIndex Added item's index
     */
    void ServiceAdded( TInt aIndex );
    
    /**
     * From MVcxNsServiceProviderObserver
     * Handles updated service data 
     * 
     */
    void ServiceUpdated( TInt aIndex );
    
    /**
     * Notify services data has been removed
     *
     * @param aIndex Removed item's index
     * @param aUpdatedServiceList Updated service list array
     */
    void ServiceRemoved( 
            TInt /*aIndex*/, 
            RPointerArray<CVcxNsService>& /*aUpdatedServiceList*/ );

    /**
     * From MVcxNsCategoryProviderObserver 
     * Application state needs to be updated.
     */
    void HandleAppStateChangedL();
    
    /**
     * From MVcxNsServiceProviderObserver
     */
    void OpenBrowserLinkL( const TDesC& aUri );
    
    /**
     * From MVcxNsServiceProviderObserver
     */
    void OpenStreamingLinkL( const TDesC& aUri );
    
    /**
     * Create the service item's second visible text
     * @param aGroupService Group service
     */
    HBufC* GenerateInfoTextLC( CVcxNsService& aGroupService );
    
    /**
     * Refreshes view.
     */
    void RefreshViewL();
    
    /**
     * Updates data of item in list.
	 * @param aIndex Item's index
	 */
    void UpdateListItemL( TInt aIndex );
    
    /**
     * Handles completed thumbnail loading. Makes sure that aIcon is
     * pushed to CleanupStack before any leaving code is executed.
     */
    void DoThumbnailReadyL( CGulIcon* aIcon, TInt64 aClientId );
    
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
    * Pointer to service provider interface in UI engine
    * Not own.
    */
    MVcxNsServiceProviderIf* iServiceProvider;
   
   /**
    * Pointer to array that holds grouped services 
    * Not own.
    */
    RPointerArray<CVcxNsService>* iGroupServices;
    
    /**
     * Reference to UI component responsible for drawing the list.
     */
    CHgScroller& iScroller;

    /**
     * Reference to vod main view
     */
    CVcxHgVodMainView& iView;
    
    /**
     * Ref to service view implementation class
     * 
     */
    CVcxHgVodServiceGroupListImpl& iListImpl;

    /**
     * Used to load thumbnails asynchronously.
     */
    CVcxHgVodThumbnailManager* iTnManager;
    
    /**
     * Saves the highlight index.
     */
    TInt iHighlight;
    
    };

#endif /*VCXHGVODSERVICEGROUPLISTMODELHANDLER_H_*/
