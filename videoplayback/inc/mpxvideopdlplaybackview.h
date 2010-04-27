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
* Description:  MPX Video PDL playback view
*
*/


// Version : %version: 7 %


// This file defines the API for VideoPlaybackView.dll

#ifndef __VIDEOPDLPLAYBACKVIEW_H__
#define __VIDEOPDLPLAYBACKVIEW_H__

//  Include Files
#include <mpxplaybackcommanddefs.h>

#include "mpxvideobaseplaybackview.h"

//  Constants

//  Forward Declarations

//
//  Class Definitions
//
class CMPXVideoPdlPlaybackView : public CMPXVideoBasePlaybackView
{
    public:
        IMPORT_C static CMPXVideoPdlPlaybackView* NewL();
        IMPORT_C static CMPXVideoPdlPlaybackView* NewLC();

        ~CMPXVideoPdlPlaybackView();

        void RetrievePdlInformationL();

    protected:

        /*
         * From CAknView
         * @return Views Uid
         */
        virtual TUid Id() const;

        TUid ViewImplementationId() const;

        void CloseEmbeddedPlaybackViewL();

        void HandlePluginErrorL( TInt aError );

        /*
         *  Handle Download State Change
         *  @param  aState  new state of the download
         */
        void HandlePdlStateChangeL( TInt aState );

        /*
         *  Handle transition to the stopped state
         */
        void HandleStoppedStateL();

        void HandleBufferingStateL();

        void HandleInitializingStateL( TMPXPlaybackState aLastState );

    private:

        CMPXVideoPdlPlaybackView();

        void ConstructL();

        void SendPdlCustomCommandL( TMPXPlaybackPdCommand aCustomCmd );

        TBool BringUpBrowserL();

        void ClosePlaybackViewWithErrorL();

    private: // data

        TMPXPlaybackPdDownloadState         iPdlState;
        TInt                                iDownloadSize;
};

#endif  // __VIDEOPDLPLAYBACKVIEW_H__

// EOF
