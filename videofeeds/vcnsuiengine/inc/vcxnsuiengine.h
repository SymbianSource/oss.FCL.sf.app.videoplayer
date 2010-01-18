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
* Description: 
*
*/




#ifndef C_VCXNSUIENGINE_H
#define C_VCXNSUIENGINE_H

//  INCLUDES
#include <e32base.h>

#include "iptvlastwatcheddata.h"

// FORWARD DECLARATIONS
class CVcxNsServiceProvider;
class CVcxNsContentProvider;
class CVcxNsCategoryProvider;
class MVcxNsServiceProviderIf;
class MVcxNsContentProviderIf;
class MVcxNsCategoryProviderIf;
class CVcxNsServiceClientHandler;
class CVcxNsContentClientHandler;
class CVcxNsMpxCollectionClientHandler;
class CVcxNsEventRouter;
class CVcxNsContentSearchHandler;
class MVcxNsContentSearchIf;
class CVcxNsUpdateProgressHandler;
class MVcxNsUpdateProgressIf;
class CVcxConnectionUtility;
class CVcxNsStreamingHandler;
class CIptvLastWatchedApi;
class CMediatorEventProvider;
class CVcxNsContent;

// ENUMS

/**
*  Video Center application states
*/
typedef enum
    {
    EStateNoState = 0,
    EStateServiceView,
    EStateCategoryView,
    EStateContentView,
    EStateCustomView,
    EStateSetting,
    EStateMyVideos,
    EStateBrowser,
    EStatePlayback,
    EStateOther
    } TVcxNsAppState;

// CLASS DECLARATION

/**
*  Video Center Ui Engine class
*
*  @lib vcxnsuiengine.lib
*/
class CVcxNsUiEngine : public CBase
	{

public:

    /**
    * Get instance of VcxNs ui engine.
    *
    * Returns a pointer to existing, or else creates a new instance.
    * NOTE: this increases automatically the reference count, so this should
    * not be called multiple times without cleaning the references.
    *
    * @return Pointer to CVcxNsUiEngine.
    */
    IMPORT_C static CVcxNsUiEngine* InstanceL();

    /**
    * Remove the reference to this ui engine instance.
    *
    * Decreases the reference count by one, this should
    * be called when not needing ui engine anymore. When
    * all references are removed, the ui engine is destructed.
    */
    IMPORT_C void DecreaseReferenceCount();

// New functions

    /**
    * Get pointer to app service provider object. Ownership not transferred.
    *
    * @return Pointer to CVcxNsServiceProvider.
    */
    IMPORT_C MVcxNsServiceProviderIf* GetServiceProviderL();

    /**
    * Get pointer to app content provider object. Ownership not transferred.
    *
    * @return Pointer to CVcxNsContentProvider.
    */
    IMPORT_C MVcxNsContentProviderIf* GetContentProviderL();

    /**
    * Get pointer to app category provider object. Ownership not transferred.
    *
    * @return Pointer to CVcxNsCategoryProvider.
    */
    IMPORT_C MVcxNsCategoryProviderIf* GetCategoryProviderL();
    
    /**
    * Get pointer to app content search handler object. 
    * Ownership not transferred.
    *
    * @return Pointer to content search interface.
    */
    IMPORT_C MVcxNsContentSearchIf* ContentSearchHandlerL();
    
    /**
    * Get pointer to content update progress handler 
    * Ownership not transferred.
    *
    * @return Pointer to update progress interface.
    */
    IMPORT_C MVcxNsUpdateProgressIf* ProgressHandlerL();

    /**
    * Get the videocenter application state.
    * 
    * @return Current videocenter application state
    */
    IMPORT_C TVcxNsAppState VcAppState();

    /**
    * Set the videocenter application state.
    * 
    * @param aAppState New videocenter application state
    */
    IMPORT_C void SetVcAppState( TVcxNsAppState aAppState );

    /**
    * Handle back command. Activates previous application state.
    *
    * @return Was the back event consumed. If EFalse,
    *         the back event should be handled by caller.
    */
    IMPORT_C TBool HandleBack();
    
    /**
    * Handle clieant app exit command. Clears application states.
    */
    IMPORT_C void ResetStates();
    
    /**
     * Returns active Category id
     * @return Id of the category
     */
    IMPORT_C TUint32 ActiveCategory();
    
    /**
     * Get iap for connection
     * @return IAP. 
     */
    IMPORT_C TUint32 GetAvailableIapL();
    
    /**
     * Get WAP id for the connection.
     * @return WAP id matching the given IAP id.
     */
    IMPORT_C TUint32 GetAvailableWapIdL();

    /**
     * Starts to play the given content 
     * 
     * NOTE: this Works only with MPX video player
     * 
     * @param aContent Content to be played. 
     */
    IMPORT_C void PlayVideoL( CVcxNsContent* aContent );

    /**
     * Starts to play the given content 
     * 
     * NOTE: this Works only with MPX video player
     * 
     * @param aContent Content to be played. Ownership not passed.
     */
    IMPORT_C void PlayStreamL( CVcxNsContent* aContent );
    
    /**
     * Starts to play the last watched
     * 
     * NOTE: this Works only with MPX video player
     */  
    IMPORT_C void PlayLastWatchedL();
    
    /**
     * Should be called when playback of video is starting. Updates 'Last
     * Watched' information to Matrix and dat-file.
     * 
     * @param aContent Content item representing played video.
    */
    IMPORT_C void VideoPlaybackStartingL( CVcxNsContent* aContent );
    
    /**
     * Removes the topmost state from application state stack.
     * Needed for vod view changes, where back navigation route changes.
     * This is intended just for UI engine's internal use
     */
    void PopAppStateStack();

    /**
    * Get service provider ptr, ownership not transferred.
    * for ui engine internal use, pointer can be NULL
    *
    * @return Pointer to CVcxNsServiceProvider
    */
    CVcxNsServiceProvider* ServiceProvider();

    /**
    * Get content provider ptr, ownership not transferred.
    * for ui engine internal use, pointer can be NULL
    *
    * @return Pointer to CVcxNsContentProvider
    */
    CVcxNsContentProvider* ContentProvider();

    /**
    * Get category provider ptr, ownership not transferred.
    * for ui engine internal use, pointer can be NULL
    *
    * @return Pointer to CVcxNsContentProvider
    */
    CVcxNsCategoryProvider* CategoryProvider();

    /**
    * Get pointer to service client handler class
    *
    * @return Pointer to CVcxNsServiceClientHandler.
    * ownership not transferred.
    */
    CVcxNsServiceClientHandler* GetServiceClientHandlerL();

    /**
    * Get pointer to content client handler class
    *
    * @return Pointer to CVcxNsContentClientHandler.
    * ownership not transferred.
    */
    CVcxNsContentClientHandler* GetContentClientHandlerL();

    /**
    * Get pointer to Mpx collection client handle
    *
    * @return Pointer to CVcxNsContentClientHandler.
    * ownership not transferred.
    */
    CVcxNsMpxCollectionClientHandler* GetMpxCollectionClientHandlerL();
    
    /**
    * Get pointer to search handler.
    *
    * @return Pointer to CVcxNsContentSearchHandler.
    * ownership not transferred.
    */
    CVcxNsContentSearchHandler* GetContentSearchHandlerL();
    
    /**
    * Get pointer to event handler class
    *
    * @return Pointer to CVcxNsEventRouter.
    * ownership not transferred.
    */
    CVcxNsEventRouter* GetEventRouter();
    
    /**
    * Get pointer to progress update handler
    *
    * @return Pointer to progress handler.
    * Ownership not transferred.
    */
    CVcxNsUpdateProgressHandler* GetProgressHandler();
    
    /**
     * Returns active Service id
     * 
     * @return Active service's id
     */
    TUint32 ActiveService();

    /**
     * Set active Service id
     * 
     * @param aServiceId Active service's id
     */
    void SetActiveService( TUint32 aServiceId );

    /**
     * Returns active Service group
     * 
     * @return Active service group id
     */
    TUint32 ActiveServiceGroup();

    /**
     * Set active Service group id
     * 
     * @param aServiceId Active service group id
     */
    void SetActiveServiceGroup( TUint32 aServiceId );

    /**
     * Set active Category id
     * @param aCategoryId Active gategory's id
     */
    void SetActiveCategory( TUint32 aCategoryId );

// from MvcxNsPlayerObserver

    /**
     * MnpwoLastPlaybackPosition
     * @param aContentUri Content Uri
     * @return Play position
     */
    TInt MnpwoLastPlaybackPosition( const TDesC& aContentUri );

    /**
     * MnpwoSetLastPlaybackPosition
     * @param aContentUri Content Uri
     * @param aPos Play position
     */
    void MnpwoSetLastPlaybackPosition( const TDesC& aContentUri, TInt aPos );
    
private:

    /**
    * Deletes the ui engine instance.
    * @param aThis class pointer
    */
    static void DeleteInstance( CVcxNsUiEngine* aThis );

    /**
    * Increases the reference count to this ui engine instance.
    */
    void IncreaseReferenceCount();

    CVcxNsUiEngine();

    void ConstructL();

    virtual ~CVcxNsUiEngine();
    
    void RaisePlayEventL( CVcxNsContent* aContent );
    
    void WriteLastWatchedL( CVcxNsContent* aContent );
    
    /**
     * @return The content. Ownership is transferred to caller.
     */
    CVcxNsContent* LastWatchedToContentL();

private:

// Data

    /**
    * Service provider
    */
    CVcxNsServiceProvider* iServiceProvider;

    /**
    * Content provider
    */
    CVcxNsContentProvider* iContentProvider;

    /**
    * Category provider
    */
    CVcxNsCategoryProvider* iCategoryProvider;

    /**
    * Count of references to this ui engine object.
    */
    TInt iReferenceCount;

    /**
     * Service client handler
     */
    CVcxNsServiceClientHandler* iServiceClientHandler;

    /**
     * Content client handler
     */
    CVcxNsContentClientHandler* iContentClientHandler;

    /**
     * Mpx collection client handler
     */
    CVcxNsMpxCollectionClientHandler* iMpxCollectionClientHandler;

    /**
     * Application state
     */
    TVcxNsAppState iApplicationState;

    /**
     * Event router
     */
    CVcxNsEventRouter* iEventRouter;

    /**
     * Application state history, needed for handling back commands.
     */
    RArray<TVcxNsAppState> iAppStateStack;

    /**
     * Active Service id
     */
    TUint32 iActiveService;

    /**
     * Active Service Group id
     */
    TUint32 iActiveServiceGroup;
    
    /**
     * Active Category id
     */
    TUint32 iActiveCategory;
	
    /**
     *  Content search handler.
     *  Own.
     */
	CVcxNsContentSearchHandler* iContentSearchHandler;
	
    /**
     * Connection utility
     */
    CVcxConnectionUtility* iConnUtil;
    
    /**
     * Content update progress handler.
     * Own.
     */
    CVcxNsUpdateProgressHandler* iProgressHandler;
	
    /**
     * Pointer to 'Last Watched' API.
     * Own.
     */
    CIptvLastWatchedApi* iLastWatchedApi;
    
    /**
     * Pointer to Mediator Event class.
     * Own.
     */        
    CMediatorEventProvider* iMediatorEventProvider;
    
    /**
     * Pointer to Streaming Handler.
     * Own.
     */
    CVcxNsStreamingHandler* iStreamingHandler;
    
	};

#endif // C_VCXNSUIENGINE_H
