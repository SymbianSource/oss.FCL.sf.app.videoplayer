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
* Description:  App UI
*
*/

// Version : %version: da1mmcf#14 %


#ifndef CMPXVIDEOPLAYERAPPUI_H
#define CMPXVIDEOPLAYERAPPUI_H

#include <aknViewAppUi.h>


// FORWARD DECLARATIONS
class CAiwGenericParamList;
class CMpxVideoPlayerMediaKeyHandler;
class CMpxVideoPlayerRemConListener;
class CMpxVideoPlayerAppUiEngine;

/**
*  Application UI class.
*
*  @lib mpxvideoplayer.exe
*  @since MpxVideoPlayer 0.1
*/
class CMpxVideoPlayerAppUi : public CAknViewAppUi
{
    public:        // Constructors and destructor

        /**
        * Default constructor
        */
        CMpxVideoPlayerAppUi();

        /**
        * Destructor.
        */
        ~CMpxVideoPlayerAppUi();

    public:       // New functions

        /**
        * Exit
        */
        void HandleExit();

        /**
        * From CEikAppUi, processes shell commands.
        * @param aCommand
        * @param aDocumentName
        * @param aTail
        *  @return  ETrue if document name exists        
        */
        TBool ProcessCommandParametersL( TApaCommand aCommand,
                                         TFileName& aDocumentName,
                                         const TDesC8& aTail );

        /**
         * From CEikAppUi
         * handle window server events.
         *
         * @param aEvent The window server event to be handled.
         * @param aDestination The control associated with the event.
         */
        void HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination );

        /**
        * Command handling
        * @param aCmd Command ID
        */
        void HandleCommandL( TInt aCommand );

        /**
         * This function is used for querying whether the application
         * is launched in embedded mode or not.
         * @since 3.1
         * @return ETrue:  The application is launched in embedded mode.
         *         EFalse: The application is launched in standalone mode.
         */
        TBool IsEmbedded() const;

        /**
         * Opens the specified file in response.
         * @since 3.1
         * @param aFile      File handle to open
         * @param aParams    Generic parameter list from document handler
         */
        void OpenFileL( RFile& aFile, const CAiwGenericParamList* aParams );
        
        /**
         * Returns the active view.
         * Ownership is not transferred.
         *
         * @return Active akn view or NULL if no views.
         */
        CAknView* View();

        /**
         * Sends a message to Matrix menu.
         * @param aMessage The message to be sent.
         */
        void LaunchMmViewL( const TDesC8& aMessage );
        
    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
         * Checks if the stored wg id is the same as the current wg id of embedded player.
		 * If it is clears it.
         * @since 3.1         
         */
        void DoWgIdUpkeep();

        /**
         * From CEikAppUi
         * Receives closing notification from video storage
         *
         * @param aClientHandleOfTargetWindowGroup Target window group (this app).
         * @param aMessageUid The TUid type of the message
         * @param aMessageParameters A reference to constant descriptor of optional parameters
         */
        MCoeMessageObserver::TMessageResponse HandleMessageL(
                                                    TUint32 aClientHandleOfTargetWindowGroup,
                                                    TUid aMessageUid,
                                                    const TDesC8 &aMessageParameters);

        /**
         * From CEikAppUi
         * opens the specified file in response to a corresponding message.
         *
         * @param aFileName File to be opened.
         */
        void OpenFileL( const TDesC& aFileName );

        /**
         * From CCoeAppUi
         * called when event occurs of type EEventFocusLost or EEventFocusGained.
         *
         * @param aForeground ETrue if the application is in the foreground,
         *                    otherwise EFalse.
         */
        void HandleForegroundEventL( TBool aForeground );

    private:       // data

        // own
        CMpxVideoPlayerAppUiEngine* iAppUiEngine;
        TBool                       iEndKeyExit;

};

#endif             // CMPXVIDEOPLAYERAPPUI_H
