/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mpxvideoplayerappuiengine test class for STIF testing
*
*/

// Version : %version: ou1cpsw#14.1.1 %

#ifndef MPXVIDEOPLAYER_APPUIENGINETESTER_H_
#define MPXVIDEOPLAYER_APPUIENGINETESTER_H_

#include <e32base.h>
#include <e32def.h>
#include <StifItemParser.h>
#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <AiwGenericParam.h>
#include <mpxmessage2.h>
#include <TestScripterInternal.h>

#include "timeoutcontroller.h"

enum TActivationCommands
{
    EProcessTail,
    EProcessFile,
    EProcessStandAlone
};

enum TStifFileTypes
{
    ELocalFile,
    ESdpFile,
    ERamFile,
    EUrl,
    EAsxFile,
    EMultiLinkFile
};

enum TMpxUtilityEvents
{
    EViewUtilityPreLoadView,
    EViewUtilityActivateView,
    EViewUtilityActivateViewUid,
    EViewUtilityActivatePdlView,
    EViewUtilitySetAsDefaultView,
    EViewUtilityActivatePreviousView,
    EPlaybackUtilityInitFileHandle,
    EPlaybackUtilityInitFileName,
    EPlaybackUtilityInitStreamingUrl,
    EPlaybackUtilityInitStreamingFileHandle,
    EPlaybackUtilityInitPlaylist,
    EPlaybackUtilityStartPdl,
    EPlaybackUtilityPlayCmd,
    EPlaybackUtilityPdlInstance,
    EPlaybackUtilityClose,
    EPlaybackUtilityDisableEffects,
    ECollectionUtilityMedia,
    ECollectionUtilityOpen,
    EAppUiCmdExit,
    EPlaybackUtilityOpenPlaylist,
    EAsxAttributeSeekUpdate,
    EFunctionLeave
};

typedef struct
{
    TMpxUtilityEvents  iEvent;
    TUid               iUid;
    TFileName          iFileName;
    TInt               iDownloadId;
    TInt               iApId;
    TInt               iExtra;
} TCallbackEvent;

typedef CArrayPtrFlat<TCallbackEvent> CCallbackEventArray;



NONSHARABLE_CLASS( MAppUiEngineStifTestObserver )
{
    public:
        virtual void HandleUtilityEvent( TCallbackEvent* aEvent ) = 0;
};

class CMpxVideoPlayerAppUi;
class CMpxVideoPlayerAppUiEngine;
class CMPXCollectionUtility;
class CActiveSchedulerWait;

class CMpxVideoPlayer_AppUiEngineTester : public CBase,
                                          public MAppUiEngineStifTestObserver,
                                          public MTimeoutObserver
{
    public: // constructors and destructors
        static CMpxVideoPlayer_AppUiEngineTester* NewL( CScriptBase* aScriptBase );

        virtual ~CMpxVideoPlayer_AppUiEngineTester();

        TInt CreateEngineL( CStifItemParser& aItem );
        TInt CreateGenericParametersTerminateL();
        TInt OpenFileL( CStifItemParser& aItem );
        TInt OpenMediaL( CStifItemParser& aItem );
        TInt SetGenericParamAccessPointL( CStifItemParser& aItem );
        TInt OpenFileEmbeddedPdlL( CStifItemParser& aItem );
        TInt HandleMessageEmbeddedPdlL( CStifItemParser& aItem );
        TInt HandleMessageActivationMessageL( CStifItemParser& aItem );
        TInt StartStandAloneL( CStifItemParser& aItem );
        TInt HandleCollectionMessageL( CStifItemParser& aItem );
        TInt HandleCollectionMediaL( CStifItemParser& aItem );
        TInt HandleSoftkeyBackL( CStifItemParser& aItem );
        TInt HandleOpenPlaylistL( CStifItemParser& aItem );
        TInt HandleOpenMediaL( CStifItemParser& aItem );
        TInt HandleViewActivation( CStifItemParser& aItem );
        TInt ProcessCommandParametersL( CStifItemParser& aItem );
        TInt GetViewDepthL( CStifItemParser& aItem );
        TInt ClearPlaybackUtilityL( CStifItemParser& aItem );
        TInt ActivateLateConstructTimerL( CStifItemParser& aItem );
        TInt HandleEmbeddedOpenL( CStifItemParser& aItem );
        void ClearPdlInformation();

        //
        //  MAppUiEngineStifTestObserver Implementation
        //
        void HandleUtilityEvent( TCallbackEvent* aEvent );

    private:

        void ConstructL();

        CMpxVideoPlayer_AppUiEngineTester( CScriptBase* aScriptBase );

        void AddExpectedEvent( TCallbackEvent* aEvent );

        void CreateEmbeddedPdlGenericParametersL( TInt aDownloadId, TDesC& aFileName );

        void AddEmbeddedPdlCallbacksL( TInt aDownloadId, TDesC& aFileName );
        void AddMessagePdlCallbacksL( TInt aNewFile, TInt aDownloadId, TDesC& aFileName );

        CMPXMessage* CreateMpxMessageLC( TInt aEvent, TInt aType, TInt aData );

        void AddLocalFileCallbacksL( TDesC& aFileName,  TBool aUseFileHandle );
        void AddCollectionMediaCallbacksL( TDesC& aFileName );
        void AddSdpFileCallbacksL( TDesC& aFileName, TBool aUseFileHandle, CStifItemParser& aItem );
        void AddRamFileCallbacksL( TDesC& aFileName, TBool aUseFileHandle, CStifItemParser& aItem );
        void AddAsxFileCallbacksL( TDesC& aFileName, TBool aUseFileHandle, CStifItemParser& aItem );
        void AddUrlCallbacksL( TDesC& aFileName, CStifItemParser& aItem );

        void EndTest();

        void HandleTimeout( TInt error );

        TFileName GetFileNameAndPathL( CStifItemParser& aItem );

        void AddCollectionMediaPdlCallbacksL( TInt aDownloadId, TDesC& aFileName );

    private:

        CScriptBase*                iScriptBase;
        CMpxVideoPlayerAppUi*       iAppUi;
        CMpxVideoPlayerAppUiEngine* iAppUiEngine;

        MMPXPlaybackUtility*        iPlaybackUtility;
        MMPXViewUtility*            iViewUtility;
        CMPXCollectionUtility*      iCollectionUtility;

        CAiwGenericParamList*       iGenericParamList;

        CCallbackEventArray*        iExpectedEventArray;
        TInt                        iError;
        CSimpleTimeout*             iTimeoutController;
        
        CActiveSchedulerWait*       iActiveWait;

};

#endif /* MPXVIDEOPLAYER_APPUIENGINETESTER_H_ */
