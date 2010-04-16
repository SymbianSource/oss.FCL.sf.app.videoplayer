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
#include "../stub/inc/mpxplaybackutilityimpl.h"
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
    pass[2] = "c:\\data\\TestMPXVideoViewWrapper.txt";
    
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
    delete mVideoViewWrapper;
    mVideoViewWrapper = NULL;
    
    delete mBaseVideoView;
    mBaseVideoView = NULL;
}


void TestMPXVideoViewWrapper::testRequestMedia()
{
    init();

    TRAPD(err, mVideoViewWrapper->RequestMediaL());
    QVERIFY( err == KErrNone );
	        
    QVERIFY( mVideoViewWrapper->iMediaRequested == true );
    
    cleanup();

}

void TestMPXVideoViewWrapper::testIsLive()
{
    init();

    TRAPD(err, mVideoViewWrapper->RequestMediaL());
    QVERIFY( err == KErrNone );
    
    mVideoViewWrapper->IsLive();       
    
    mVideoViewWrapper->iFileDetails->clearFileDetails();
    
    QVERIFY( ! mVideoViewWrapper->IsLive() );
                
    cleanup();
}


void TestMPXVideoViewWrapper::testIsPlaylist()
{
    init();

    mVideoViewWrapper->IsPlaylist();
            
    QVERIFY( mVideoViewWrapper->IsPlaylist() == false );
        
    cleanup();

}


void TestMPXVideoViewWrapper::testHandleCommand()
{
    init();
    
    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    TRAPD(errHdlCmd, mVideoViewWrapper->HandleCommandL( EMPXPbvCmdPlay ));    
    QVERIFY( errHdlCmd == KErrNone );

    int state;
    TRAPD(errState, state = mVideoViewWrapper->iPlaybackUtility->StateL());
    QVERIFY( errState == KErrNone );
    
    QVERIFY( state == EPbStatePlaying );        
    
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
        message->SetTObjectValueL<TInt>( KMPXMediaVideoTvOutPlayAllowed, EFalse );
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

    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    TRAPD(errSetProp, mVideoViewWrapper->SetPropertyL( EPbPropertyMute, 1 ) );   // 0=>noraml  1=>muted
    QVERIFY( errSetProp == KErrNone );
    
    QVERIFY( ! mVideoViewWrapper->iFileDetails->mAudioEnabled );
            
    cleanup();
}


void TestMPXVideoViewWrapper::testHandleProperty()
{
    init();
    
    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    TRAPD(errHdlProp, mVideoViewWrapper->HandlePropertyL( EPbPropertyDuration, 5000, KErrNone ));   
    QVERIFY( errHdlProp == KErrNone );
    
    QVERIFY( mVideoViewWrapper->iFileDetails->mDuration == 5000 );
            
    cleanup();
}


void TestMPXVideoViewWrapper::testRetrieveFileNameAndMode()
{
    init();
    
    mVideoViewWrapper->iFileDetails->clearFileDetails();  
    
    CMPXCommand* cmd = NULL;
    
    TRAP_IGNORE( cmd = CMPXCommand::NewL() );

    TRAPD(errRetFileName, mVideoViewWrapper->RetrieveFileNameAndModeL( cmd ) );
    QVERIFY( errRetFileName == KErrNone );
    
    int state;
    TRAPD(errState, state = mVideoViewWrapper->iPlaybackUtility->StateL());
    QVERIFY( errState == KErrNone );    
    
    QVERIFY( state == EPbStateInitialised );
    
    QCOMPARE( mVideoViewWrapper->iFileDetails->mClipName, QString("testClip.3gp"));
    
    delete cmd;
    cmd = NULL;
    
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


void TestMPXVideoViewWrapper::testIssuePlayCommand()
{
    init();
    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    TRAPD(errIssuePlay, mVideoViewWrapper->IssuePlayCommandL());
    QVERIFY( errIssuePlay == KErrNone );
    
    
    int state;
    TRAPD(errState, state = mVideoViewWrapper->iPlaybackUtility->StateL());
    QVERIFY( errState == KErrNone );    
    
    QVERIFY( state == EPbStatePlaying );    
    
    cleanup();
}


void TestMPXVideoViewWrapper::testSetAspectRatio()
{
    init();
    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    TRAPD(errHdlCmd, mVideoViewWrapper->HandleCommandL(EMPXPbvCmdStretchAspectRatio));
    QVERIFY( errHdlCmd == KErrNone );
            
    QVERIFY( mVideoViewWrapper->iDisplayHandler->iCurrentIndexForAspectRatio == 1 );
        
    cleanup();
}


void TestMPXVideoViewWrapper::testIsAppInFront()
{
    init();
    
    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    bool front = false;
    TRAPD(errIsAppInFrnt, front = mVideoViewWrapper->IsAppInFrontL());
    
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
        
    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    TRAPD(errIssuePlay, mVideoViewWrapper->IssuePlayCommandL());
    QVERIFY( errIssuePlay == KErrNone );
    
    TRAPD(errHdlCmd, mVideoViewWrapper->HandleCommandL(EMPXPbvCmdDecreaseVolume));
    QVERIFY( errHdlCmd == KErrNone );    
        
    QVERIFY( mVideoViewWrapper->iFileDetails->mAudioEnabled );
    
    cleanup();
}


void TestMPXVideoViewWrapper::testHandleShortPressBackward()
{
    init();
        
    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    TRAPD(errIssuePlay, mVideoViewWrapper->IssuePlayCommandL());
    QVERIFY( errIssuePlay == KErrNone );
    
    TRAPD(errHdlCmd, mVideoViewWrapper->HandleCommandL(EMPXPbvCmdShortPressBackward));
    QVERIFY( errHdlCmd == KErrNone );  
            
    cleanup();
}


void TestMPXVideoViewWrapper::testIssueVideoAppForegroundCmd()
{
    init();
        
    TRAPD(errReqMedia, mVideoViewWrapper->RequestMediaL());
    QVERIFY( errReqMedia == KErrNone );
    
    TRAPD(errIssuePlay, mVideoViewWrapper->IssuePlayCommandL());
    QVERIFY( errIssuePlay == KErrNone );
    
    TRAPD(errIssueVidAppFGCmd, mVideoViewWrapper->IssueVideoAppForegroundCmdL( ETrue ));
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
        
    mVideoViewWrapper->UpdateVideoRect(0,0,50,50,false);
        
    float brY = 50;
    
    QCOMPARE(mVideoViewWrapper->iDisplayHandler->iBrYDiff, brY);
                        
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




void TestMPXVideoViewWrapper::testCreateGeneralPlaybackCommand()
{
    init();

    TRAPD(err, mVideoViewWrapper->CreateGeneralPlaybackCommandL( EPbCmdDecreaseVolume ));  
    QVERIFY( err == KErrNone );
    
    QVERIFY( ! mVideoViewWrapper->iFileDetails->mAudioEnabled );         
                        
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
    
    TRAP_IGNORE(
        RArray<TInt> suppIds;
        CleanupClosePushL( suppIds );            
        suppIds.AppendL( KMPXMediaIdGeneral );
        suppIds.AppendL( KMPXMediaIdVideo );
    
        media = CMPXMedia::NewL( suppIds.Array() );
        CleanupStack::PopAndDestroy( &suppIds );
    
        media->SetTObjectValueL<TInt>( TMPXAttribute( KMPXMediaVideoError ), 
                                       KErrCancel );
    );
    
    TRAPD(err, mVideoViewWrapper->HandleMediaL( *media, KErrNone ) );
    QVERIFY( err == KErrNone );
    
    QCOMPARE( mVideoViewWrapper->iView->mCurrentError, KErrCancel );

    if ( media )
    {    
        delete media;
        media = NULL;      
    }
  
                            
    cleanup();
}


// End of file
    


