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




#ifndef VCXHGVODCONTENTLISTMODELHANDLER_H
#define VCXHGVODCONTENTLISTMODELHANDLER_H

#include <ganes/HgScrollBufferObserverIface.h>

#include "vcxhgvodthumbnailobserver.h"
#include "vcxhgvodmainviewcontainer.h"
#include "vcxnscontentproviderif.h"
#include "vcxnscontentproviderobserver.h"

class CHgScroller;
class CVcxHgVodMainView;
class CVcxNsUiEngine;
class CVcxNsContent;
class CVcxHgVodContentListImpl;
class CVcxHgVodThumbnailManager;
class TVcxHgVodVideoDetailsDialog;

// CLASS DECLARATION
/**
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
class CVcxHgVodContentListModelHandler : public CBase,
                                         public MVcxNsContentProviderObserver,
                                         public MVcxHgVodThumbnailObserver,
                                         public MHgScrollBufferObserver
    {

public:

    /**
     * Two-phased constructor.
     */
    static CVcxHgVodContentListModelHandler* NewL( 
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView, 
            CHgScroller& aScroller,
            CVcxHgVodContentListImpl& aContentListImpl );
    
    /**
     * Two-phased constructor.
     */
    static CVcxHgVodContentListModelHandler* NewLC( 
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView, 
            CHgScroller& aScroller,
            CVcxHgVodContentListImpl& aContentListImpl );

    /**
    * Destructor.
    */
    virtual ~CVcxHgVodContentListModelHandler();

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
    MVcxNsContentProviderIf* ContentProvider();
    
    /**
     * Get selected video's details
     */
    void ShowVideoDetailsL( TInt aSelected );
    
    /**
     * Check if model is empty.
     */
    TBool IsEmptyModel();

public:    

    /**
     * From MVcxNsContentProviderObserver
     * Notify content's data has been updated.
     */
    void ContentUpdated( TInt aIndex );

    /**
     * From MVcxNsContentProviderObserver 
     * Application state or full view data needs to be updated.
     */
    void RefreshView();
    
    /**
     * From MVcxNsCategoryProviderObserver 
     * Application state needs to be updated.
     */
    void HandleAppStateChangedL();
       
    /**
     * MVcxNsContentProviderObserver
     * Epg update is ongoing, show the notification.
     * When downloading and parsing the xml, both parameters are 0.
     * 
     * @param aDownloaded count of downloaded thumbnails.
     * @param aTotal Total count of thumbnails to be downloaded.
     */
    void ShowUpdatingNoteL( TBool aShow, TInt aDownloadedTbns, TInt aTotalTbns );
    
    /**
     * From MVcxNsContentProviderObserver
     * Handle error 
     * 
     * @param aError Error to handle in UI
     */
    void HandleUpdateErrorL( TInt aError );
    
    /**
     * From MVcxNsContentProviderObserver
     * Handle download error 
     * 
     * @param aError Error to handle in UI
     * @param aContent Details of the video which content has failed.
     */
    void HandleDownloadErrorL( TIptvDlError aError, CVcxNsContent& aContent );

    /**
     * From MVcxHgVodThumbnailObserver
     * Notifies about completed thumbnail loading.
     *
     * @param aIcon Pointer to the created icon, ownership is transferred.
     * @param aClientId ContentId of the thumbnail.
     */  
    void MvtoThumbnailReady( CGulIcon* aIcon, TInt64 aClientId );
    
private:    
    
    /**
     * Default constructor
     *
     */
    CVcxHgVodContentListModelHandler( 
                CVcxNsUiEngine& aDataProviderIf,
                CVcxHgVodMainView& aView, 
                CHgScroller& aScroller,
                CVcxHgVodContentListImpl& aContentListImpl );

    /**
     * Constructor
     */
    void ConstructL( );


    /**
     * Generate second line text according to content current status.
     * @param aContent Content details
     * @return Content info string
     */
    HBufC* GenerateContextInfoTextLC( CVcxNsContent& aContent );

    /**
     * Update msk commands.
     */
    void HandleCommandUpdatesL();
    
    /**
     * Update data in the model
     *  
     * @param aStartIndex Index at which the item request has been initiated
     * @param aRange Number of items to update
     */
    void ProvideDataL( int aStartIndex, int aRange );
    
    /**
     * Update view details:
     * Title pane, empty list text.
     */
    void UpdateViewDetailsL();
    
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
     * Handles completed thumbnail loading. Makes sure that the 
     * aIcon is pushed to CleanupStack before any leaving code is
     * executed.
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
     * Video details dialog.
     * Own.
     */
    TVcxHgVodVideoDetailsDialog* iVideoDetails;
    
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
     * Reference to UI component responsible for displaying the list.
     */
    CHgScroller& iScroller;
    
    /**
     * Pointer to content provider if in UI engine
     * 
     */
    MVcxNsContentProviderIf* iContentProvider;
    
    /**
     * Pointer to array that holds contents in UI engine
     * Not own.
     */
    RPointerArray<CVcxNsContent>* iContents;
    
    /**
     * Ref to list implementation
     * 
     */
    CVcxHgVodContentListImpl& iListImpl;
    
    /**
     * Used to load thumbnails asynchronously.
     */
    CVcxHgVodThumbnailManager* iTnManager;
    
    };

#endif // VCXHGVODCONTENTLISTMODELHANDLER_H
