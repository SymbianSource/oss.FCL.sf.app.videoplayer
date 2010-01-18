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
* Description:  MPX Video playback view
*
*/

// Version : %version: 4 %


// This file defines the API for VideoPlaybackView.dll

#ifndef __VIDEOPLAYBACKVIEW_H__
#define __VIDEOPLAYBACKVIEW_H__

//  Include Files
#include "mpxvideobaseplaybackview.h"

//  Constants

//  Forward Declarations

//  Class Definitions

class CMPXVideoPlaybackView : public CMPXVideoBasePlaybackView
{
    public:

        /*
         *  Two phase constructor
         *
         *  @since 9.2
         *  @return  pointer to the new object
         */
        IMPORT_C static CMPXVideoPlaybackView* NewL();
        IMPORT_C static CMPXVideoPlaybackView* NewLC();

        /*
         *  Destuctor
         */
        ~CMPXVideoPlaybackView();

    protected:

        /*
         *  From CAknView
         *  Returns views id.
         *
         *  @return Views Uid
         */
        TUid Id() const;

        TUid ViewImplementationId() const;

        void CloseEmbeddedPlaybackViewL();

        virtual void HandlePluginErrorL( TInt aError );

        /*
         *  Handle Download State Change
         *  @param  aState  new state of the download
         */
        void HandlePdlStateChangeL( TInt aState );

        /*
         *  Handle transition to the stopped state
         */
        void HandleStoppedStateL();

    private:

        CMPXVideoPlaybackView();
        void ConstructL();
};

#endif  // __VIDEOPLAYBACKVIEW_H__

// EOF
