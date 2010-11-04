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
* Description:  App UI engine
*
*/


// Version : %version: 37 %


#ifndef CMPXVIDEOPLAYERAPPUIENGINE_H
#define CMPXVIDEOPLAYERAPPUIENGINE_H

#include <e32base.h>
#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <mpxcollectionutility.h>
#include <mpxmessage2.h>
#include <mpxcollectionplaylist.h>
#include <mediarecognizer.h>
#include <mpxviewactivationobserver.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionuihelperobserver.h>

#ifdef __APPUIENGINESTIF__
#include "mpxvideoplayerappui_stub.h"
#else
#include "mpxvideoplayerappui.h"
#endif

class CMpxVideoEmbeddedPdlHandler;
class MMPXCollectionUiHelper;
class CAiwGenericParamList;
class CVideoPlaylistUtility;
class CMpxVideoPlayerIadUpdate;

/**
*  Application UI class.
*
*  @lib mpxvideoplayer.exe
*  @since MpxVideoPlayer 0.1
*/
class CMpxVideoPlayerAppUiEngine : public CBase,
                                   public MMPXViewActivationObserver,
                                   public MMPXCollectionObserver ,
                                   public MMPXCHelperEmbeddedOpenObserver
{
    public:        // Constructors and destructor
        static CMpxVideoPlayerAppUiEngine* NewL( CMpxVideoPlayerAppUi* aAppUi );

        /**
        * Destructor.
        */
        virtual ~CMpxVideoPlayerAppUiEngine();

    public:

        /*
         *  Retrieves playback utility and creates it if needed
         */
        MMPXPlaybackUtility& PlaybackUtilityL();

        /**
         * Opens the specified file in response to a corresponding message.
         *
         * @param aFile File to be opened.
         * @param aParams aiw generic parameters for the file
         */
        void OpenFileL( RFile& aFile, const CAiwGenericParamList* aParams );

        /**
         * Opens the specified file in response to a corresponding message.
         *
         * @param aFile File to be opened.
         */
        void OpenFileL( const TDesC& aFileName );

        /**
         * Opens the specified mpx media object.
         *
         * @param aMedia Media to be opened.
         */
        void OpenMediaL( const CMPXMedia& aMedia );

        /*
         *  Activate the proper playback view
         */
        void ActivatePlaybackViewL();

        /**
         * Sets AppUiEngine in stand alone "mode"
         */
        void StartStandAloneL();

        /**
         * Handle media properties.
         */
        void DoHandleCollectionMediaL( const CMPXMedia& aMedia );

        /**
         * Steps one level up in collection path
         */
        void StepBackCollectionPathL();

        /*
         * Handle embedded playback message
         * @param aMessageUid message identification uid
         * @param aMessageParameters  aiw generic parameters
         */
        TBool HandleMessageL( TUid aMessageUid,
                              const TDesC8& aMessageParameters );

        /**
         * From MMPXViewActivationObserver
         * Handle view activation.
         *
         * @since 3.1
         * @param aCurrentViewType Current view type Uid.
         * @param aPreviousViewType Previous view type Uid.
         */
        void HandleViewActivation( const TUid& aCurrentViewType, const TUid& aPreviousViewType );


        /**
         *  From MPXCollectionObserver
         *  @since S60 3.2.3
         *  @param aMessage collection message, ownership not transferred.
         *         Please check aMsg is not NULL before using it. If aErr is not
         *         KErrNone, plugin might still call back with more info in the aMsg.
         *  @param aErr system error code
         */
        void HandleCollectionMessage( CMPXMessage* aMsg, TInt /*aErr*/ );

        /**
         * From MMPXCollectionObserver
         * Handle media properties.
         *
         * @since 3.1
         * @param aMedia  media properties.
         * @param aError Error code.
         */
        void HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError );

         /**
         * From MMPXCollectionObserver
         * Handles the collection entries being opened.
         *
         * @since 3.1
         * @param aEntries Collection entries opened.
         * @param aIndex Focused entry.
         * @param aComplete ETrue no more entries. EFalse more entries
         *                  expected.
         * @param aError Error code.
         */
        void HandleOpenL( const CMPXMedia& aEntries,
                          TInt aIndex,
                          TBool aComplete,
                          TInt aError );

        /**
         * From MMPXCollectionObserver
         * Handles the item being opened.
         *
         * @since 3.1
         * @param aPlaylist Collection playlist, owner ship is transfered.
         * @param aError Error code.
         */
        void HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError);

        // from base class MMPXCHelperEmbeddedOpenObserver
            /**
            * From MMPXCHelperEmbeddedOpenObserver
            * Handles errors from opening in embedded mode
            *
            * @since 3.1
            * @param aErr Error code
            * @param aCategory Type of item to be opened.
            */
        void HandleEmbeddedOpenL( TInt aErr, TMPXGeneralCategory aCategory  );

        void PreLoadPdlPlaybackViewL();

        /*
         * Handles the "back" button.
         */
        void HandleSoftKeyBackL();

        /*
         *  Processes shell commands.
         *  @param aCommand
         *  @param aDocumentName
         *  @param aTail
         *  @return  ETrue if document name exists
         */
        TBool ProcessCommandParametersL( TApaCommand aCommand,
                                         TFileName& aDocumentName,
                                         const TDesC8& aTail );

        /*
         *  Provides the static function for the callback to exit the application
         *  Called by CIdle iIdle
         *  @since 9.2
         *  @param aPtr Pointer to callback class
         *  @return KErrNone
         */
        static TInt ExitApplicationL( TAny* aPtr );

        /*
         * returns the viewdepth from the viewutlity
         */
        TInt ViewHistoryDepth();

        void ClearPdlInformation();

        void InitializeFileL( const TDesC& aFileName );

        void ClosePlaybackPluginL();

        void SendMessageToPdlViewL( TInt aMsg );

        /*
         *  Activates an active object to finish the
         *  initialization of the standalone application
         */
        void ActivateLateConstructTimerL();
                
        /**
         * Clears internal playbackutility pointer
         */
        void ClearPlaybackUtility();
        
        /**
         * Sends given command to playback utility 
         * @since 9.2
         * @param aCmd reference to command
         */
        void SendCustomMpxPlaybackUtilityCmdL( CMPXCommand& aCmd );
        
        /**
         * Sends given command to playback utility 
         * @since 9.2
         * @param aCmd reference to command
         */
        void SendMpxPlaybackCmdL( TMPXPlaybackCommand aCmd );

    private:
        /**
         * Constructor
         */
        CMpxVideoPlayerAppUiEngine( CMpxVideoPlayerAppUi* aAppUi );

        /**
         * Default constructor
         */
        CMpxVideoPlayerAppUiEngine( );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();


        void HandleMultiLinksFileL( const TDesC& aFileName,
                                    CMediaRecognizer::TMediaType aMediaType );

        void HandleMultiLinksFileL( RFile& aFile,
                                    CMediaRecognizer::TMediaType aMediaType );

        void DoHandleMultiLinksFileL( CVideoPlaylistUtility* aPlaylistUtil,
                                      TBool aSingleLink,
                                      TBool aLocalFile );

        TPtrC GetLinkLC( const TDesC& aFileName,
                         CMediaRecognizer::TMediaType aMediaType,
                         TBool aUseFileHandle = EFalse );

        TInt HandleAiwGenericParamListL( const CAiwGenericParamList* aParams );

        /**
        * Handle collection message
        *
        * @param aMessage collection message
        */
       void DoHandleCollectionMessageL( CMPXMessage* aMessage );

       void CreateCollectionUtilityMemberVariablesL();

       void CreateRemoteControlListenerL();

       /*
        *  Activates an active object to exit the application
        *  @since 5.0
        */
       void ActivateExitActiveObject();

       /*
        *  Called to stop and exit the application
        *  @since 9.2
        *  @return void
        */
       virtual void DoExitApplicationL();

       /*
       * used to send media info to plugin
       */
       void UpdatePbPluginMediaL();

       void InitializeStreamingLinkL( const TDesC& aUri );
       void InitializePlaylistL( const CMPXCollectionPlaylist& aPlaylist, TBool aPlay );

        /*
         *  Provides the static function for the callback to
         *  finish the standalone application construction
         *  Called by CPeriodic iConstructTimer
         *  @param aPtr Pointer to callback class
         *  @return KErrNone
         */
        static TInt LateConstructCallback( TAny* aPtr );

        /*
         *  Called to finalize the standalone
         *  application initialization.
         */
        virtual void DoLateConstructL();
        
        /**
         * Handles error codes coming from MMPXPlaybackUtility
         * method calls  
         * @since 9.2
         * @param aError error code
         */
        void HandleUtilityErrorL( TInt aError );


        /**
         * Checks for updates via IAD.
         */
        void DoCheckForUpdatesL();

    private:       // data

        CMpxVideoPlayerAppUi*         iAppUi;

        //
        //  Owned Utilities
        //
        MMPXPlaybackUtility*     iPlaybackUtility;
        MMPXViewUtility*         iViewUtility;
        MMPXCollectionUtility*   iCollectionUtility;
        MMPXCollectionUiHelper*  iCollectionUiHelper;  // own

        CPeriodic*                    iConstructTimer;
        CIdle*                        iExitAo;
        CMediaRecognizer*             iRecognizer;       // own
        CMpxVideoEmbeddedPdlHandler*  iPdlHandler;       // own
        CMpxVideoPlayerIadUpdate*     iIadUpdate;        // own

        TInt                     iAccessPointId;
        TBool                    iMultilinkPlaylist;
        TBool                    iSeekable;
        TBool                    iUpdateSeekInfo;
    };

#endif             // CMPXVIDEOPLAYERAPPUIENGINE_H
