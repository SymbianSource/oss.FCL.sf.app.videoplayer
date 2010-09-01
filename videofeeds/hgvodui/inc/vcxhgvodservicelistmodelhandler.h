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




#ifndef VCXHGVODSERVICELISTMODELHANDLER_H
#define VCXHGVODSERVICELISTMODELHANDLER_H

#include <ganes/HgScrollBufferObserverIface.h>

#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodthumbnailobserver.h"
#include "vcxnsserviceproviderobserver.h"
#include "vcxnsupdateprogresshandler.h"
#include "vcxnsservice.h"

class CHgScroller;
class CHgItem;
class CVcxHgVodMainViewContainer;
class CVcxHgVodMainView;
class CVcxHgVodThumbnailManager;
class MVcxNsServiceProviderIf;
class CVcxNsUiEngine;
class CVcxNsService;
class MVcxNsUpdateProgressIf;
   
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
class CVcxHgVodServiceListModelHandler : public CBase,
                                         public MVcxNsServiceProviderObserver,
                                         public MVcxNsUpdateProgressObserver,
                                         public MVcxHgVodThumbnailObserver,
                                         public MHgScrollBufferObserver
    {

public:

    /**
     * Two-phased constructor.
     */
    static CVcxHgVodServiceListModelHandler* NewL( 
                CVcxNsUiEngine& aDataProviderIf, 
                CVcxHgVodMainView& aView, 
                CHgScroller& aScroller,
                CVcxHgVodServiceListImpl& aServiceListImpl );
    
    /**
     * Two-phased constructor.
     */
    static CVcxHgVodServiceListModelHandler* NewLC( 
                CVcxNsUiEngine& aDataProviderIf, 
                CVcxHgVodMainView& aView, 
                CHgScroller& aScroller,
                CVcxHgVodServiceListImpl& aServiceListImpl );

    /**
    * Destructor.
    */
    virtual ~CVcxHgVodServiceListModelHandler();

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
     * Show selected feed's details
     */
    void ShowFeedDetailsL( TInt aSelected );

public:    

    /**
     * From MVcxNsUpdateProgressObserver
     * Handle update progress change.
     * Informs which index in list needs to be updated with new progress
     * @param aIndex Index where progress needs to be updated
     * @param aProgress Progress as percentage 
     *
     */
    void HandleFeedUpdateProgressL( TInt aIndex, TInt aProgress );

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
     * Set highlighted item index
     * @param aIndex highlighted item index
     */
    void SetSelected( TInt aIndex );

    /**
     * Check if there are any services in the list
     * 
     */
    TBool IsEmptyModel();
    
    /**
     * From MVcxNsServiceProviderObserver
     */
    void RefreshView();
    
    /**
     * Marks Item
     * @param aIndex 
     */
    void MarkItem( TInt aIndex );
   
    /**
     * Unmarks Item
     * @param aIndex 
     */
    void UnmarkItem( TInt aIndex );
    
    /**
     * @return Count of list items.
     */
    TInt ItemCount();
    
    /**
     * Move the visual item from the source index to target index position.
     * @param aSourceIndex 
     * @param aTargetIndex 
     */
    void MoveItemL( TInt aSourceIndex, TInt aTargetIndex );
    
    /**
     * @return Is the last item of the list selected.
     */
    TBool LastItemSelected();

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
     * @param aServiceListImpl List implementation
     */
    CVcxHgVodServiceListModelHandler( 
            CVcxNsUiEngine& aDataProviderIf, 
            CVcxHgVodMainView& aView, 
            CHgScroller& aScroller,
            CVcxHgVodServiceListImpl& aServiceListImpl );

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
            TInt aIndex, 
            RPointerArray<CVcxNsService>& aUpdatedServiceList );

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
     * @param aUpdateStatus service update status
     * @param aUpdateTime Last update time
     */
    HBufC* GenerateServiceInfoTextLC( 
        CVcxNsService::TServiceUpdateStatus aUpdateStatus,
        TTime aUpdateTime );
    
    /**
     * Refreshes view. 
     */
    void RefreshViewL();
    
    /**
     * Updates item in list.
	 * @param aIndex Item's index
     */
    void UpdateListItemL( TInt aIndex );
    
    /**
     * Handles completed thumbnail loading. Makes sure that aIcon is
     * pushed to CleanupStack before calling any leaving code.
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
    * Pointer to array that holds services in UI engine
    * Not own.
    */
    RPointerArray<CVcxNsService>* iServices;
    
    /**
     * Reference to UI component responsible of drawing the list.
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
    CVcxHgVodServiceListImpl& iServiceListImpl;
    
    /**
    * Pointer to update progress handler. 
    * Not own.
    */
    MVcxNsUpdateProgressIf* iProgressHandler;

    /**
     * Used to load thumbnails asynchronously.
     */
    CVcxHgVodThumbnailManager* iTnManager;
    
    /**
     * Saves the highlight index.
     */
    TInt iHighlight;
    
    };

#endif // VCXHGVODSERVICELISTMODELHANDLER_H
