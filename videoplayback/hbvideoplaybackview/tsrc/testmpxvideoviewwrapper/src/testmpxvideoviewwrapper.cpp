/**
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
* Description:   tester for methods in VideoSortFilterProxyModel
* 
*/

// Version : %version:  8 %

#include <e32err.h>
#include <w32std.h>

#include <hbapplication.h>
#include <hbinstance.h>
#include <QDebug>

#include <mpxplaybackframeworkdefs.h>
#include <mpxplaybackutility.h>
#include <mpxcommonvideoplaybackview.hrh>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediavideodefs.h>

#include "testmpxvideoviewwrapper.h"

#include "../stub/inc/hbvideobaseplaybackview.h"
#include "../stub/inc/mpxvideoplaybackviewfiledetails.h"
#include "../stub/inc/mpxvideoplaybackdisplayhandler.h"
#include "../stub/inc/mpxvideoplaybackcontrolscontroller.h"


#define protected public
#include "mpxvideoviewwrapper.h"
#undef protected


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    
    TestMPXVideoViewWrapper tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmpxvideoviewwrapper.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestMPXVideoViewWrapper::init()
{	  
    mBaseVideoView    = new HbVideoBasePlaybackView();
    TRAPD( err, mVideoViewWrapper = CMPXVideoViewWrapper::NewL( mBaseVideoView ) );	  
    QVERIFY( err == KErrNone );
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestMPXVideoViewWrapper::cleanup()
{
    if ( mVideoViewWrapper )
    {
        delete mVideoViewWrapper;
        mVideoViewWrapper = NULL;        
    }

    if ( mBaseVideoView )
    {
        delete mBaseVideoView;
        mBaseVideoView = NULL;
    }
}

void TestMPXVideoViewWrapper::testRequestMedia()
{
    init();

    TRAPD(err, mVideoViewWrapper->RequestMediaL());
    QVERIFY( err == KErrNone );
	        
    QVERIFY( mVideoViewWrapper->iMediaRequestStatus == MediaRequested );
    
    cleanup();
}

void TestMPXVideoViewWrapper::testIsLive()
{
    init();

    TRAPD(err, mVideoViewWrapper->RequestMediaL());
    QVERIFY( err == KErrNone );
    
    mVideoViewWrapper->iFileDetails->mPlaybackMode = EMPXVideoLiveStreaming;
    QVERIFY( mVideoViewWrapper->IsLive() );

    mVideoViewWrapper->iFileDetails->mPlaybackMode = EMPXVideoLocal;
    QVERIFY( ! mVideoViewWrapper->IsLive() );
                
    cleanup();
}

void TestMPXVideoViewWrapper::testIsPlaylist()
{
    init();

    mVideoViewWrapper->iPlaylistView = true;
    QVERIFY( mVideoViewWrapper->IsPlaylist() == true );
        
    mVideoViewWrapper->iPlaylistView = false;
    QVERIFY( mVideoViewWrapper->IsPlaylist() == false );

    cleanup();
}

void TestMPXVideoViewWrapper::testHandleCommand()
{
    init();
    
    TRAPD( errReqMedia, mVideoViewWrapper->RequestMediaL() );
    QVERIFY( errReqMedia == KErrNone );

    //
    // Test 'Play' command
    //
    TRAPD( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdPlay ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPlay );
    
    //
    // Test 'Pause' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdPause ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPause );

    //
    // Test 'Close' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdClose ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdClose );
    
    //
    // Test 'Seek Forward' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdSeekForward ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdStartVideoSeekingForward );

    //
    // Test 'Seek Backward' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdSeekBackward ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdStartVideoSeekingBackward );
    
    //
    // Test 'End Seek' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdEndSeek ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdStopVideoSeeking );
    
    //
    // Test 'PlayPause' command
    //
    mVideoViewWrapper->iPlaybackState = EPbStatePlaying;
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdPlayPause ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPause );

    mVideoViewWrapper->iPlaybackState = EPbStatePaused;
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdPlayPause ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPlay );

    //
    // Test 'Stop' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdStop ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdStop );
    
    //
    // Test 'Decrease Volume' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdDecreaseVolume ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdHandleDecreaseVolume );
    
    //
    // Test 'Increase Volume' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdIncreaseVolume ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdHandleIncreaseVolume );
        
    //
    // Test 'Natural Aspect Ratio' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdNaturalAspectRatio ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iDisplayHandler->iCommand == EPbCmdNaturalAspectRatio );
    
    //
    // Test 'Zoom Aspect Ratio' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdZoomAspectRatio ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iDisplayHandler->iCommand == EPbCmdZoomAspectRatio );
    
    //
    // Test 'Stretch Aspect Ratio' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdStretchAspectRatio ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iDisplayHandler->iCommand == EPbCmdStretchAspectRatio );
    
    //
    // Test 'Mute' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdMute ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdMuteVolume );
    
    //
    // Test 'Un-mute' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdUnMute ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdUnMuteVolume );
    
    //
    // Test 'Short Press Backward' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdShortPressBackward ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iProperty == EPbPropertyPosition );
    
    //
    // Test 'Reset Controls' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdResetControls ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdInitView );
    
    //
    // Test 'Next List Item' command
    //
    mVideoViewWrapper->iPlaylistView = true;
    mVideoViewWrapper->iFileDetails->mMultiItemPlaylist = true;    
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdNextListItem ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdNext );
    
    //
    // Test 'Previous List Item' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdPreviousListItem ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPrevious );
    
    //
    // Test 'End Of Clip' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdEndOfClip ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdEndofClipReached );
    
    //
    // Test 'Custom Pause' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdCustomPause ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdCustomPause );
    
    //
    // Test 'Custom Play' command
    //
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdCustomPlay ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdCustomPlay );
    
    //
    // Test 'RealOne Bitmap Timeout' command
    //
    mVideoViewWrapper->iMediaRequestStatus = MediaDelivered;
    TRAP( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdRealOneBitmapTimeout ) );    
    QVERIFY( errHdlCmd == KErrNone );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPlay );
    
    cleanup();
}

void TestMPXVideoViewWrapper::testHandlePluginError()
{
    init();

    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    mVideoViewWrapper->HandlePluginError( KErrNotSupported );
    
    QVERIFY( mVideoViewWrapper->iView->mCurrentError == KErrNotSupported );
    
    cleanup();
}

void TestMPXVideoViewWrapper::testHandlePlaybackMessage()
{
    init();

    //***************************
    // Test Video Msg
    //***************************
    CMPXMessage* message = NULL;
    TRAP_IGNORE( 
        message = CMPXMessage::NewL();    
        message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMediaIdVideoPlayback );
        message->SetTObjectValueL<TMPXVideoPlaybackCommand>
                                            ( KMPXMediaVideoPlaybackCommand, EPbCmdTvOutEvent );
        message->SetTObjectValueL<TInt>( KMPXMediaVideoTvOutConnected, ETrue );
    );
    mVideoViewWrapper->HandlePlaybackMessage( message, KErrNone );
    QVERIFY( mVideoViewWrapper->iFileDetails->mTvOutConnected );    
    if ( message )
    {
        delete message;
        message = NULL;
    }    

    //***************************
    // Test General Msg
    //***************************
    TRAP_IGNORE(     
        message = CMPXMessage::NewL();
        message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
        message->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, 28 );   // EReachedEndOfPlaylist = 28 
        message->SetTObjectValueL<TInt>( KMPXMessageGeneralType, 0 );
        message->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0 );   
    );
    mVideoViewWrapper->HandlePlaybackMessage( message, KErrNone );    
    QVERIFY( ! mVideoViewWrapper->iView->mViewActive );    
    if ( message )
    {
        delete message;
        message = NULL;
    } 

    //***************************
    // Test Video Msg with err
    //***************************
    TRAP_IGNORE( message = CMPXMessage::NewL() );
    mVideoViewWrapper->HandlePlaybackMessage( message, KErrNotFound );
    QVERIFY( mVideoViewWrapper->iView->mCurrentError == KErrNotFound );    
    if ( message )
    {
        delete message;
        message = NULL;
    } 
        
    cleanup();
}

void TestMPXVideoViewWrapper::testSetProperty()
{
    init();

    TRAPD( errReqMedia, mVideoViewWrapper->RequestMediaL()) ;
    QVERIFY( errReqMedia == KErrNone );

    TMPXPlaybackProperty property = EPbPropertyMute;
    TInt propertyValue = 1;

    TRAPD( errSetProp, mVideoViewWrapper->SetPropertyL( property, propertyValue ) );
    QVERIFY( errSetProp == KErrNone );

    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iProperty == property );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iPropertyValue == propertyValue );

    cleanup();
}

void TestMPXVideoViewWrapper::testHandleProperty()
{
    init();

    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );

    //
    // Duration
    //
    int value = 5000;
    TRAPD(errHdlProp, mVideoViewWrapper->HandlePropertyL( EPbPropertyDuration, value, KErrNone ) );   
    QVERIFY( errHdlProp == KErrNone );

    QVERIFY( mVideoViewWrapper->iControlsController->mReceivedEvent == EMPXControlCmdSetDuration );
    QVERIFY( mVideoViewWrapper->iControlsController->mValue == value );

    //
    // Position
    //
    value = 500;

    TRAP(errHdlProp, mVideoViewWrapper->HandlePropertyL( EPbPropertyPosition, value, KErrNone ) );   
    QVERIFY( errHdlProp == KErrNone );

    QVERIFY( mVideoViewWrapper->iControlsController->mReceivedEvent == EMPXControlCmdSetPosition );
    QVERIFY( mVideoViewWrapper->iControlsController->mValue == value );

    //
    // Volume
    //
    value = 10;

    TRAP(errHdlProp, mVideoViewWrapper->HandlePropertyL( EPbPropertyVolume, value, KErrNone ) );   
    QVERIFY( errHdlProp == KErrNone );

    QVERIFY( mVideoViewWrapper->iControlsController->mReceivedEvent == EMPXControlCmdSetVolume );
    QVERIFY( mVideoViewWrapper->iControlsController->mValue == value );

    //
    // Mute
    //
    value = 1;

    TRAP(errHdlProp, mVideoViewWrapper->HandlePropertyL( EPbPropertyMute, value, KErrNone ) );   
    QVERIFY( errHdlProp == KErrNone );

    QVERIFY( mVideoViewWrapper->iControlsController->mReceivedEvent == EMPXControlCmdSetVolume );
    QVERIFY( mVideoViewWrapper->iControlsController->mValue == 0 );

    cleanup();
}

void TestMPXVideoViewWrapper::testRetrieveFileNameAndMode()
{
    init();

    mVideoViewWrapper->iFileDetails->clearFileDetails();  

    CMPXCommand* cmd = NULL;
    
    TRAP_IGNORE( cmd = CMPXCommand::NewL() );

    if ( cmd )
    {
        TRAPD(errRetFileName, mVideoViewWrapper->RetrieveFileNameAndModeL( cmd ) );
        QVERIFY( errRetFileName == KErrNone );
        
        QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdInitView );    
        
        QCOMPARE( mVideoViewWrapper->iFileDetails->mClipName, QString("testClip.3gp"));

        delete cmd;
        cmd = NULL;
    }

    cleanup();
}

void TestMPXVideoViewWrapper::testActivateClosePlayerActiveObject()
{
    init();

    mVideoViewWrapper->ActivateClosePlayerActiveObject();

    QVERIFY( mVideoViewWrapper->iCloseAO->IsActive() );

    cleanup();
}

void TestMPXVideoViewWrapper::testDoClosePlayer()
{
    init();

    TRAPD(err, mVideoViewWrapper->DoClosePlayerL());
    QVERIFY( err == KErrNone );  

    QVERIFY( ! mVideoViewWrapper->iView->mViewActive );

    cleanup();
}

void TestMPXVideoViewWrapper::testSetAspectRatio()
{
    init();
    TRAPD( errReqMedia, mVideoViewWrapper->RequestMediaL() );
    QVERIFY( errReqMedia == KErrNone );

    TRAPD( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdStretchAspectRatio ) );
    QVERIFY( errHdlCmd == KErrNone );

    QVERIFY( mVideoViewWrapper->iDisplayHandler->iAspectRatio == EMMFStretch );

    cleanup();
}

void TestMPXVideoViewWrapper::testIsAppInFront()
{
    init();

    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );

    bool front = false;
    TRAPD( errIsAppInFrnt, front = mVideoViewWrapper->IsAppInFrontL() );

    QVERIFY( errIsAppInFrnt == KErrNone );

    cleanup();
}

void TestMPXVideoViewWrapper::testClosePlaybackView()
{
    init();

    TRAPD(err, mVideoViewWrapper->ClosePlaybackViewL());
    QVERIFY( err == KErrNone );

    QVERIFY( ! mVideoViewWrapper->iView->mViewActive );

    cleanup();
}

void TestMPXVideoViewWrapper::testHandleVolumeCmd()
{
    init();

    TRAPD( errReqMedia, mVideoViewWrapper->RequestMediaL() );
    QVERIFY( errReqMedia == KErrNone );

    TRAPD( errIssuePlay, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdPlay ) );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPlay );
    QVERIFY( errIssuePlay == KErrNone );

    TRAPD( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdDecreaseVolume ) );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdHandleDecreaseVolume );
    QVERIFY( errHdlCmd == KErrNone );    

    QVERIFY( mVideoViewWrapper->iFileDetails->mAudioEnabled );

    cleanup();
}

void TestMPXVideoViewWrapper::testHandleShortPressBackward()
{
    init();

    TRAPD( errReqMedia, mVideoViewWrapper->RequestMediaL() );
    QVERIFY( errReqMedia == KErrNone );

    TRAPD( errIssuePlay, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdPlay ) );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPlay );
    QVERIFY( errIssuePlay == KErrNone );

    TRAPD( errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdShortPressBackward ) );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iProperty == EPbPropertyPosition );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iPropertyValue == 0 );

    QVERIFY( errHdlCmd == KErrNone );  

    cleanup();
}

void TestMPXVideoViewWrapper::testIssueVideoAppForegroundCmd()
{
    init();

    TRAPD( errReqMedia, mVideoViewWrapper->RequestMediaL() );
    QVERIFY( errReqMedia == KErrNone );

    TRAPD( errIssuePlay, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdPlay ) );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPlay );

    QVERIFY( errIssuePlay == KErrNone );

    //
    // test foreground
    //
    TRAPD( errIssueVidAppFGCmd, mVideoViewWrapper->IssueVideoAppForegroundCmdL( ETrue ) );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdHandleForeground );
    QVERIFY( errIssueVidAppFGCmd == KErrNone );

    //
    // test background
    //
    TRAP( errIssueVidAppFGCmd, mVideoViewWrapper->IssueVideoAppForegroundCmdL( EFalse ) );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdHandleBackground );
    QVERIFY( errIssueVidAppFGCmd == KErrNone );

    cleanup();
}

void TestMPXVideoViewWrapper::testCreateControls()
{
    init();

    TRAPD(err, mVideoViewWrapper->CreateControlsL());
    QVERIFY( err == KErrNone );

    QVERIFY( mVideoViewWrapper->iControlsController );

    cleanup();
}

void TestMPXVideoViewWrapper::testIsMultiItemPlaylist()
{
    init();

    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );

    QVERIFY( ! mVideoViewWrapper->IsMultiItemPlaylist() );

    cleanup();
}

void TestMPXVideoViewWrapper::testUpdateVideoRect()
{
    init();

    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );

    TRect rect( 0, 0, 50, 50 );

    mVideoViewWrapper->UpdateVideoRect( 
        rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY, false );       

    QVERIFY( rect == mVideoViewWrapper->iDisplayHandler->iRect );

    cleanup();
}

void TestMPXVideoViewWrapper::testUpdateVideoRectDone()
{
    init();

    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );

    mVideoViewWrapper->UpdateVideoRectDone();

    cleanup();
}

void TestMPXVideoViewWrapper::testHandleBufferingState()
{
    init();

    TRAPD(err, mVideoViewWrapper->HandleBufferingStateL());
    QVERIFY( err == KErrNone );

    cleanup();
}

void TestMPXVideoViewWrapper::testHandleVideoPlaybackMessage()
{
    init();

    CMPXMessage* message = NULL;
    TRAP_IGNORE(
        message = CMPXMessage::NewL();     
        message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMediaIdVideoPlayback );
        message->SetTObjectValueL<TMPXVideoPlaybackCommand>
                                            ( KMPXMediaVideoPlaybackCommand, EPbCmdPluginError );
        message->SetTObjectValueL<TInt>( KMPXMediaVideoError, KErrNotSupported );
    );
    
    mVideoViewWrapper->HandleVideoPlaybackMessage( message );    
    
    QCOMPARE( mVideoViewWrapper->iView->mCurrentError, KErrNotSupported );   
    if ( message )
    {
        delete message;
        message = NULL;            
    }
                        
    cleanup();
}

void TestMPXVideoViewWrapper::testHandlePlaybackCommandComplete()
{
    init();

    CMPXCommand* cmd = NULL;
	  
    TRAP_IGNORE( cmd = CMPXCommand::NewL() );
    
    mVideoViewWrapper->HandlePlaybackCommandComplete( cmd, KErrNone );    
    
    QVERIFY( mVideoViewWrapper );
    
    delete cmd;
    cmd = NULL;    
                            
    cleanup();
}

void TestMPXVideoViewWrapper::testHandleMedia()
{
    init();
        
    CMPXMedia* media = NULL;

    //
    // Error case
    //
    TRAP_IGNORE(
        RArray<TInt> suppIds;
        CleanupClosePushL( suppIds );            
        suppIds.AppendL( KMPXMediaIdGeneral );
        suppIds.AppendL( KMPXMediaIdVideo );
    
        media = CMPXMedia::NewL( suppIds.Array() );
        CleanupStack::PopAndDestroy( &suppIds );
    
        media->SetTObjectValueL<TInt>( TMPXAttribute( KMPXMediaVideoError ), KErrCancel );
    );
    
    TRAPD( err, mVideoViewWrapper->HandleMediaL( *media, KErrNone ) );
    QVERIFY( err == KErrNone );
    
    QCOMPARE( mVideoViewWrapper->iView->mCurrentError, KErrCancel );
    QVERIFY( ! mVideoViewWrapper->iControlsController->mFileDetailsAdded );
    QVERIFY( mVideoViewWrapper->iMediaRequestStatus == MediaNotRequested );

    if ( media )
    {    
        delete media;
        media = NULL;      
    }
    
    //
    // working case - RN logo is not visible
    //
    TRAP_IGNORE(
        RArray<TInt> suppIds;
        CleanupClosePushL( suppIds );            
        suppIds.AppendL( KMPXMediaIdGeneral );
        suppIds.AppendL( KMPXMediaIdVideo );
    
        media = CMPXMedia::NewL( suppIds.Array() );
        CleanupStack::PopAndDestroy( &suppIds );
    );

    mVideoViewWrapper->iView->mCurrentError = KErrNone;
    mVideoViewWrapper->iFileDetails->mVideoEnabled = true;
    mVideoViewWrapper->iDisplayHandler->SetAspectRatioL( EPbCmdNaturalAspectRatio );
    mVideoViewWrapper->iControlsController->mRNLogoVisible = false;

    TRAP(err, mVideoViewWrapper->HandleMediaL( *media, KErrNone ) );

    QVERIFY( err == KErrNone );
    QCOMPARE( mVideoViewWrapper->iView->mCurrentError, KErrNone );
    QVERIFY( mVideoViewWrapper->iControlsController->mFileDetailsAdded );
    QVERIFY( mVideoViewWrapper->iControlsController->mReceivedEvent == EMPXControlCmdSetAspectRatio );
    QVERIFY( mVideoViewWrapper->iControlsController->mValue == EMMFNatural );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPlay );
    QVERIFY( mVideoViewWrapper->iMediaRequestStatus == MediaDelivered );

    if ( media )
    {    
        delete media;
        media = NULL;      
    }

    //
    // working case - RN logo is visible
    //
    TRAP_IGNORE(
        RArray<TInt> suppIds;
        CleanupClosePushL( suppIds );            
        suppIds.AppendL( KMPXMediaIdGeneral );
        suppIds.AppendL( KMPXMediaIdVideo );
    
        media = CMPXMedia::NewL( suppIds.Array() );
        CleanupStack::PopAndDestroy( &suppIds );
    );

    mVideoViewWrapper->iFileDetails->mVideoEnabled = false;
    mVideoViewWrapper->iPlaybackUtility->iCommand = EPbCmdPause;
    mVideoViewWrapper->iControlsController->mFileDetailsAdded = false;
    mVideoViewWrapper->iDisplayHandler->SetAspectRatioL( EPbCmdZoomAspectRatio );
    mVideoViewWrapper->iControlsController->mRNLogoVisible = true;

    TRAP(err, mVideoViewWrapper->HandleMediaL( *media, KErrNone ) );

    QVERIFY( err == KErrNone );
    QVERIFY( ! mVideoViewWrapper->iControlsController->mFileDetailsAdded );
    QVERIFY( mVideoViewWrapper->iControlsController->mReceivedEvent == EMPXControlCmdSetAspectRatio );
    QVERIFY( mVideoViewWrapper->iControlsController->mValue == EMMFNatural );
    QVERIFY( mVideoViewWrapper->iPlaybackUtility->iCommand == EPbCmdPause );
    QCOMPARE( mVideoViewWrapper->iView->mCurrentError, KErrNone );
    QVERIFY( mVideoViewWrapper->iMediaRequestStatus == MediaDelivered );

    if ( media )
    {    
        delete media;
        media = NULL;      
    }

    cleanup();
}

// End of file
