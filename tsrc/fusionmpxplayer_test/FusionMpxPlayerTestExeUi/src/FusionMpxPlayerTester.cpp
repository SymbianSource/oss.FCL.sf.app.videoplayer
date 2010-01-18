/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <S32STRM.H>
#include <S32MEM.H>

#include "VCXTestLog.h"
#include "CIptvTestActiveWait.h"
#include "VCXTestCommon.h"

#include "FusionMpxPlayerTester.h"
#include "FusionMpxPlayerTestCommon.h"

#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxviewutility.h>
#include <mpxmediabase.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectionutility.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionplaylist.h>

#include <mpxvideoplaybackdefs.h>
#include <vcxmyvideosuids.h>
#include <vcxmyvideosdefs.h>

// CONSTANTS
// From mpxvideoplayerconstants.h
const TUid KVideoPlaybackViewUid = { 0x200159AD };
_LIT8( KDATATYPEVIDEOHELIX, "video/videohelix" );

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::NewL
// -----------------------------------------------------------------------------
//
CFusionMpxPlayerTester* CFusionMpxPlayerTester::NewL( MFusionMpxPlayerTesterObserver* aObserver )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::NewL");
    CFusionMpxPlayerTester* self = new (ELeave) CFusionMpxPlayerTester();
    CleanupStack::PushL(self);
    self->ConstructL( aObserver );
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CFusionMpxPlayerTester::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::CFusionMpxPlayerTester
// -----------------------------------------------------------------------------
//
CFusionMpxPlayerTester::CFusionMpxPlayerTester()
    {
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::ConstructL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::ConstructL( MFusionMpxPlayerTesterObserver* aObserver )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::ConstructL");

    iWait = CIptvTestActiveWait::NewL();
    
    iObserver = aObserver;
    
    iViewUtility = MMPXViewUtility::UtilityL();
    iViewUtility->AddObserverL( this );
    iViewUtility->ConstructDefaultViewHistoryL();

    VCXLOGLO1("<<<CFusionMpxPlayerTester::ConstructL");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::~CFusionMpxPlayerTester
// -----------------------------------------------------------------------------
//
CFusionMpxPlayerTester::~CFusionMpxPlayerTester()
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::~CFusionMpxPlayerTester");

    if ( iViewUtility )
        {
        iViewUtility->RemoveObserver( this );
        iViewUtility->Close();
        VCXLOGLO1("CFusionMpxPlayerTester::~CFusionMpxPlayerTester - viewUtility destroyed.");
        }
    
    if( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) ); 
        iPlaybackUtility->Close();
        VCXLOGLO1("CFusionMpxPlayerTester::~CFusionMpxPlayerTester - playbackutility destroyed.");
        }

    iExpectedGeneralMessages.Reset();
    
    delete iWait;
    iWait = NULL;    
    
    VCXLOGLO1("<<<CFusionMpxPlayerTester::~CFusionMpxPlayerTester");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::OpenLocalFile
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayerTester::OpenLocalFile( const TDesC& aFileName )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::OpenLocalFile");
    
    VCXLOGLO2("CFusionMpxPlayerTester::OpenLocalFileL: aFileName: %S", &aFileName);
  
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EPlayerChanged, KFusionTestMsgNone, KFusionTestMsgNone ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateInitialising, KFusionTestMsgNone ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EMediaChanged, KFusionTestMsgNone, KFusionTestMsgNone ) );
    // Start playback is always true
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EInitializeComplete, KFusionTestMsgNone, 1 ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateInitialised, KFusionTestMsgNone ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyPosition, 0 ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateBuffering, KFusionTestMsgNone ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePlaying, KFusionTestMsgNone ) );

    if( !iPlaybackUtility )
        {
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeNewPlayer );
        iPlaybackUtility->AddObserverL( *this );
        iPlaybackUtility->CommandL( EPbCmdSetAutoResume, EFalse );
        }
    
    TInt err( KErrNone );
    
    TRAP( err, iViewUtility->PreLoadViewL( KVideoPlaybackViewUid ) );
    if( err == KErrNone ) 
        {       
        TRAP( err, iPlaybackUtility->InitL( aFileName ) );
        }
    else
        {
        VCXLOGLO2("CFusionMpxPlayerTester:: PreLoadViewL left: %d", err);
        }

    VCXLOGLO1("<<<CFusionMpxPlayerTester::OpenLocalFile");
    return err;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::OpenUrlL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::OpenUrlL( const TDesC& aUrl, TInt aIapId )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::OpenUrlL");
    
    VCXLOGLO2("CFusionMpxPlayerTester::OpenUrlL: aUrl: %S", &aUrl);
    VCXLOGLO2("CFusionMpxPlayerTester::OpenUrlL: aIapId: %d", aIapId);
    
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EPlayerChanged, KFusionTestMsgNone, KFusionTestMsgNone ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateInitialising, KFusionTestMsgNone ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EMediaChanged, KFusionTestMsgNone, KFusionTestMsgNone ) );
    // Start playback is always true
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EInitializeComplete, KFusionTestMsgNone, 1 ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateInitialised, KFusionTestMsgNone ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyPosition, 0 ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateBuffering, KFusionTestMsgNone ) );
    iExpectedGeneralMessages.Append( 
            TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePlaying, KFusionTestMsgNone ) );
    
    if( !iPlaybackUtility )
        {
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeNewPlayer );
        iPlaybackUtility->AddObserverL( *this );
        iPlaybackUtility->CommandL( EPbCmdSetAutoResume, EFalse );
        }
    
    TInt err( KErrNone );
    
    TRAP( err, iViewUtility->PreLoadViewL( KVideoPlaybackViewUid ) );
    if( err == KErrNone ) 
        {       
        iPlaybackUtility->InitStreamingL( aUrl,
                                      (TDesC8*)(&KDATATYPEVIDEOHELIX),
                                      aIapId );
        }
    else
        {
        VCXLOGLO2("CFusionMpxPlayerTester:: PreLoadViewL left: %d", err);
        }    

    VCXLOGLO1("<<<CFusionMpxPlayerTester::OpenUrlL");
    }

// -------------------------------------------------------------------------------------------------
//   CFusionMpxPlayerTester::CreateGeneralPlaybackCommandL()
// -------------------------------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::CreateGeneralPlaybackCommandL( TMPXPlaybackCommand aCmd, TInt aData )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::CreateGeneralPlaybackCommandL");
    
    TBuf<256> cmdName;
    GetGeneralPlaybackCommandDes( aCmd, cmdName );
    VCXLOGLO2(">>>CFusionMpxPlayerTester:: cmd: %S", &cmdName);

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, aCmd );
    
    AddExpectedGeneralPlaybackCommandMessages( aCmd, aData );
    
    if( aData != KTestUndefined )
        {
        cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralData, aData );
        }

    iPlaybackUtility->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
    
    if( iExpectedGeneralMessages.Count() == 0 )
        {
        iObserver->TesterCommandComplete();
        }

    VCXLOGLO2("<<<CFusionMpxPlayerTester::CreateGeneralPlaybackCommandL (%d)", aCmd );
    }

// -------------------------------------------------------------------------------------------------
// CFusionMpxPlayerTester::CreateVideoPlaybackCommandL
// -------------------------------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::CreateVideoPlaybackCommandL( TMPXVideoPlaybackCommand aCmd )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::CreateVideoPlaybackCommandL");

    TBuf<256> cmdName;
    GetVideoPlaybackCommandDes( aCmd, cmdName );
    VCXLOGLO2(">>>CFusionMpxPlayerTester:: cmd: %S", &cmdName);

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );
    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>(KMPXMediaVideoPlaybackCommand, aCmd );
    
    AddExpectedVideoPlaybackCommandMessages( aCmd );
    
    iPlaybackUtility->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
    
    if( iExpectedGeneralMessages.Count() == 0 )
        {
        iObserver->TesterCommandComplete();
        }

    VCXLOGLO1("<<<CFusionMpxPlayerTester::CreateVideoPlaybackCommandL");
    }

// -------------------------------------------------------------------------------------------------
// CFusionMpxPlayerTester::CheckExpectedMessages
// -------------------------------------------------------------------------------------------------
//
TInt CFusionMpxPlayerTester::CheckExpectedMessages()
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::CheckExpectedMessages");
    
    TInt err( KErrNone );
    
    if( iExpectedGeneralMessages.Count() > 0 )
        {
        err = KErrCorrupt;
        VCXLOGLO1("CFusionMpxPlayerTester:: Did not get all expected general messages, error!");
        
        for( TInt i=0; i<iExpectedGeneralMessages.Count(); i++ )
            {
            TBuf<256> eventBuff( _L("") );
            TBuf<256> typeBuff( _L("") );
            TBuf<64> dataBuff( _L("") );
            
            TInt event = iExpectedGeneralMessages[i].iEvent;
            
            GetGeneralPlaybackEventDes( event, eventBuff );

            if( iExpectedGeneralMessages[i].iData != KFusionTestMsgNone )
                {
                dataBuff.AppendNum( iExpectedGeneralMessages[i].iData );
                }
            
            if( event == TMPXPlaybackMessage::EStateChanged )
                {
                GetStateDes( static_cast<TMPXPlaybackState>(iExpectedGeneralMessages[i].iType), typeBuff );
                }
            
            if( event == TMPXPlaybackMessage::EPropertyChanged )
                {
                GetPropertyDes( static_cast<TMPXPlaybackProperty>(iExpectedGeneralMessages[i].iType), typeBuff );
                }
            
            VCXLOGLO4("CFusionMpxPlayerTester:: event %S = %S ( %S )", &eventBuff, &typeBuff, &dataBuff);
            }
        }
    
    VCXLOGLO1("<<<CFusionMpxPlayerTester::CheckExpectedMessages");
    return err;
    }

// -------------------------------------------------------------------------------------------------
// CFusionMpxPlayerTester::AddExpectedGeneralPlaybackCommandMessages
// -------------------------------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::AddExpectedGeneralPlaybackCommandMessages( TMPXPlaybackCommand aCmd, TInt &aData )
    {
    switch( aCmd )
        {
        case EPbCmdPlay: // 0 Play which ever track is currently stopped, paused etc.
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePlaying, KFusionTestMsgNone ) );
            }
            break;
        case EPbCmdPlayWithFadeIn: // 1 uses KPbFadeInDuration
            {
            
            }
            break;
        case EPbCmdPause:    // 2
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePaused, KFusionTestMsgNone ) );
            }
            break;
        case EPbCmdStop:     // 3
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateStopped, KFusionTestMsgNone ) );
            }
            break;
        case EPbCmdNext:     // 4 No effect if no next track
            {
            
            }
            break;
        case EPbCmdPrevious: // 5 No effect if no previous track;
                        //   if pos>KPbReplayThreshold, replays current track
            {
            
            }
            break;
        case EPbCmdReplay:   // 6 Restart current track at beginning
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyPosition, 0 ) );           
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePlaying, KFusionTestMsgNone ) );
            }
            break;
        case EPbCmdStartSeekForward:  // 7
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateSeekingForward, KFusionTestMsgNone ) );
            
            }
            break;
        case EPbCmdStartSeekBackward: // 8
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateSeekingBackward, KFusionTestMsgNone ) );
            }
            break;
        case EPbCmdStopSeeking:       // 9
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePlaying, KFusionTestMsgNone ) );
            }
            break;
        case EPbCmdIncreaseVolume:    // 10
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: Getting volume.");
            // Get volume.
            iPlaybackUtility->PropertyL( *this, EPbPropertyVolume );
            iWait->Start();
            
            TInt vol = iVolume + 10;
            if( vol > 100 ) vol = 100;
                        
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyVolume, vol ) );           
            }
            break;
        case EPbCmdDecreaseVolume:    // 11
            {
            // Get volume.
            iPlaybackUtility->PropertyL( *this, EPbPropertyVolume );
            iWait->Start();
            
            TInt vol = iVolume - 10;
            if( vol < 0 ) 
                {
                vol = 0;
                }

            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyVolume, vol ) );           
            }
            break;
        case EPbCmdMuteVolume:        // 12
            {
            // Get volume.
            iPlaybackUtility->PropertyL( *this, EPbPropertyVolume );
            iWait->Start();
            
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyVolume, 0 ) );
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyMute, 1 ) );
            
            iVolumeBeforeMute = iVolume;
            }
            break;
        case EPbCmdUnMuteVolume:      // 13
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyVolume, iVolumeBeforeMute ) );
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyMute, 0 ) );
            }
            break;
        case EPbCmdClose:             // 14
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateStopped, KFusionTestMsgNone ) );
            }
            break;
        case EPbApplyEffect:          // 15 Apply an effect to the current playback,
                                 //    aData used to identify effect
            {
            
            }
            break;
        case EPbCmdPreservePosition:  // 16 Preserve playback position
            {
            
            }
            break;
        case EPbCmdPreserveState:     // 17 Preserve playback state
            {
            
            }
            break;
        case EPbCmdCloseItem:         // 18 Request to close the file handle
            {
            
            }
            break;
        case EPbCmdPlayPause:         // 19 Play/Pause toggle command
            {
            if( iState == EPbStatePlaying )
                {
                iExpectedGeneralMessages.Append( 
                        TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePaused, KFusionTestMsgNone ) );
                }
            else
            if( iState == EPbStatePaused )
                {
                iExpectedGeneralMessages.Append( 
                        TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePlaying, KFusionTestMsgNone ) );
                }
            }
            break;
        case EPbCmdCancelInit:         // 20
            {
            if( !iInitComplete )
                {
                iExpectedGeneralMessages.Reset();
                iExpectedGeneralMessages.Append( 
                        TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStateStopped, KFusionTestMsgNone ) );
                }
            }
            break;
        case EPbCmdResetPreserveState: // 21
            {
            
            }
            break;
        case EPbCmdUnloadNonActivePlugin: // 22 KMPXCommandPlaybackGeneralData is TUid.iUid
            {
            
            }
            break;
        case EPbCmdClearKeyBuffer:        // 23 Clear all media key commands
            {
            
            }
            break;
        case EPbCmdDisableEffect:
            {
            
            }
            break;
        case EPbCmdSetVolume:             // 25 Set volume to the value specified by aData
            {
            // Get volume.
            iPlaybackUtility->PropertyL( *this, EPbPropertyVolume );
            iWait->Start();
            // Make sure volume changes.
            if( aData == iVolume ) 
                {
                aData += 10;
                if( aData > 100 ) aData -= 20;
                }
            VCXLOGLO3("CFusionMpxPlayerTester:: volume before: %d, set to: %d", iVolume, aData );
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EPropertyChanged, EPbPropertyVolume, aData ) );           
            }
            break;
        default:
            break;
        }
    }

// -------------------------------------------------------------------------------------------------
// CFusionMpxPlayerTester::AddExpectedVideoPlaybackCommandMessages
// -------------------------------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::AddExpectedVideoPlaybackCommandMessages( TMPXVideoPlaybackCommand aCmd )
    {
    switch( aCmd )
        {
        case EPbCmdInitView:
            {
            
            }
            break;
        case EPbCmdAbortDSA:
            {
            
            }
            break;
        case EPbCmdRestartDSA:
            {
            
            }
            break;
        case EPbCmdSetDisplayWindow:
            {
            
            }
            break;
        case EPbCmdStartVideoSeekingForward:
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePluginSeeking, KFusionTestMsgNone ) );
            }
            break;
        case EPbCmdStartVideoSeekingBackward:
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePluginSeeking, KFusionTestMsgNone ) );
            }
            break;
        case EPbCmdStopVideoSeeking:
            {
            iExpectedGeneralMessages.Append( 
                    TExpectedGeneralMessage( TMPXPlaybackMessage::EStateChanged, EPbStatePlaying, KFusionTestMsgNone ) );            
            }
            break;
        case EPbCmdHandleForeground:
            {
            
            }
            break;
        case EPbCmdHandleBackground:
            {
            
            }
            break;
        case EPbCmdNaturalAspectRatio:
            {
            
            }
            break;
        case EPbCmdZoomAspectRatio:
            {
            
            }
            break;
        case EPbCmdStretchAspectRatio:
            {
            
            }
            break;
        case EPbCmdSetDefaultAspectRatio:
            {
            
            }
            break;        
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::PrepareCloseL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::PrepareCloseL()
    {
    if( iPlaybackUtility )
        {
        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        TRAP_IGNORE( manager.ClearSelectPlayersL() );
        iPlaybackUtility->CommandL( EPbCmdClose );
        }
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::HandlePlaybackMessage
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::HandlePlaybackMessage");
    if( aError == KErrNone && aMessage )
        {
        TMPXMessageId id( *aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ) );    

        if ( id == KMPXMessageGeneral )
            {
            TRAP_IGNORE( DoHandleGeneralPlaybackMessageL( *aMessage ) );
            }
        else
        if( id == KMPXMediaIdVideoPlayback )
            {
            TRAP_IGNORE( DoHandleVideoPlaybackMessageL( *aMessage ) );
            }
        }
    VCXLOGLO1("<<<CFusionMpxPlayerTester::HandlePlaybackMessage");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::DoHandleGeneralPlaybackMessageL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::DoHandleGeneralPlaybackMessageL( const CMPXMessage& aMessage )
    {
    VCXLOGLO1("CFusionMpxPlayerTester:: ----------------------------");
    VCXLOGLO1("CFusionMpxPlayerTester::  KMPXMessageGeneral message ");
    VCXLOGLO1("CFusionMpxPlayerTester:: ----------------------------");
    
    TInt event = *aMessage.Value<TInt>( KMPXMessageGeneralEvent );
    TInt type = *aMessage.Value<TInt>( KMPXMessageGeneralType );
    TInt data = *aMessage.Value<TInt>( KMPXMessageGeneralData );
    
    for( TInt i=0; i<iExpectedGeneralMessages.Count(); i++ )
        {
        if( iExpectedGeneralMessages[i].iEvent == event && 
            ( iExpectedGeneralMessages[i].iType == type || iExpectedGeneralMessages[i].iType == KFusionTestMsgNone ) &&  
            ( iExpectedGeneralMessages[i].iData == data || iExpectedGeneralMessages[i].iData == KFusionTestMsgNone ) )
            {
            iExpectedGeneralMessages.Remove( i );
            
            if( iExpectedGeneralMessages.Count() == 0 && iObserver )
                {
                iObserver->TesterCommandComplete();
                }
            break;
            }
        }
    
    switch ( event )
        {
        case TMPXPlaybackMessage::ENoEvent:
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: ENoEvent");
            break;
            }
        case TMPXPlaybackMessage::EError: // EError must be 1. iData is error originating from player: iType=event type
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EError ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: type: %d", type);
            VCXLOGLO2("CFusionMpxPlayerTester:: data: %d", data);
            break;
            }
        case TMPXPlaybackMessage::ECommandReceived: // iType=TMPXPlaybackCommand
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: ECommandReceived");
            break;
            }
        case TMPXPlaybackMessage::ECommandComplete: // iType=TMPXPlaybackCommand
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: ECommandComplete ------>");
            break;
            }
        case TMPXPlaybackMessage::EPropertyChanged: // iType=TMPXPlaybackProperty; iData=new value
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EPropertyChanged ------>");
            DoHandlePropertyChangeL( type, data );
            break;
            }
        case TMPXPlaybackMessage::EStateChanged: // iType=TMPXPlaybackState
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EStateChanged ------>");
            DoHandleStateChangeL( static_cast<TMPXPlaybackState>( type ) );
            break;
            }
        case TMPXPlaybackMessage::ESongCorrupt:
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: ESongCorrupt");
            break;
            }
        case TMPXPlaybackMessage::ESongContainerChanged: // Need to call MMPXMedia::CollectionPathL() to get new path
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: ESongContainerChanged");
            break;
            }
        case TMPXPlaybackMessage::EInitializeComplete: // iData = ETrue if init was to start playback: else EFalse.
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EInitializeComplete ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: data (start playback): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::ESongChanged: // iData=new index; need to query to get new properties (e.g. duration: title etc.)
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: ESongChanged ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: data: %d", data);
            break;
            }
        case TMPXPlaybackMessage::EPlayerChanged:
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EPlayerChanged ------>");
            
            MMPXPlayer* player = iPlaybackUtility->PlayerManager().CurrentPlayer();

            TUid pluginUid( KNullUid );
            RArray<TUid> array;

            CleanupClosePushL( array );

            if ( player )
                {
                pluginUid = player->UidL();
                array.AppendL( pluginUid );
                VCXLOGLO1("CFusionMpxPlayerTester:: UID added for ActivateViewL");
                }
            
            TRAPD( err, iViewUtility->ActivateViewL( array ) );
            if( err != KErrNone )
                {
                VCXLOGLO2("CFusionMpxPlayerTester:: ActivateViewL left: %d", err);
                }
            
            CleanupStack::PopAndDestroy( &array );            
            break;
            }
        case TMPXPlaybackMessage::EActivePlayerChanged: // iData=active/inactive (ETrue/EFalse); iType (ETrue:rebind to  ActivePlayerMode)
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EActivePlayerChanged ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: type (rebind): %d", type);
            VCXLOGLO2("CFusionMpxPlayerTester:: data (active): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::ESubPlayersChanged: // Need to query to get new list of sub players
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: ESubPlayersChanged");
            break;
            }
        case TMPXPlaybackMessage::EPlayerSelectionChanged: // Need to query MMPXPlayerManager::GetSelectionL()
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EPlayerSelectionChanged");
            break;
            }
        case TMPXPlaybackMessage::EDownloadStarted: // iData=total expected bytes
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EDownloadStarted ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: data (expected bytes): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::EDownloadUpdated: // iData=total bytes so far
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EDownloadUpdated ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: type (rebind): %d", type);
            VCXLOGLO2("CFusionMpxPlayerTester:: data (bytes downloaded): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::EDownloadComplete: // iData=total bytes downloaded
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EDownloadComplete ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: data (bytes downloaded): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::EDownloadPositionChanged: // iData=total bytes so far
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EDownloadPositionChanged ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: data (bytes downloaded): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::EDownloadStateChanged: // iData=TMPXPlaybackPdDownloadState
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EDownloadStateChanged ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: data (download state): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::EDownloadCmdPauseDownload: // iData=transaction ID
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EDownloadCmdPauseDownload ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: data (id): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::EDownloadCmdResumeDownload: // iData=transaction ID
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EDownloadCmdResumeDownload ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: data (id): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::EDownloadCmdCancelDownload: // iData=transaction ID
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EDownloadCmdCancelDownload ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: data (id): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::EAccessoryChanged: // iType=TAccessoryMode
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EAccessoryChanged ------>");
            VCXLOGLO2("CFusionMpxPlayerTester:: type (accessory type): %d", type);
            break;
            }
        case TMPXPlaybackMessage::EMediaChanged: // When the song/playlist hasn't: but data may become available
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EMediaChanged ------>");
            break;
            }
        case TMPXPlaybackMessage::ESkipping: //skipping event: iData=+1: skipping forward: -1: skipping backward
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: ESkipping");
            VCXLOGLO2("CFusionMpxPlayerTester:: data (skipping): %d", data);                
            break;
            }
        case TMPXPlaybackMessage::ESkipEnd: //skipping end
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: ESkipEnd");
            break;
            }
        case TMPXPlaybackMessage::EPlayerUnavailable:
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EPlayerUnavailable");
            break;
            }
        case TMPXPlaybackMessage::EPlaylistUpdated: // playlist updated
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EPlaylistUpdated");
            break;
            }
        case TMPXPlaybackMessage::EReachedEndOfPlaylist: // playback or skipping forward has reached end of playlist
            {
            VCXLOGLO1("CFusionMpxPlayerTester::DoHandlePlaybackMessageL: EReachedEndOfPlaylist");
            break;
            }

        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::DoHandleVideoPlaybackMessageL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::DoHandleVideoPlaybackMessageL( const CMPXMessage& aMessage )
    {
    VCXLOGLO1("CFusionMpxPlayerTester:: ----------------------------------");
    VCXLOGLO1("CFusionMpxPlayerTester::  KMPXMediaIdVideoPlayback message ");
    VCXLOGLO1("CFusionMpxPlayerTester:: ----------------------------------");
    
    TMPXVideoPlaybackCommand message =
        ( *(aMessage.Value<TMPXVideoPlaybackCommand>(KMPXMediaVideoPlaybackCommand)) );
    
    switch ( message )
        {
        case EPbCmdPluginError:
            {
            TInt error( *aMessage.Value<TInt>( KMPXMediaVideoError ) );
            VCXLOGLO2("CFusionMpxPlayerTester:: EPbCmdPluginError: %d", error);
            }
            break;
            
        case EPbCmdTvOutEvent:
            {
            TBool connected( *aMessage.Value<TInt>( KMPXMediaVideoTvOutConnected ) );
            TBool playbackAllowed( EFalse );
            if( connected )
                {
                TBool playbackAllowed = *aMessage.Value<TInt>( KMPXMediaVideoTvOutPlayAllowed );
                }
            VCXLOGLO3("CFusionMpxPlayerTester:: EPbCmdTvOutEvent: connected: %d, playback allowed: %d", connected, playbackAllowed);
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::DoHandleStateChangeL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::DoHandleStateChangeL( TMPXPlaybackState aState )
    {
    iState = aState;
    
    switch ( aState )
        {
        case EPbStateNotInitialised:
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStateNotInitialised ------>");            
            }
            break;            
        case EPbStateInitialising:   
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStateInitialising ------>");
            }
            break;
        case EPbStatePlaying:        
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStatePlaying ------>");
            }
            break;
        case EPbStatePaused:         
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStatePaused ------>");
            }
            break;
        case EPbStateStopped:        
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStateStopped ------>");
            }
            break;
        case EPbStateSeekingForward: 
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStateSeekingForward ------>");
            }
            break;
        case EPbStateSeekingBackward:
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStateSeekingBackward ------>");
            }
            break;
        case EPbStateShuttingDown:   
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStateShuttingDown ------>");
            }
            break;
        case EPbStateBuffering:      
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStateBuffering ------>");
            }
            break;
        case EPbStateDownloading:    
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStateDownloading ------>");
            }
            break;
        case EPbStateInitialised:        
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStateInitialised ------>");
            iInitComplete = ETrue;
            }
            break;
        case EPbStatePluginSeeking:   
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: EPbStatePluginSeeking ------>");
            }
            break;
        default:
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: state: UKNOWN! ------>");
            }
            break;
        }    
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::DoHandlePropertyChangeL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::DoHandlePropertyChangeL( TInt aProperty, TInt aValue )
    {
    switch( static_cast<TMPXPlaybackProperty>( aProperty ) )
        {
        case EPbPropertyVolume:    // 0 volume level values 0-KPbPlaybackVolumeLevelMax
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyVolume: %d ------>", aValue);
            iVolume = aValue;
            }
            break;
        case EPbPropertyMaxVolume: // 1 read only, KPbPlaybackVolumeLevelMax
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyMaxVolume: %d ------>", aValue);
            }
            break;
        case EPbPropertyVolumeRamp:// 2
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyVolumeRamp: %d ------>", aValue);
            }
            break;
        case EPbPropertyMute:      // 3 values 0(normal), 1(muted)
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyMute: %d ------>", aValue);
            }
            break;
        case EPbPropertyBalance:   // 4 values -100(left) - 0 - +100(right)
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyBalance: %d ------>", aValue);
            }
            break;
        case EPbPropertyEmbeddedMode:// 5
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyEmbeddedMode: %d ------>", aValue);
            }
            break;
        case EPbPropertyCrossFade:   // 6
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyCrossFade: %d ------>", aValue);
            }
            break;
        case EPbPropertyRandomMode:  // 7
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyRandomMode: %d ------>", aValue);
            }
            break;
        case EPbPropertyRepeatMode:  // 8
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyRepeatMode: %d ------>", aValue);
            }
            break;
        case EPbPropertyAccessPoint: // 9
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyAccessPoint: %d ------>", aValue);
            }
            break;
        case EPbPropertyPosition:    // 10 depends on the plugin, it's safer to pause
                                // the playing item before setting position
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyPosition: %d ------>", aValue);
            }
            break;
        case EPbPropertyDuration:    // 11 read only, millisecond
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyDuration: %d ------>", aValue);
            }
            break;
        case EPbPropertySongValid:   // 12
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertySongValid: %d ------>", aValue);
            }
            break;
        case EPbPropertyRemote:      // 13
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyRemote: %d ------>", aValue);
            }
            break;
        case EPbPropertySupportedFeatures:// 14 read only
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertySupportedFeatures: %d ------>", aValue);
            }
            break;
        case EPbPropertyNum:               // 15 Last
            {
            VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyNum: %d ------>", aValue);
            }
            break;
        
        default:
            {
            VCXLOGLO1("CFusionMpxPlayerTester:: property: UNKNOWN! ------>");
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::HandleViewActivation
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::HandleViewActivation( const TUid& /* aCurrentViewType */, const TUid& /* aPreviousViewType */ )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::HandleViewActivation ------>");
    VCXLOGLO1("<<<CFusionMpxPlayerTester::HandleViewActivation <------");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::HandleViewUpdate
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::HandleViewUpdate( TUid /* aViewUid */, TViewUpdateEvent /* aUpdateEvent */, 
        TBool /* aLoaded */, TInt /* aData */ )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::DoHandlePlaybackMessageL ------>");
    VCXLOGLO1("<<<CFusionMpxPlayerTester::DoHandlePlaybackMessageL <------");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::HandleViewUpdate
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::HandlePropertyL( TMPXPlaybackProperty aProperty,
                                     TInt aValue, 
                                     TInt aError )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTester::HandlePropertyL ------>");
    if( aError == KErrNone )
        {
        switch( aProperty )
            {
            case EPbPropertyVolume:    // 0 volume level values 0-KPbPlaybackVolumeLevelMax
                {
                VCXLOGLO2("CFusionMpxPlayerTester:: property: EPbPropertyVolume: %d ------>", aValue);
                iVolume = aValue;
                }
                break;
            default:
                break;
            }    
        }
    
    iWait->Stop();
    VCXLOGLO1("<<<CFusionMpxPlayerTester::HandlePropertyL ------>");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::HandleViewUpdate
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::HandleSubPlayerNamesL(TUid aPlayer, 
                           const MDesCArray* aSubPlayers,
                           TBool aComplete,
                           TInt aError)
    {
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::HandleViewUpdate
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::HandleMediaL(const CMPXMedia& aProperties,
                                  TInt aError)
    {
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::HandleViewUpdate
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::HandlePlaybackCommandComplete(CMPXCommand* /*aCommandResult*/, 
                                                  TInt /*aError*/)
    {
    }

// EOF
