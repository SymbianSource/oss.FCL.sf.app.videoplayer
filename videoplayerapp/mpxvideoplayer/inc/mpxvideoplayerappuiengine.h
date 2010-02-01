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

// Version : %version: da1mmcf#27 %


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
class RConnection;

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
         *  Retrieve the playback utility pointer
         */
        inline MMPXPlaybackUtility* PlaybackUtility();

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
         * Sets mpx components ready for application shutdown
         */
        void PrepareCloseMpxL();

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

        /**
         * Process activation message.
         * @param aMsg reference to activation message
         */
        void ProcessActivationMessageL( const TDesC8 &aMsg );

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

        void CreateEmbeddedPdlPlaybackUtilityMemberVariablesL();

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

        /**
         *  Checks if application has to exit to Matrix root menu
         */
        TBool ExitToMatrixMenu();

        void InitializeFileL( const TDesC& aFileName );

        void ClosePlaybackPluginL();

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

        /*
         *  Sets the access point member variable
         */
        void SetAccessPointL();

        /**
         *  Checks if the the AP is WLAN
         *
         *  @param aAPId - the ID of access point to be evaluated
         *  @return ETrue if the bearer type is WLAN for this access point
         */
        TBool IsWLANAccessPointL(TInt aAPId);

        /**
         *  Reads the default AP
         *
         *  @return ID of the default Access Point read from the config file
         */
        TInt GetDefaultAccessPointL();

        /**
         *  Converts UID of the AP to and Access Point Id
         *
         *  @return ID of the Access Point
         */
        TInt GetAccessPointIdForUIDL(TUint32 aAPUid);

        /**
         *  Tries to get AP ID for connection.
         *  If default AP is not available, AP is queried from the user.
         *
         *  @return ID of the Access Point
         */
        TUint32 TryToGetAccessPointL();

        TInt HandleAiwGenericParamListL( const CAiwGenericParamList* aParams );

        /**
        * Handle collection message
        *
        * @param aMessage collection message
        */
       void DoHandleCollectionMessageL( CMPXMessage* aMessage );

       void CreatePlaybackUtilityMemberVariablesL();

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

       /*
       * Sends custom command to active view
       *
       * @param aMsg custom message data
       */
       void SendCustomCommandToActiveViewL( const TDesC8& aMsg );

       /*
       * Gets IAP ID from active connection.
       *
       * @param aConn   Connection to query.
       */
       TUint32 QueryIap( RConnection& aConn );

       void InitializeStreamingLinkL( const TDesC& aUri );
       void InitializePlaylistL( const CMPXCollectionPlaylist& aPlaylist, TBool aPlay );

    private:       // data

        CMpxVideoPlayerAppUi*         iAppUi;

        //
        //  Owned Utilities
        //
        MMPXPlaybackUtility*     iPlaybackUtility;
        MMPXViewUtility*         iViewUtility;
        MMPXCollectionUtility*   iCollectionUtility;
        MMPXCollectionUiHelper*  iCollectionUiHelper;  // own

        TUid iVideoCollectionId;

        CIdle*                        iExitAo;
        CMediaRecognizer*             iRecognizer;       // own
        CMpxVideoEmbeddedPdlHandler*  iPdlHandler;       // own

        TInt                     iAccessPointId;
        TInt                     iExtAccessPointId;
        TBool                    iMultilinkPlaylist;
        TBool                    iSeekable;
        TBool                    iUpdateSeekInfo;
        TBool                    iExitToMatrixMenu;
};

//
//  Inline methods
//
inline
MMPXPlaybackUtility* CMpxVideoPlayerAppUiEngine::PlaybackUtility()
{
    return iPlaybackUtility;
}

#endif             // CMPXVIDEOPLAYERAPPUIENGINE_H
