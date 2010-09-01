/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares UI class for LWPlayer application.
 *
*/

// Version : %version: 5 %



#ifndef _LWPLAYERAPPUI_H_
#define _LWPLAYERAPPUI_H_

#include <aknViewAppUi.h>
#include <AknServerApp.h>

//
//  FORWARD DECLARATIONS
//
class CDocumentHandler;
class CAiwGenericParamList;

/*
 * Application UI class.
 * Provides support for the following features:
 * - EIKON control architecture
 */

class CLWPlayerAppUi : public CAknViewAppUi,
                       public MAknServerAppExitObserver
{
    public:
        /*
         * EPOC default constructor.
         */
        void ConstructL();

        /*
         * Destructor.
         */
        virtual ~CLWPlayerAppUi();

        // ----------------------------------------------------
        //  New Methods added for Progressive Download
        // ----------------------------------------------------

        /*
         * Process Commandline Parameters
         */
        TBool ProcessCommandParametersL( CApaCommandLine& aCommandLine );

        /*
         * Extract Generic Parameters
         */
        void ExtractGenericParamsL( const TDesC8& aParams );

        /*
         * Handle DlMgr Message
         */
        MCoeMessageObserver::TMessageResponse HandleMessageL(
                TUint32 aClientHandleOfTWG,
                TUid aMessageUid,
                const TDesC8& aMessageParameters);

        /*
         * From MAknServerAppExitObserver.
         */
        void HandleServerAppExit(TInt aReason);

    private:
        /*
         * From CEikAppUi, takes care of command handling.
         * @param aCommand command to be handled
         */
        void HandleCommandL( TInt aCommand );

        /*
         * From CEikAppUi, takes care of error handling.
         * @param aCommand command to be handled
         */
        void HandleErrorL( TInt aError );

        /*
         *  LaunchMediaPlayerL
         *
         *  Uses the document handler framework to open a file
         *  Used to create the embedded media player
         *
         *  @since S60 3.2
         *  @param afilename   filename of clip to be opened
         *  @param aparamList  list of parameters for progressive download
         *  @return error code
         */
        TInt LaunchMediaPlayerL( const TDesC& afilename,
                                 const CAiwGenericParamList* aparamList );


        /*
         *  SendMessageL
         *
         *  Finds the embedded media player and forwards the message
         *  from the browser to the embedded Media Player
         *
         *  @since S60 3.2
         *  @param aMessageParameters   list of parameters for prog. download
         *  @return error code
         */
        TInt SendMessageL(const TDesC8& aMessageParameters);

        /*
         *  DocumentHandlerL
         *
         *  Returns a reference to the Document Handler.
         *  Sets this class as an exit observer
         *
         *  @since S60 3.2
         *  @return reference to the document handler
         */
        CDocumentHandler& DocumentHandlerL();

        /*
         *  ActivateClosePlayerActiveObject
         *
         *  Starts an active object to close the LWPlayer.
         *
         *  @since S60 9.2
         */
        void ActivateClosePlayerActiveObject();

        /*
         *  Called to stop and exit the LWPlayer
         *  @since 3.2
         *  @return void
         */
        virtual void DoCloseLWPlayerL();

        /*
         *  Provides the static function for the callback to close the LWPlayer
         *  Called by CIdle iIdle
         *  @since 3.2
         *  @param aPtr Pointer to callback class
         *  @return KErrNone
         */
        static TInt CloseLWPlayerL( TAny* aPtr );

        /*
         *  Handles the Block Events Timer Timout
         *  @since 9.2
         *  @param aPtr Pointer to callback class
         *  @return KErrNone
         */
        static TInt HandleBlockEventsTimeout( TAny* aPtr );

        /*
         *  Called to unblock events
         *  @since 9.2
         *  @return void
         */
        void UnblockEvents();

        /*
         *  Called to block events
         *  @since 9.2
         *  @return void
         */
        void BlockEvents();

        //
        //  Member Variables
        //
        CDocumentHandler*      iDocHandler;
        TBool                  iEmbeddedMPCreated;
        TBool                  iAllowEvents;
        CIdle*                 iCloseAO;
        CPeriodic*             iBlockEventsTimer;
};

#endif

// End of File
