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
* Description:    HG VOD content view functionality implementation*
*/




#ifndef VCXHGVODCONTENTLISTIMPL_H
#define VCXHGVODCONTENTLISTIMPL_H


#include <remconcoreapi.h>
#include "vcxhgvodmainviewcontainer.h"
#include "vcxhgvodlistimplbase.h"
#include "vcxnscontent.h"

class CHgScroller;
class CVcxHgVodMainViewContainer;
class CVcxHgVodContentListModelHandler;

// CLASS DECLARATION
/**
 *  Content list implementation
 *  This class handles service view list functionality.
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
class CVcxHgVodContentListImpl : public CVcxHgVodListImplBase
    {

public:

    /**
     * Two-phased constructor.
     * @param aScroller UI component responsible of drawing the information
     * @param aDataProviderIf Reference to UI engine
     * @param aView Reference to main view
     * @return Pointer to newly created instance
     */
    static CVcxHgVodContentListImpl* NewL(
            CHgScroller& aScroller,
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView );
    
    /**
     * Two-phased constructor.
     * @param aScroller UI component responsible of drawing the information
     * @param aDataProviderIf Reference to UI engine
     * @param aView Reference to main view
     * @return Pointer to newly created instance
     */
    static CVcxHgVodContentListImpl* NewLC( 
            CHgScroller& aScroller, 
            CVcxNsUiEngine& aDataProviderIf,
            CVcxHgVodMainView& aView );

    /**
    * Destructor.
    */
    virtual ~CVcxHgVodContentListImpl();
    
    /**
     * Activate service model
     */
    void DoActivateL( );

    /**
     * Deactivate service model
     *
     */
    void DoDeactivate();

    /**
     * Handle user command
     */
    void HandleCommandL( TInt aCommand );
    
    /**
     * Activate menu pane items before displaying menu
     * 
     * @param aMenuPane Pointer to menu pane
     *
     */
    void ActivateMenuPaneItemsL( CEikMenuPane* aMenuPane, TInt aResourceId );

    /**
     * Update title pane text for content view.
     */
    void UpdateTitlePaneL();
    
    /**
     * Get content download status and content type data.
     * @param aDlStatus On return, download status data
     * @param aContentTypes On return, content types
     * 
     */
    void ContentStatusDataL( 
        CVcxNsContent::TVcxContentDlStatus &aDlStatus,
        TUint32 &aContentTypes );
    
    /**
     * Check msk
     * 
     */
    void CheckMskL();

    /**
    * Get help context.
    *
    * @param aContext context
    */
    void GetHelpContext( TCoeHelpContext& aContext ) const;
    
    /**
     * Selection (highlight) changes.
     * @param aIndex Index of selected item.
     */
    void HandleSelectL( TInt /*aIndex*/ );
    
    /**
     * Handles item opening.
     * @param aIndex Index of item to be opened.
     */
    void HandleOpenL( TInt /*aIndex*/ );

public: //from MViaPlayerMediaKeyListner

    /**
     * Called when media key event happens.
     *
     * @since S60 v3.0
     * @param aOperationId The media key operation (ie. play, rewind, etc)
     * @param aButtonAct The activity of the button (ie. pressed, released, etc)
     */
    void MediaKeyEventL( TRemConCoreApiOperationId aOperationId, 
        TRemConCoreApiButtonAction aButtonAct );
    
private:    
    
    /**
     * Default constructor
     * @param aListWidget List widget
     * @param aDataProviderIf Reference to ui engine interface
     * @param aView Main view
     */
    CVcxHgVodContentListImpl( CHgScroller& aScroller,
                              CVcxNsUiEngine& aDataProviderIf,
                              CVcxHgVodMainView& aView );

    /**
     * 
     * @param aDataProviderIf
     * @param aView Main view
     */
    void ConstructL( 
            CVcxNsUiEngine& aDataProviderIf, 
            CVcxHgVodMainView& aView  );
    
    /**
     * Start playback of given video item.
     */
    void StartPlaybackL();
    
    /**
     * Open selected list items browser link. 
     * @param aSelected Focused list item index
     */
    void OpenBrowserLinkL( TInt aSelected );
    
    /**
     * Send video content link via SMS or Email
     *
     * @param aMessageTypeModule MTM service Uid
     * @param aSelected Selected item index from the list
     */
    void ShareLinkL( const TUid aMessageTypeModule, TInt aSelected );
    
    /**
     * Check MSK.
     * @param aShowDownload
     * @param aShowPlay
     * @param aShowResume
     * 
     */
    void CheckMskL( TBool aShowDownload, 
                    TBool aShowPlay,
                    TBool aShowResume );   
    
    /**
     * Handle selected item in content list (touch).
     */
    void HandleSelectionL();

private: // data

    /**
     * Flag for email accoun availability.
     */
    TBool iEmailAccountEnabled;
        
    /**
     * List widget model handler
     * Own.
     */
    CVcxHgVodContentListModelHandler* iModelHandler;
    };

#endif // VCXHGVODCONTENTLISTIMPL_H
