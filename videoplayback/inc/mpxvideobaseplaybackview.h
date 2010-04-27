/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  MPX Video base playback view
*
*/


// Version : %version: 21 %


// This file defines the API for VideoBasePlaybackView.dll

#ifndef __VIDEOBASEPLAYBACKVIEW_H__
#define __VIDEOBASEPLAYBACKVIEW_H__

//  Include Files
#include <e32base.h>    // CBase
#include <e32std.h>     // TBuf
#include <aknview.h>
#include <mpxplaybackobserver.h>
#include <mpxviewactivationobserver.h>
#include <mpxcollectionobserver.h>

#include "mpxvideoplaybackcontrol.hrh"
#include "mpxvideoplaybackdisplayhandler.h"
#include <mpxvideoplaybackdefs.h>
#include <AknWsEventObserver.h>

//  Constants

//  Forward Declarations
class CMPXVideoPlaybackContainer;
class MMPXPlaybackUtility;
class MMPXViewUtility;
class MMPXCollectionUtility;
class CMPFileDetailsDialog;
class CMPFileDetails;

class CMPXVideoPlaybackViewFileDetails;

//  Class Definitions

class CMPXVideoBasePlaybackView : public CAknView,
                                  public MMPXPlaybackObserver,
                                  public MMPXViewActivationObserver,
                                  public MMPXPlaybackCallback,
                                  public MMPXCollectionObserver,
                                  public MAknWsEventObserver
{
    public:
        ~CMPXVideoBasePlaybackView();

        /**
        * From CAknView
        * Command handling function.
        *
        * @param aCommand Command which is handled
        */
        virtual void HandleCommandL( TInt aCommand );

        void RetrieveFileNameAndModeL( CMPXCommand* aCmd );

        void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );

    protected:

        /**
        * From CAknView
        * Processes user commands.
        *
        * @param aCommand ID of the command to respond to.
        */
        virtual void ProcessCommandL( TInt aCommand );

        /**
        * From CAknView
        * Event handler for status pane size changes.
        *
        */
        virtual void HandleStatusPaneSizeChange();

        /**
        * From CAknView
        * Handles a view activation.
        *
        * @param aPrevViewId Specifies the view previously active.
        * @param aCustomMessageId Specifies the message type.
        * @param aCustomMessage The activation message.
        */
        virtual void DoActivateL( const TVwsViewId& aPrevViewId,
                                  TUid aCustomMessageId,
                                  const TDesC8& aCustomMessage );

        /**
        * From CAknView
        * View deactivation function.
        */
        virtual void DoDeactivate();

        /**
        * From CAknView
        * Foreground event handling function.
        *
        * @param aForeground Indicates the required focus state of the control.
        */
        virtual void HandleForegroundEventL( TBool aForeground );

        /**
        * From MEikMenuObserver
        */
        virtual void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From MMPXPlaybackObserver
        * Handle playback message
        *
        * @param aMessage Playback Message
        * @param aErr system error code.
        */
        virtual void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

        /*
         *  From MMPXViewActivationObserver
         *  Handle view activation.
         *
         *  @param aCurrentViewType Current view type Uid.
         *  @param aPreviousViewType Previous view type Uid.
         */
        void HandleViewActivation( const TUid& aCurrentViewType, const TUid& aPreviousViewType );

        /**
        *  From MMPXPlaybackCallback
        *  Handle playback property
        *
        *  @param aProperty the property
        *  @param aValue the value of the property
        *  @param aError error code
        */
        void HandlePropertyL( TMPXPlaybackProperty aProperty,
                              TInt aValue,
                              TInt aError );

        /**
        *  Method is called continously until aComplete=ETrue, signifying that
        *  it is done and there will be no more callbacks
        *  Only new items are passed each time
        *
        *  @param aPlayer UID of the subplayer
        *  @param aSubPlayers a list of sub players
        *  @param aComplete ETrue no more sub players. EFalse more subplayer
        *                   expected
        *  @param aError error code
        */
        inline void HandleSubPlayerNamesL( TUid /*aPlayer*/,
                                           const MDesCArray* /*aSubPlayers*/,
                                           TBool /*aComplete*/,
                                           TInt /*aError*/ )
        {}

        /**
        *  Call back of media request
        *
        *  @param aMedia media
        *  @param aError error code
        */
        void HandleMediaL( const CMPXMedia& aProperties, TInt aError );

        void UpdatePbPluginMediaL( TBool aSeek);


        /**
        *  Handle completion of a asynchronous command
        *  @param aCommandResult result of the command, NULL if error
        *  @param aError error code
        */
        void HandlePlaybackCommandComplete( CMPXCommand* /*aCommandResult*/,
                                            TInt /*aError*/);
        /*
         *  From base class MMPXCollectionMediaObserver
         *  (via MMPXCollectionObserver)
         *  Handle extended media properties
         *
         *  @param aMedia media
         *  @param aError error code
         */
        inline void HandleCollectionMediaL( const CMPXMedia& /*aMedia*/, TInt /*aError*/ ) {}

        /*
         *  From base class MMPXCollectionObserver
         */
        inline void HandleCollectionMessage( CMPXMessage* /*aMsg*/, TInt /*aErr*/ ) {}

        inline void HandleOpenL( const CMPXMedia& /*aEntries*/,
                                 TInt /*aIndex*/,
                                 TBool /*aComplete*/,
                                 TInt /*aError*/ ) {}

        inline void HandleOpenL( const CMPXCollectionPlaylist& /*aPlaylist*/, TInt /*aError*/ ) {}

    public:
        /**
        * Set property
        */
        void SetPropertyL( TMPXPlaybackProperty aProperty, TInt aValue );
        virtual void RetrievePdlInformationL();

    protected:

        virtual void CloseEmbeddedPlaybackViewL() = 0;

        /*
         *  Handle Download State Change
         *  @param  aState  new state of the download
         */
        virtual void HandlePdlStateChangeL( TInt aState ) = 0;

        /*
         *  Handle transition to the stopped state
         */
        virtual void HandleStoppedStateL() = 0;

        virtual void HandleInitializingStateL( TMPXPlaybackState aLastState ) = 0;

        virtual void HandlePluginErrorL( TInt aError );

        virtual void ClosePlaybackViewL();

        virtual void HandleBufferingStateL();

        virtual void IssuePlayCommandL();

        void InitializeVideoPlaybackViewL();
        void DisplayInfoMessageL( TInt aResourceId, TBool aWaitingDialog = EFalse );
        void DisplayErrorMessageL( TInt aResourceId );

        CMPXVideoBasePlaybackView();

        /*
         *  Activates an active object to close the player
         *  @since 5.0
         */
        void ActivateClosePlayerActiveObject();

        /*
         *  Provides the static function for the callback to close the player
         *  Called by CIdle iIdle
         *  @since 3.2
         *  @param aPtr Pointer to callback class
         *  @return KErrNone
         */
        static TInt ClosePlayerL( TAny* aPtr );

        /*
         *  Called to stop and exit the player
         *  @since 3.2
         *  @return void
         */
        virtual void DoClosePlayerL();


        virtual void HandleVideoPlaybackMessage( CMPXMessage* aMessage );

        /**
         * Returns views Implementation id.
         * @return id for this view.
         */
        virtual TUid ViewImplementationId() const = 0;

        virtual void HandleClosePlaybackViewL();

        TBool IsAppInFrontL();

        TInt RetrieveBufferingPercentageL();

        void DoHandleInitializingStateL( TMPXPlaybackState aLastState );

        TBool IsMultiItemPlaylist();

    private:

        /**
        *  Handle playback message
        *
        *  @param aMsg playback message
        */
        virtual void DoHandlePlaybackMessageL( CMPXMessage* aMessage );

        /**
        *  Handle playback error message
        *
        *  @param aErr system error code.
        */
        void DoHandleErrorPlaybackMessageL( TInt aError );

        void SetDisplayWindowL();

        void CreateGeneralPlaybackCommandL( TMPXPlaybackCommand aCmd );

        void SetVideoRectL();

        void DisplayFileDetailsDialogL();

        /**
        *  Handle media properties.
        *
        *  @param aMedia media properties
        *  @param aError error code
        */
        virtual void DoHandleMediaL( const CMPXMedia& aMedia, TInt aError );

        void CreateAbortDsaCmdL();

        void CreateVideoSpecificCmdL( TMPXVideoPlaybackCommand aCmd );

        void RestartDsaL();

        void RequestMediaL();
        void RequestCollectionMediaL();
        void RequestPlaybackMediaL();

        void ParseMetaDataL( const CMPXMedia& aMedia );

        void DoHandleStateChangeL( TInt aNewState );

        void HandleGeneralPlaybackMessageL( CMPXMessage* aMessage );

        void SetAspectRatioL( TMPXVideoPlaybackCommand aCmd );

        void HandleVolumeCmdL( TMPXPlaybackCommand aCmd );

        void HandleShortPressBackwardL();

        TInt AddDllPath( TDes& aFileName );

        void ShowFileDetailsDialogL();

        void HandleDrmErrorsL( TInt aError );

        TInt OpenDrmFileHandleL( RFile& aFile );
        void LaunchDRMDetailsL();

        void HandleRealOneBitmapTimeoutL();

        void SendWindowCommandL( TMPXVideoPlaybackCommand aCmd );

        TBool IsInMemoryPlugin();

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        TInt OpenDrmFileHandle64L( RFile64& aFile );
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    protected: // data
        MMPXPlaybackUtility*                iPlaybackUtility;
        MMPXViewUtility*                    iViewUtility;
        MMPXCollectionUtility*              iCollectionUtility;
        CMPXVideoPlaybackDisplayHandler*    iDisplayHandler;
        CAknWsEventMonitor*                 iAknEventMonitor;

        TMPXPlaybackState                   iPlaybackState;

        CMPXVideoPlaybackContainer*         iContainer;

        TInt                                iVideoResourceOffset;

        CMPFileDetailsDialog*               iFileDetailsDialog;
        CMPXVideoPlaybackViewFileDetails*   iFileDetails;
        CIdle*                              iCloseAO;

        TBool                               iMediaRequested;
        TBool                               iPlaylistView;
        TBool                               iCollectionMediaRequested;
        TBool                               iPdlReloading;
        TBool                               iRealOneDelayedPlay;
        TBool                               iKeyboardInFocus;
};

#endif  // __VIDEOBASEPLAYBACKVIEW_H__

// EOF
