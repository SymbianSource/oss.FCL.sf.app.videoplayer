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
* Description:   This class plays files with the Helix Engine
*
*/

// Version : %version: 14 %


#ifndef _CMPXVIDEOPLAYBACKMODE_H_
#define _CMPXVIDEOPLAYBACKMODE_H_

//
//  INCLUDES
//
#include <e32def.h>
#include <e32base.h>

#include "mpxhelixplaybackplugindefs.h"
#include "mpxvideodlmgrif.h"
#include "mpxvideo_debug.h"

//
//  FORWARD DECLARATIONS
//
class CMPXVideoPlaybackController;
class CMPXVideoDlMgrIf;


//
//  CLASS DECLARATION
//

/*
 *  CMpxVideoPlaybackMode class
 */
NONSHARABLE_CLASS( CMPXVideoPlaybackMode ) : public CBase
{
    public:
        //
        //  Constructors and destructor
        //

        /*
         *  Destructor
         *  Destroy the object and release all memory objects
         */
        ~CMPXVideoPlaybackMode();

        inline virtual TInt GetMode();
        virtual void ConnectToDownloadL( CMPXCommand& aCmd );
        virtual void HandleOpenComplete();
        virtual void GetPdlStatusL( CMPXCommand& aCmd );
        virtual void UpdateSeekPosition( TInt64& aPosition );
        inline virtual TBool IsDownloadPaused();
        virtual void HandlePause();
        virtual void HandleBackground();
        virtual TBool IsNetworkMode2GL();
        virtual TBool CanPlayNow();
        virtual TBool IsTvOutAllowedL();
        virtual void OpenFileL( const TDesC& aMediaFile );
        virtual void OpenFileL( const RFile& aMediaFile );

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        virtual void OpenFile64L( const RFile64& aMediaFile );
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API


    protected:
        /*
         *  C++ default constructor
         */
        CMPXVideoPlaybackMode();

        /*
         *  the second phase constructor ConstructL to safely construct things
         *  that can leave
         */
        virtual void ConstructL( CMPXVideoPlaybackController* aVideoPlaybackCtlr );


    protected:
        //
        //  Data
        //
        CMPXVideoPlaybackController*        iVideoPlaybackCtlr;   // not owned

};

/*******************************************************/
/*         SUB CLASS DECLARATIONS                      */
/*******************************************************/

////////////////////////////////////////////////////////
///     Declaration of CMPXLocalPlaybackMode
////////////////////////////////////////////////////////
class CMPXLocalPlaybackMode : public CMPXVideoPlaybackMode
{
    public:
        static CMPXVideoPlaybackMode* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXLocalPlaybackMode();

        // Methods where video plabkack behavior varies for local media
        inline virtual TInt GetMode();
};


////////////////////////////////////////////////////////
///     Declaration of CMPXStreamingPlaybackMode
////////////////////////////////////////////////////////
class CMPXStreamingPlaybackMode : public CMPXVideoPlaybackMode
{
    public:
        static CMPXVideoPlaybackMode* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXStreamingPlaybackMode();

        // Methods where video plabkack behavior varies for streaming media
        inline virtual TInt GetMode();
        virtual TBool CanPlayNow();
        virtual void HandleOpenComplete();
        TBool IsTvOutAllowedL();
        void OpenFileL( const TDesC& aMediaFile );
        virtual void HandlePause();
};

////////////////////////////////////////////////////////////
///     Declaration of CMPXLiveStreamingPlaybackMode
///
///     live streaming is a specialized streaming case
////////////////////////////////////////////////////////////
class CMPXLiveStreamingPlaybackMode : public CMPXStreamingPlaybackMode
{
    public:
        static CMPXVideoPlaybackMode* NewL(CMPXVideoPlaybackController* aVideoPlaybackCtlr);
        virtual ~CMPXLiveStreamingPlaybackMode();

        // Methods where video plabkack behavior varies for live streaming media
        inline virtual TInt GetMode();
        virtual void HandlePause();
        virtual void HandleBackground();
};

////////////////////////////////////////////////////////////////////////////////
//
//     Declaration of CMPXProgressiveDLPlaybackMode
//
//     Progressive Download is a specialized local playback case
//
////////////////////////////////////////////////////////////////////////////////
class CMPXProgressiveDLPlaybackMode : public CMPXLocalPlaybackMode
{
    public:
        static CMPXVideoPlaybackMode* NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr );
        virtual ~CMPXProgressiveDLPlaybackMode();

        inline virtual TInt GetMode();
        void ConnectToDownloadL( CMPXCommand& aCmd );
        void HandleOpenComplete();
        void GetPdlStatusL( CMPXCommand& aCmd );
        void UpdateSeekPosition( TInt64& aPosition );
        inline TBool IsDownloadPaused();

    private:

        void ConstructL( CMPXVideoPlaybackController* aVideoPlaybackCtlr );

    private:
        CMPXVideoDlMgrIf*  iDlMgrIf;   // owned
};

// INLINE METHODS

inline
TInt CMPXVideoPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::GetMode()"));
    // sub-classes to return their respective value
    return EMPXNumberOfModes;
}

inline
TInt CMPXLocalPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXLocalPlaybackMode::GetMode()"));
    return EMPXVideoLocal;
}

inline
TInt CMPXStreamingPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXStreamingPlaybackMode::GetMode()"));
    return EMPXVideoStreaming;
}

inline
TInt CMPXLiveStreamingPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXLiveStreamingPlaybackMode::GetMode()"));
    return EMPXVideoLiveStreaming;
}

inline
TInt CMPXProgressiveDLPlaybackMode::GetMode()
{
    MPX_DEBUG(_L("CMPXProgressiveDLPlaybackMode::GetMode()"));
    return EMPXVideoProgressiveDownload;
}

inline
TBool CMPXVideoPlaybackMode::IsDownloadPaused()
{
    return EFalse;
}

inline
TBool CMPXProgressiveDLPlaybackMode::IsDownloadPaused()
{
    return iDlMgrIf->IsDownloadPaused();
}

#endif  //_CMPXVIDEOPLAYBACKMODE_H_
