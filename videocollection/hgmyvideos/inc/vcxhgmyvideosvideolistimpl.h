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
* Description:      Implementation of UI functionality for video list.*
*/




#ifndef VCXHGMYVIDEOSVIDEOLISTIMPL_H
#define VCXHGMYVIDEOSVIDEOLISTIMPL_H

// INCLUDE FILES
#include <e32cmn.h>
#include <ganes/HgSelectionObserverIface.h>
#include <ganes/HgMarkingObserverIface.h>
#include <AknProgressDialog.h>
#include <upnpcommandobserver.h>
#include "vcxhgmyvideoslistbase.h"

// FORWARD DECLARATIONS
class CVcxHgMyVideosVideoModelHandler;
class TViaPlayerCustomMessage;
class CAknWaitDialog;
class CVcxHgMyVideosVideoCopier;
class CVcxHgMyVideosUPnPInterface;
class CVcxHgTelephonyClient;
class CHgScroller;
class CSendUi;
class CHgMyVideosAiwMenuHandler;
class CDRMHelper;

namespace DRM
    {
    class CDrmUtility;
    }

// CLASS DECLARATION

/**
 * Implementation of UI functionality for video list.
 * 
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosVideoListImpl ) : public CVcxHgMyVideosListBase,
                                                   public MProgressDialogCallback,
                                                   public MUpnpCommandObserver,
                                                   public MHgSelectionObserver,
                                                   public MHgMarkingObserver
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @param aModel Component model
         * @param aView Component view
         * @param aScroller Hg list control 
    
         * @return Pointer to newly created instance
         */
        static CVcxHgMyVideosVideoListImpl* NewL(
            CVcxHgMyVideosModel& aModel,
            CVcxHgMyVideosMainView& aView,
            CHgScroller& aScroller );

        /**
         * Two-phased constructor.
         *
         * @param aModel Component model 
         * @param aView Component view
         * @param aScroller Hg list control
         *
         * @return Pointer to newly created instance
         */
        static CVcxHgMyVideosVideoListImpl* NewLC( 
             CVcxHgMyVideosModel& aModel,
             CVcxHgMyVideosMainView& aView,
             CHgScroller& aScroller );
    
        /**
        * Destructor.
        */
        virtual ~CVcxHgMyVideosVideoListImpl();
    
        /**
         * Activate.
         * 
         * @param aIndex Index of video list to activate.
         */
        void DoListActivateL( TInt aIndex );
    
        /**
         * Deactivate
         */
        void DoListDeactivate();

        /**
         * Returns reference to the video model.
         * 
         * @return Reference to video model.
         */
        CVcxHgMyVideosVideoModelHandler& VideoModel();
        
        /**
         * Returns reference to the uPnP interface
         * 
         * @return Reference to uPnP interface
         */
        CVcxHgMyVideosUPnPInterface& UPnPInterface();

        /**
         * Gets list highlight.
         * 
         * @return Highlight.
         */
        TInt Highlight();

        /**
         * Returns the index(es) of video(s) that are operation targets.
         * 
         * @param aOperationTargets On return, index(es) of the video(s).
         */
        void GetOperationTargetIndexesL( RArray<TInt>& aOperationTargets );        

        /**
         * Starts playback of currently focused video.
         */
        void PlayCurrentVideoL();
                        
        /**
         * Deletes currently selected video(s).
         */
        void DeleteVideosL();

        /**
         * Plays currently selected video.
         */
        void PlayVideoL();
        
        /**
         * Resumes currently selected download.
         */
        void ResumeDownloadL();
        
        /**
         * Cancels currently selected download.
         */
        void CancelDownloadL();        
        
        /**
         * Handles sort command to list.
         * 
         * @param aSortCommand Sort command.
         */
        void HandleSortCommandL( TInt aSortCommand );
        
        /**
         * Checks the UI and list status and returns information to show
         * correct mark-menu items.
         * 
         * @param aShowMarkSubmenu On return, ETrue if mark sub-menu can be shown.
         * @param showStartMarking On return, ETrue if start marking menu item can be shown.
         * @param showEndMarking On return, ETrue if stop marking menu item can be shown.
         * @param aShowMarkAll On return, ETrue if mark all menu item can be shown.
         * @param aShowUnmarkAll On return, ETrue if unmark all menu item can be shown.
         */
        void ShowMarkMenuItemsL( 
                 TBool& aShowMarkSubmenu, TBool& showStartMarking, TBool& showEndMarking, 
                 TBool& aShowMarkAll, TBool& aShowUnmarkAll );

        /**
         * Handles mark command to list.
         * 
         * @param aMarkCommand Mark command.
         */
        void HandleMarkCommandL( TInt aMarkCommand );
        
        /**
         * Checks the UI and list status, and returns information to show correct
         * move and copy -menu items.
         * 
         * @param aShowMoveAndCopySubmenu On return, ETrue if move/copy sub-menu cab be shown.
         * @param aShowCopy On return, ETrue if copy menu item can be shown.
         * @param aShowMove On return, ETrue if move menu item can be shown.
         */
        void ShowMoveAndCopyMenuItemsL( 
                 TBool& aShowMoveAndCopySubmenu, TBool& aShowCopy, TBool& aShowMove );
        
        /**
         * Handles move and copy commands to list.
         * 
         * @param aCopy ETrue if copy command was given. EFalse if move.
         */
        void HandleMoveOrCopyCommandL( TBool aCopy );
        
        /**
         * Set empty list text for list control. 
         */
        void SetEmptyListTextL();        

        /**
         * Set title text for view.
         * 
         * @param aIndex Index of active video list.
         */
        void SetTitleTextL( TInt aIndex );

        /**
         * Handles dynamic initialisation of menu items specific to video list.
         * 
         * @param aResourceId Resource ID of menu to be activated.
         * @param aMenuPane Pointer to menu pane.
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );        
        
        /**
         * Returns correct MSK resource Id based on current state of video list.
         *
         * @return MSK resource Id.
         */
        TInt GetMskResourceL();
        
        /**
         * Called by video model when video deletion is completed.
         * 
         * @param aFailedCount Count of items that could not be deleted.
         * @param aFailedName Name of video where deletion failed, if there was only one.
         */
        void VideoDeletionCompletedL( TInt aFailedCount, const TDesC& aFailedName );
        
        /**
         * Called by video model when move or copy command is completed.
         * 
         * @param aFailedCount Count of items that could not be moved/copied.
         * @param aFailedName Name of video where move/copy failed, if there was only one.
         */
        void VideoMoveOrCopyCompletedL( TInt aFailedCount, const TDesC& aFailedName );
        
        /**
         * Displays note about resume starting from beginning.
         */
        void ShowResumeStartsFromBeginningNoteL();
        
        /**
         * Begins multimarking mode.
         */
        void StartMarkingMode();
        
        /**
         * Ends multimarking mode.
         */
        void EndMarkingMode();
        
        /**
         * Send video using SendUI
         */
        void HandleSendL();
        
        /**
         * Handles command if aCommand is an Aiw command
         * 
         * @param aCommand Menu command ID
         */
        void TryHandleAiwCommandL( TInt aCommand );
        
        /**
         * Checks if we are at marking mode and returns
         * corresponding value.
		 *
		 * @return ETrue if in marking mode else EFalse.
         *  
         */
        TBool IsMarking();
        
    public: // From MProgressDialogCallback

        /**
         * Called when (delete) wait dialog is dismissed.
         *
         * @aButtonId Button with which dialog was dismissed.
         */     
        void DialogDismissedL( TInt aButtonId ); 
    
    public: // From MHgSelectionObserver
            
        /**
         * Handle selection.
         * 
         * @param aIndex Selected item index.
         */
        void HandleSelectL( TInt aIndex );
            
        /**
         * Handle opening.
         * 
         * @param aIndex Opened item index.
         */
        void HandleOpenL( TInt aIndex );
    
    public: // From MHgMarkingObserver
        
        /**
         * Handle marking.
         * 
         * @param aIndex Selected item index.
         * @param aMarked ETrue if item marked, EFalse if item is unmarked.
         */
        void HandleMarkingL( TInt aIndex, TBool aMarked ); 
        
    public:
        
       /**
        * Handles async reposnses from CVcxHgMyVideosuPnPInterface::ShowVideo
        * In case we receive KErrNone or value below it (error), video playback 
        * has been ended due error or succesfully. 
        * 
        * Status might also be positive, which refers some undocumented additional
        * information. According to UPnP documentation, those status values should 
        * be ignored. 
        * 
        * @param aStatusCode  statuscode from the CVcxHgMyVideosUPnPInterface
        */
       void CommandComplete( TInt aStatusCode );
        
    protected:    
        
        /**
         * Default constructor
         *
         * @param aModel Component model
         * @param aView Component view
         * @param aScroller Hg list control
         */
        CVcxHgMyVideosVideoListImpl( CVcxHgMyVideosModel& iModel,
                                     CVcxHgMyVideosMainView& aView,
                                     CHgScroller& aScroller );

        /**
         * 2nd phase constructor.
         */
        void ConstructL();

        /**
         * Confirms video deletion from user.
         * 
         * @param aTargetCount Count of deletion targets.
         * @param aTargetName Name of deletion target, if there is only one.
         * @return ETrue if user confirmed deletion.
         */
        TBool ConfirmDeleteVideosL( TInt aTargetCount, const TDesC& aTargetName );

        /**
         * Opens the deletion wait note.
         */
        void OpenDeleteWaitNoteL();

        /**
         * Closes the deletion wait note.
         */
        void CloseDeleteWaitNote();
        
        /**
         * Checks parental control for a video, and asks lock code.
         *
         * @leave KErrPermissionDenied If not allowed to play.
         * @param aAgeProfile Age profile for the video to be checked.
         */
        void CheckParentalControlL( TUint32 aAgeProfile );
        
        /**
         * Fetch SendUI messaging configuration from CenRep
         * 
         * @return Messaging configuration from CenRep
         */
        TInt MessagingConfL();

        /**
         * Returns pointer to instance of CSendUi
         * 
         * @return Pointer to CSendUi
         */
        CSendUi* SendUiL();

        /**
         * Returns pointer to instance of CHgMyVideosAiwMenuHandler.
         * First call creates handler and attaches AIW menu.
         * 
         * @return Pointer to CHgMyVideosAiwMenuHandler
         */
        CHgMyVideosAiwMenuHandler* AiwMenuHandlerL();

    protected:
    
        /**
         * Video model handler.
         * Own.
         */
        CVcxHgMyVideosVideoModelHandler* iVideoModel;    
        
        /**
         * Index of video currently in play.
         */
        TInt iCurrentlyPlayedVideo;
        
        /**
         * Delete wait dialog.
         * Own.
         */
        CAknWaitDialog* iDeleteWaitDialog;
        
        /**
         * Class for handling copying and moving of videos.
         * Own.
         */
        CVcxHgMyVideosVideoCopier* iVideoCopier; 
        
        /**
         * UPnP handler interface
         * Own.
         */
        CVcxHgMyVideosUPnPInterface* iUPnP;
        
        /**
         * Central repository access needed for parental control setting.
         * Own.
         */
        CRepository* iCenRep;
        
        /**
         * Telephony client, used for asking lock code from user.
		 * Own.
         */
        CVcxHgTelephonyClient* iTelephonyClient;
        
        /**
         * Flag for marking mode. True when marking mode enabled.
         */
        TBool iMultipleMarkingActive;

        /**
         * SendUI
         * Own
         */
        CSendUi* iSendUi;

        /** 
         * Aiw Menu Handler
         * Own
         */
        CHgMyVideosAiwMenuHandler* iAiwMenuHandler;

    };

#endif // VCXHGMYVIDEOSVIDEOLISTIMPL_H
