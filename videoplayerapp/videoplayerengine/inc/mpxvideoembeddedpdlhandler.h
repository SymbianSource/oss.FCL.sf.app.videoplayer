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
* Description:  Handles Embbeded PDL playback
 *
*/

// Version : %version: 2 %



#ifndef __CMPXVIDEOEMBEDDEDPDLHANDLER_H__
#define __CMPXVIDEOEMBEDDEDPDLHANDLER_H__

//
//  INCLUDES
//
#include <e32base.h>

#include <mpxplaybackframeworkdefs.h>
#include <mpxplaybackcommanddefs.h>

//
//  FORWARD DECLARATIONS
//
class CMpxVideoPlayerAppUiEngine;

//
//  CLASS DECLARATION
//

/*
 *  Process PDL commands passed in by other applications.
 *
 *  @lib mpxvideo.exe
 *  @since 9.2 time box
 */
class CMpxVideoEmbeddedPdlHandler : public CBase
{
    public:  // Constructors and destructor

        /*
         * Two-phased constructor.
         *
         * @since 9.2
         * @param aPlaybackUtility Playback utility to use.
         * @return Pointer to newly created object.
         */
        static CMpxVideoEmbeddedPdlHandler* NewL( CMpxVideoPlayerAppUiEngine* aAppUiEngine );

        /*
         *  Destructor.
         */
        virtual ~CMpxVideoEmbeddedPdlHandler();

    public:

        void ConnectToEmbeddedDownloadL( TInt aDlId, TDesC& aFileName );
        void ConnectToCollectionDownloadL( TInt aDlId, TDesC& aFileName );

        void ActivatePdlPlaybackViewL();

    private:

        /*
         *  C++ Constructor
         */
        CMpxVideoEmbeddedPdlHandler( CMpxVideoPlayerAppUiEngine* aAppUiEngine );

        /*
         *  By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:  // New functions

        void SendPdlCustomCommandL( TMPXPlaybackPdCommand aCustomCmd, TInt aData );

        void StartNewDownloadL( TInt aDlId, TDesC& aFileName );

    private:

        CMpxVideoPlayerAppUiEngine*   iAppUiEngine;

        TInt                          iDownloadId;
        HBufC*                        iDownloadFileName;
};

#endif // __CMPXVIDEOEMBEDDEDPDLHANDLER_H__

// End of File
