/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>

#include "FusionMpxPlayerTester.h"

#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackframeworkdefs.h>
#include <mpxviewutility.h>

#include "mpxmediavideodefs.h"
#include <mpxvideoplaybackdefs.h>

// -------------------------------------------------------------------------------------------------
// CFusionMpxPlayerTester::GetGeneralPlaybackMessageDes
// -------------------------------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::GetGeneralPlaybackCommandDes( TMPXPlaybackCommand aCmd, TDes& aDes )
    {
    switch( aCmd )
        {
        case EPbCmdPlay: // 0 Play which ever track is currently stopped, paused etc.
            {
            aDes.Copy( _L("EPbCmdPlay" ) );
            }
            break;
        case EPbCmdPlayWithFadeIn: // 1 uses KPbFadeInDuration
            {
            aDes.Copy( _L("EPbCmdPlayWithFadeIn" ) );
            }
            break;
        case EPbCmdPause:    // 2
            {
            aDes.Copy( _L("EPbCmdPause" ) );
            }
            break;
        case EPbCmdStop:     // 3
            {
            aDes.Copy( _L("EPbCmdStop" ) );
            }
            break;
        case EPbCmdNext:     // 4 No effect if no next track
            {
            aDes.Copy( _L("EPbCmdNext" ) );
            }
            break;
        case EPbCmdPrevious: // 5 No effect if no previous track;
                        //   if pos>KPbReplayThreshold, replays current track
            {
            aDes.Copy( _L("EPbCmdPrevious" ) );
            }
            break;
        case EPbCmdReplay:   // 6 Restart current track at beginning
            {
            aDes.Copy( _L("EPbCmdReplay" ) );
            }
            break;
        case EPbCmdStartSeekForward:  // 7
            {
            aDes.Copy( _L("EPbCmdStartSeekForward" ) );
            }
            break;
        case EPbCmdStartSeekBackward: // 8
            {
            aDes.Copy( _L("EPbCmdStartSeekBackward" ) );
            }
            break;
        case EPbCmdStopSeeking:       // 9
            {
            aDes.Copy( _L("EPbCmdStopSeeking" ) );
            }
            break;
        case EPbCmdIncreaseVolume:    // 10
            {
            aDes.Copy( _L("EPbCmdIncreaseVolume" ) );
            }
            break;
        case EPbCmdDecreaseVolume:    // 11
            {
            aDes.Copy( _L("EPbCmdDecreaseVolume" ) );
            }
            break;
        case EPbCmdMuteVolume:        // 12
            {
            aDes.Copy( _L("EPbCmdMuteVolume" ) );
            }
            break;
        case EPbCmdUnMuteVolume:      // 13
            {
            aDes.Copy( _L("EPbCmdUnMuteVolume" ) );
            }
            break;
        case EPbCmdClose:             // 14
            {
            aDes.Copy( _L("EPbCmdClose" ) );
            }
            break;
        case EPbApplyEffect:          // 15 Apply an effect to the current playback,
                                 //    aData used to identify effect
            {
            aDes.Copy( _L("EPbApplyEffect" ) );
            }
            break;
        case EPbCmdPreservePosition:  // 16 Preserve playback position
            {
            aDes.Copy( _L("EPbCmdPreservePosition" ) );
            }
            break;
        case EPbCmdPreserveState:     // 17 Preserve playback state
            {
            aDes.Copy( _L("EPbCmdPreserveState" ) );
            }
            break;
        case EPbCmdCloseItem:         // 18 Request to close the file handle
            {
            aDes.Copy( _L("EPbCmdCloseItem" ) );
            }
            break;
        case EPbCmdPlayPause:         // 19 Play/Pause toggle command
            {
            aDes.Copy( _L("EPbCmdPlayPause" ) );
            }
            break;
        case EPbCmdCancelInit:         // 20
            {
            aDes.Copy( _L("EPbCmdCancelInit" ) );
            }
            break;
        case EPbCmdResetPreserveState: // 21
            {
            aDes.Copy( _L("EPbCmdResetPreserveState" ) );
            }
            break;
        case EPbCmdUnloadNonActivePlugin: // 22 KMPXCommandPlaybackGeneralData is TUid.iUid
            {
            aDes.Copy( _L("EPbCmdUnloadNonActivePlugin" ) );
            }
            break;
        case EPbCmdClearKeyBuffer:        // 23 Clear all media key commands
            {
            aDes.Copy( _L("EPbCmdClearKeyBuffer" ) );
            }
            break;
        case EPbCmdDisableEffect:
            {
            aDes.Copy( _L("EPbCmdDisableEffect" ) );
            }
            break;
        case EPbCmdSetVolume:             // 25 Set volume to the value specified by aData
            {
            aDes.Copy( _L("EPbCmdSetVolume" ) );
            }
            break;
        case EPbCmdEnd:                    // Mark for the end of command        
            {
            aDes.Copy( _L("EPbCmdEnd" ) );
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::GetGeneralPlaybackMessageDes
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::GetGeneralPlaybackEventDes( TInt aEvent, TDes& aDes )
    {
    switch ( aEvent )
        {
        case TMPXPlaybackMessage::ENoEvent:
            {
            aDes.Copy( _L("ENoEvent" ) );
            break;
            }
        case TMPXPlaybackMessage::EError: // EError must be 1. iData is error originating from player: iType=event type
            {
            aDes.Copy( _L("EError" ) );
            break;
            }
        case TMPXPlaybackMessage::ECommandReceived: // iType=TMPXPlaybackCommand
            {
            aDes.Copy( _L("ECommandReceived" ) );
            break;
            }
        case TMPXPlaybackMessage::ECommandComplete: // iType=TMPXPlaybackCommand
            {
            aDes.Copy( _L("ECommandComplete" ) );
            break;
            }
        case TMPXPlaybackMessage::EPropertyChanged: // iType=TMPXPlaybackProperty; iData=new value
            {
            aDes.Copy( _L("EPropertyChanged" ) );
            break;
            }
        case TMPXPlaybackMessage::EStateChanged: // iType=TMPXPlaybackState
            {
            aDes.Copy( _L("EStateChanged" ) );
            break;
            }
        case TMPXPlaybackMessage::ESongCorrupt:
            {
            aDes.Copy( _L("ESongCorrupt" ) );
            break;
            }
        case TMPXPlaybackMessage::ESongContainerChanged: // Need to call MMPXMedia::CollectionPathL() to get new path
            {
            aDes.Copy( _L("ESongContainerChanged" ) );
            break;
            }
        case TMPXPlaybackMessage::EInitializeComplete: // iData = ETrue if init was to start playback: else EFalse.
            {
            aDes.Copy( _L("EInitializeComplete" ) );                
            break;
            }
        case TMPXPlaybackMessage::ESongChanged: // iData=new index; need to query to get new properties (e.g. duration: title etc.)
            {
            aDes.Copy( _L("ESongChanged" ) );
            break;
            }
        case TMPXPlaybackMessage::EPlayerChanged: // Need to call MMPXPlaybackUtility::CurrentPlayer() to get new features
            {
            aDes.Copy( _L("EPlayerChanged" ) );
            break;
            }
        case TMPXPlaybackMessage::EActivePlayerChanged: // iData=active/inactive (ETrue/EFalse) ); iType (ETrue:rebind to  ActivePlayerMode)
            {
            aDes.Copy( _L("EActivePlayerChanged" ) );                
            break;
            }
        case TMPXPlaybackMessage::ESubPlayersChanged: // Need to query to get new list of sub players
            {
            aDes.Copy( _L("ESubPlayersChanged" ) );
            break;
            }
        case TMPXPlaybackMessage::EPlayerSelectionChanged: // Need to query MMPXPlayerManager::GetSelectionL()
            {
            aDes.Copy( _L("EPlayerSelectionChanged" ) );
            break;
            }
        case TMPXPlaybackMessage::EDownloadStarted: // iData=total expected bytes
            {
            aDes.Copy( _L("EDownloadStarted" ) );                
            break;
            }
        case TMPXPlaybackMessage::EDownloadUpdated: // iData=total bytes so far
            {
            aDes.Copy( _L("EDownloadUpdated" ) );                
            break;
            }
        case TMPXPlaybackMessage::EDownloadComplete: // iData=total bytes downloaded
            {
            aDes.Copy( _L("EDownloadComplete" ) );                
            break;
            }
        case TMPXPlaybackMessage::EDownloadPositionChanged: // iData=total bytes so far
            {
            aDes.Copy( _L("EDownloadPositionChanged" ) );                
            break;
            }
        case TMPXPlaybackMessage::EDownloadStateChanged: // iData=TMPXPlaybackPdDownloadState
            {
            aDes.Copy( _L("EDownloadStateChanged" ) );                
            break;
            }
        case TMPXPlaybackMessage::EDownloadCmdPauseDownload: // iData=transaction ID
            {
            aDes.Copy( _L("EDownloadCmdPauseDownload" ) );                
            break;
            }
        case TMPXPlaybackMessage::EDownloadCmdResumeDownload: // iData=transaction ID
            {
            aDes.Copy( _L("EDownloadCmdResumeDownload" ) );                
            break;
            }
        case TMPXPlaybackMessage::EDownloadCmdCancelDownload: // iData=transaction ID
            {
            aDes.Copy( _L("EDownloadCmdCancelDownload" ) );                
            break;
            }
        case TMPXPlaybackMessage::EAccessoryChanged: // iType=TAccessoryMode
            {
            aDes.Copy( _L("EAccessoryChanged" ) );
            break;
            }
        case TMPXPlaybackMessage::EMediaChanged: // When the song/playlist hasn't: but data may become available
            {
            aDes.Copy( _L("EMediaChanged" ) );
            break;
            }
        case TMPXPlaybackMessage::ESkipping: //skipping event: iData=+1: skipping forward: -1: skipping backward
            {
            aDes.Copy( _L("ESkipping" ) );                
            break;
            }
        case TMPXPlaybackMessage::ESkipEnd: //skipping end
            {
            aDes.Copy( _L("ESkipEnd" ) );
            break;
            }
        case TMPXPlaybackMessage::EPlayerUnavailable:
            {
            aDes.Copy( _L("EPlayerUnavailable" ) );
            break;
            }
        case TMPXPlaybackMessage::EPlaylistUpdated: // playlist updated
            {
            aDes.Copy( _L("EPlaylistUpdated" ) );
            break;
            }
        case TMPXPlaybackMessage::EReachedEndOfPlaylist: // playback or skipping forward has reached end of playlist
            {
            aDes.Copy( _L("EReachedEndOfPlaylist" ) );
            break;
            }

        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::GetVideoPlaybackCommandDes
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::GetVideoPlaybackCommandDes( TMPXVideoPlaybackCommand aCmd, TDes& aDes )
    {
    switch ( aCmd )
        {
        case EPbCmdInitView:
            {
            aDes.Copy( _L("EPbCmdInitView") );
            }
            break;
        case EPbCmdAbortDSA:
            {
            aDes.Copy( _L("EPbCmdAbortDSA") );
            }
            break;
        case EPbCmdRestartDSA:
            {
            aDes.Copy( _L("EPbCmdRestartDSA") );
            }
            break;
        case EPbCmdSetDisplayWindow:
            {
            aDes.Copy( _L("EPbCmdSetDisplayWindow") );
            }
            break;
        case EPbCmdStartVideoSeekingForward:
            {
            aDes.Copy( _L("EPbCmdStartVideoSeekingForward") );
            }
            break;
        case EPbCmdStartVideoSeekingBackward:
            {
            aDes.Copy( _L("EPbCmdStartVideoSeekingBackward") );
            }
            break;
        case EPbCmdStopVideoSeeking:
            {
            aDes.Copy( _L("EPbCmdStopVideoSeeking") );
            }
            break;
        case EPbCmdHandleForeground:
            {
            aDes.Copy( _L("EPbCmdHandleForeground") );
            }
            break;
        case EPbCmdHandleBackground:
            {
            aDes.Copy( _L("EPbCmdHandleBackground") );
            }
            break;
        case EPbCmdNaturalAspectRatio:
            {
            aDes.Copy( _L("EPbCmdNaturalAspectRatio") );
            }
            break;
        case EPbCmdZoomAspectRatio:
            {
            aDes.Copy( _L("EPbCmdZoomAspectRatio") );
            }
            break;
        case EPbCmdStretchAspectRatio:
            {
            aDes.Copy( _L("EPbCmdStretchAspectRatio") );
            }
            break;
        case EPbCmdSetDefaultAspectRatio:
            {
            aDes.Copy( _L("EPbCmdSetDefaultAspectRatio") );
            }
            break;
        case EPbCmdTvOutEvent:
            {
            aDes.Copy( _L("EPbCmdTvOutEvent") );
            }
            break;
        case EPbCmdPluginError:
            {
            aDes.Copy( _L("EPbCmdPluginError") );
            }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::GetStateDes
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::GetStateDes( TMPXPlaybackState aState, TDes& aDes )
    {
    switch ( aState )
        {
        case EPbStateNotInitialised:
            {            
            aDes.Copy( _L("EPbStateNotInitialised") );            
            }
            break;            
        case EPbStateInitialising:   
            {
            aDes.Copy( _L("EPbStateInitialising") );
            }
            break;
        case EPbStatePlaying:        
            {
            aDes.Copy( _L("EPbStatePlaying") );
            }
            break;
        case EPbStatePaused:         
            {
            aDes.Copy( _L("EPbStatePaused") );
            }
            break;
        case EPbStateStopped:        
            {
            aDes.Copy( _L("EPbStateStopped") );
            }
            break;
        case EPbStateSeekingForward: 
            {
            aDes.Copy( _L("EPbStateSeekingForward") );
            }
            break;
        case EPbStateSeekingBackward:
            {
            aDes.Copy( _L("EPbStateSeekingBackward") );
            }
            break;
        case EPbStateShuttingDown:   
            {
            aDes.Copy( _L("EPbStateShuttingDown") );
            }
            break;
        case EPbStateBuffering:      
            {
            aDes.Copy( _L("EPbStateBuffering") );
            }
            break;
        case EPbStateDownloading:    
            {
            aDes.Copy( _L("EPbStateDownloading") );
            }
            break;
        case EPbStateInitialised:        
            {
            aDes.Copy( _L("EPbStateInitialised") );
            }
            break;
        case EPbStatePluginSeeking:   
            {
            aDes.Copy( _L("EPbStatePluginSeeking") );
            }
            break;
        }    
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTester::GetPropertyDes
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTester::GetPropertyDes( TMPXPlaybackProperty aProperty, TDes& aDes )
    {
    switch( aProperty )
        {
        case EPbPropertyVolume:    // 0 volume level values 0-KPbPlaybackVolumeLevelMax
            {
            aDes.Copy( _L("EPbPropertyVolume") );
            }
            break;
        case EPbPropertyMaxVolume: // 1 read only, KPbPlaybackVolumeLevelMax
            {
            aDes.Copy( _L("EPbPropertyMaxVolume") );
            }
            break;
        case EPbPropertyVolumeRamp:// 2
            {
            aDes.Copy( _L("EPbPropertyVolumeRamp") );
            }
            break;
        case EPbPropertyMute:      // 3 values 0(normal), 1(muted)
            {
            aDes.Copy( _L("EPbPropertyMute") );
            }
            break;
        case EPbPropertyBalance:   // 4 values -100(left) - 0 - +100(right)
            {
            aDes.Copy( _L("EPbPropertyBalance") );
            }
            break;
        case EPbPropertyEmbeddedMode:// 5
            {
            aDes.Copy( _L("EPbPropertyEmbeddedMode") );
            }
            break;
        case EPbPropertyCrossFade:   // 6
            {
            aDes.Copy( _L("EPbPropertyCrossFade") );
            }
            break;
        case EPbPropertyRandomMode:  // 7
            {
            aDes.Copy( _L("EPbPropertyRandomMode") );
            }
            break;
        case EPbPropertyRepeatMode:  // 8
            {
            aDes.Copy( _L("EPbPropertyRepeatMode") );
            }
            break;
        case EPbPropertyAccessPoint: // 9
            {
            aDes.Copy( _L("EPbPropertyAccessPoint") );
            }
            break;
        case EPbPropertyPosition:    // 10 depends on the plugin, it's safer to pause
                                // the playing item before setting position
            {
            aDes.Copy( _L("EPbPropertyPosition") );
            }
            break;
        case EPbPropertyDuration:    // 11 read only, millisecond
            {
            aDes.Copy( _L("EPbPropertyDuration") );
            }
            break;
        case EPbPropertySongValid:   // 12
            {
            aDes.Copy( _L("EPbPropertySongValid") );
            }
            break;
        case EPbPropertyRemote:      // 13
            {
            aDes.Copy( _L("EPbPropertyRemote") );
            }
            break;
        case EPbPropertySupportedFeatures:// 14 read only
            {
            aDes.Copy( _L("EPbPropertySupportedFeatures") );
            }
            break;
        case EPbPropertyNum:               // 15 Last
            {
            aDes.Copy( _L("EPbPropertyNum") );
            }
            break;
        }
    }
