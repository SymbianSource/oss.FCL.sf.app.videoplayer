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
* Description:      My Videos UI main view.*
*/


// Version : %version: 12 %

#ifndef VCXHGMYVIDEOSMAINVIEW_H
#define VCXHGMYVIDEOSMAINVIEW_H

// INCLUDE FILES
#include <coemain.h>
#include <aknappui.h>
#include <aknViewAppUi.h> 
#include <aknview.h>
#include <akntitle.h>

// FORWARD DECLARATIONS

class CVcxHgMyVideosModel;
class CVcxHgMyVideosMainViewContainer;
class CIptvResourceLoader;
class CVcxHgMyVideosCategoryListImpl;
class CVcxHgMyVideosVideoListImpl;
class TVcxHgMyVideosMemoryStatusDialog;
class CHgScroller;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

/**
 * My Videos UI main view.
 *
 * @lib vcxhgmyvideos.lib
 */
class CVcxHgMyVideosMainView : public CAknView
    {
    public: // Constructors and destructor

        /**
         * Creates main view. 
         *
         * @return New main view object.
         */
        IMPORT_C static CVcxHgMyVideosMainView* NewL();

        /**
         * Creates main view. 
         *
         * @return New main view object.
         */
        IMPORT_C static CVcxHgMyVideosMainView* NewLC();

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CVcxHgMyVideosMainView();

    public:

        /** 
         * Accessor for the application's AppUi.
         *
         * @return Pointer to AppUi.
         */
    	CAknViewAppUi* AppUi() const { return CAknView::AppUi(); }

    	/**
    	 * Returns pointer to button group container.
    	 * 
    	 * @return Button group container.
    	 */
    	CEikButtonGroupContainer* Cba();
    	
    	/**
    	 * Accessor for the view's title pane.
    	 * 
    	 * @return Pointer to title pane.
    	 */ 
    	CAknTitlePane* TitlePaneL();
    	   	
        /**
         * Accessor for category list. Creates the category list if not already
         * created. Can only leave if not created yet.
         * 
         * @return Pointer to category list.
         */
    	CVcxHgMyVideosCategoryListImpl* CategoryListL();
    	
        /**
         * Accessor for video list. Creates the video list if not already
         * created. Can only leave if not created yet.
         * 
         * @return Pointer to video list.
         */
    	CVcxHgMyVideosVideoListImpl* VideoListL();
    	
    	/**
         * Activates category list.
         */
        void ActivateCategoryListL();
        
        /**
         * Activates video list.
         * 
         * @param aIndex Index of video list to activate.
         */
        void ActivateVideoListL( TInt aIndex );    	

        /**
         * Handles commands for the view.
         *
         * @param aCommand Command ID.
         */
        void HandleCommandL( TInt aCommand );        

        /**
         * Handles key events from container.
         * 
         * @param aKeyEvent Key event.
         * @param aType     Key event type.
         * @return Response, tells if key was handled or not.
         */
        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
         * Sets correct resource to MSK. Call whenever app state changes to that
         * MSK could be affected.
         */
        void DynInitMskL();
        
        /**
         * Sets correct resource to RSK. Call whenever app state changes to that
         * RSK could be affected.
         */
        void DynInitRskL();
        
        /**
         * Updates layout for controls.
         */ 
        void UpdateLayout();
        
    private: // Constructors

        /**
         * Default C++ constructor. 
         */
        CVcxHgMyVideosMainView();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();
        
    private:

        /**
         * Returns Uid of View.
         *
         * @return TUid uid of the view
         */
        TUid Id() const;

        /**
         * Gets an object whose type is encapsulated by the specified 
         * TTypeUid object.
         * 
         * @param aId Encapsulates the Uid that identifies the type of 
         *            object required.
         * @return TTypeUid::Ptr Encapsulates the pointer to the object provided. 
         *                       Note that the encapsulated pointer may be NULL.
         */
        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
        
        /*
         * Called by the framework when the application status pane
         * size is changed.
         */
        void HandleStatusPaneSizeChange();
        
        /**
         * Activate the view.
         *
         * @param aPrevViewId      Id of previous view.
         * @param aCustomMessageId Id of custom message for view.
         * @param aCustomMessage   Custom message for view.
         */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid  aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
         * Deactivate the view (free resources).
         */
        void DoDeactivate();
        
        /**
         * Disables menu items which are not available.
         *
         * @param  aResourceId Menu resource Id.
         * @param  aMenuPane   Menu resource.
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
		
        /**
         * Handles cancel command from RSK while in marking mode.
         */
        void HandleCancelL();
                        
    private:

        /**
         * Memory status dialog.
         * Own.
         */
        TVcxHgMyVideosMemoryStatusDialog* iMemoryStatus;
        
        /**
         * Component model.
         * Own.
         */
        CVcxHgMyVideosModel* iModel;
        
        /**
         * View container.
         * Own.
         */
        CVcxHgMyVideosMainViewContainer* iContainer;
                
        /**
         * Resource loader.
         * Own.
         */
        CIptvResourceLoader* iResourceLoader;
        
        /**
         * Category list implementation.
         * Own. 
         */
        CVcxHgMyVideosCategoryListImpl* iCategoryList;

        /**
         * Video list implementation.
         * Own.
         */
        CVcxHgMyVideosVideoListImpl* iVideoList;
        
        /**
         * Background context.
         * Own.
         */
        CAknsBasicBackgroundControlContext* iBgContext;
                        
        /**
         * Resource ID of current MSK resource.
         */
        TInt iCurrentMskResource;
        
        /**
         * Resource ID of current RSK resource.
         */
        TInt iCurrentRskResource;
    };

#endif // VCXHGMYVIDEOSMAINVIEW_H
