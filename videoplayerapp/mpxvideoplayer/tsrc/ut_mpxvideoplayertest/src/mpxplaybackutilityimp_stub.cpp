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
* Description:  mpx playback utility stub for STIF testing
*
*/

// Version : %version: ou1cpsw#9.1.1 %


#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxmediageneralextdefs.h>

#include "mpxplaybackutilityimp_stub.h"
#include "mpxvideoplayerconstants.h"
#include "stifutilities.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideo_debug.h"


MMPXPlaybackUtility* MMPXPlaybackUtility::UtilityL( const TMPXCategory aCategory, const TUid& aModeId )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::UtilityL()"));

    StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

    if ( ! utility )
    {
        utility = new (ELeave) StifUtilities;

        Dll::SetTls( utility );
    }

    if ( ! utility->iPlaybackUtility )
    {
        utility->iPlaybackUtility = new (ELeave) CMPXPlaybackUtility();
    }

    utility->iPlaybackUtility->AddRefCount();

    //
    //  Send notification to observer
    //
    utility->iPlaybackUtility->StartPdlPlaybackUtility();

    return utility->iPlaybackUtility;
}

CMPXPlaybackUtility::~CMPXPlaybackUtility()
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::~CMPXPlaybackUtility()"));

    iObservers.Close();
}

CMPXPlaybackUtility::CMPXPlaybackUtility()
{
}

void CMPXPlaybackUtility::AddRefCount()
{
    iRefCount++;
}

void CMPXPlaybackUtility::StartPdlPlaybackUtility()
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::StartPdlPlaybackUtility()"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent    = EPlaybackUtilityPdlInstance;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXPlaybackUtility::AddStifObserverL( MAppUiEngineStifTestObserver* aObserver )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::AddStifObserverL()"));

    iStifObserver= aObserver;
}

void CMPXPlaybackUtility::RemoveStifObserverL()
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::RemoveStifObserverL()"));

    iStifObserver = NULL;
}

// -------------------------------------------------------------------------------------------------
//   MMPXPlaybackUtility Implementation
// -------------------------------------------------------------------------------------------------

void CMPXPlaybackUtility::AddObserverL( MMPXPlaybackObserver& aObs )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::AddObserverL()"));
    iObservers.AppendL(&aObs);
}

void CMPXPlaybackUtility::RemoveObserverL( MMPXPlaybackObserver& aObs )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::RemoveObserverL()"));

    TInt i = iObservers.FindL(&aObs);
    iObservers.Remove(i);
}


void CMPXPlaybackUtility::GetClientsL( RArray<TProcessId>& /*aClients*/ )
{
}

void CMPXPlaybackUtility::UpdatePlaylistL( const CMPXCollectionPlaylist& /*aPlaylist*/ )
{
}

void CMPXPlaybackUtility::InitL( const CMPXCollectionPlaylist& /*aPlaylist*/, TBool /*aPlay*/ )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitL(CMPXCollectionPlaylist)"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EPlaybackUtilityInitPlaylist;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXPlaybackUtility::InitL( const TDesC& aUri, const TDesC8* /*aType*/ )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitL()"),
                   _L("aUri = %S"), &aUri );

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent    = EPlaybackUtilityInitFileName;
        event->iFileName = aUri;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXPlaybackUtility::InitL( RFile& aShareableFile )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitL( RFile )"));

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent = EPlaybackUtilityInitFileHandle;
        aShareableFile.FullName( event->iFileName );
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXPlaybackUtility::Close()
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::Close()"),
                   _L("iRefCount = %d"), iRefCount );

    //
    //  When the ref count is 2, it should be the app ui engine closing the utility
    //
    if ( iRefCount == 2 )
    {
        if ( iStifObserver )
        {
            TCallbackEvent* event = new TCallbackEvent;
            event->iEvent = EPlaybackUtilityClose;
            iStifObserver->HandleUtilityEvent( event );
        }
    }

    if ( --iRefCount == 0 )
    {
        StifUtilities* utility = reinterpret_cast<StifUtilities*>( Dll::Tls() );

        utility->DeletePlaybackUtility();
    }
}

void CMPXPlaybackUtility::CancelRequest()
{
}

void CMPXPlaybackUtility::CommandL( TMPXPlaybackCommand aCmd, TInt /*aData*/ )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::CommandL()"),
                   _L("aCmd = %d"), aCmd );

    switch ( aCmd )
    {
        case EPbCmdPlay:
        {
            if ( iStifObserver )
            {
                TCallbackEvent* event = new TCallbackEvent;
                event->iEvent = EPlaybackUtilityPlayCmd;
                iStifObserver->HandleUtilityEvent( event );
            }

            break;
        }
        case EPbCmdClose:
        {
            if ( iStifObserver )
            {
                TCallbackEvent* event = new TCallbackEvent;
                event->iEvent = EPlaybackUtilityClose;
                iStifObserver->HandleUtilityEvent( event );
            }

            break;
        case EPbCmdDisableEffect:
            {
            iDisableEffectCallCount++;
            if( iDisableEffectCallCount == 1  )
                {
                if ( iStifObserver )
                    {
                    TCallbackEvent* event = new TCallbackEvent;
                    event->iEvent = EPlaybackUtilityDisableEffects;
                    iStifObserver->HandleUtilityEvent( event );
                    }                
                }            
            if( iDisableEffectCallCount >= 2 )
                {
                User::Leave( KErrNotFound );
                }                        
            }
            break;
        }
    }
}

void CMPXPlaybackUtility::CommandL( CMPXCommand& aCmd, MMPXPlaybackCallback* /*aCallback*/ )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::CommandL( CMPXCommand )"));

    if ( aCmd.IsSupported( KMPXCommandGeneralId ) )
    {
        TInt cmdId = *aCmd.Value<TInt>(KMPXCommandGeneralId);

        MPX_DEBUG(_L("CMPXPlaybackUtility::CommandL() id = 0x%08x"), cmdId );

        switch ( cmdId )
        {
            case KMPXCommandIdPlaybackGeneral:
            {
                break;
            }
            case KMPXCommandIdPlaybackPD:
            {
                TMPXPlaybackPdCommand pdCmd =
                    static_cast<TMPXPlaybackPdCommand>(
                        *aCmd.Value<TInt>(KMPXCommandPlaybackGeneralType) );

                switch ( pdCmd )
                {
                    case EPbCmdStartPd:
                    {
                        TInt downloadId = *aCmd.Value<TInt>( KMPXCommandPlaybackPDTransactionID );
                        TPtrC fileName( aCmd.ValueText( KMPXMediaVideoPlaybackFileName ) );
                        TInt movePdlFile = *aCmd.Value<TInt>( KMPXMediaVideoMovePdlFile );

                        if ( iStifObserver )
                        {
                            TCallbackEvent* event = new TCallbackEvent;
                            event->iEvent      = EPlaybackUtilityStartPdl;
                            event->iDownloadId = downloadId;
                            event->iFileName   = fileName;
                            event->iExtra      = movePdlFile;
                            iStifObserver->HandleUtilityEvent( event );
                        }

                        break;
                    }
                }
                break;
            }
            case KMPXMediaIdVideoPlayback:
            {
                TMPXVideoPlaybackCommand videoCmd =
                        static_cast<TMPXVideoPlaybackCommand>(
                            *aCmd.Value<TInt>(KMPXMediaVideoPlaybackCommand) );

                switch ( videoCmd )
                {
                    case EPbCmdUpdateSeekable:
                    {
                        TBool seekable = *aCmd.Value<TBool>( KMPXMediaGeneralExtVideoSeekable );

                        if ( iStifObserver )
                        {
                            TCallbackEvent* event = new TCallbackEvent;
                            event->iEvent      = EAsxAttributeSeekUpdate;
                            event->iExtra      = seekable;
                            iStifObserver->HandleUtilityEvent( event );
                        }
                        break;
                    }
                }

                break;
            }

        }
    }
}

TMPXPlaybackState CMPXPlaybackUtility::StateL() const
{
    return EPbStateNotInitialised;
}

MMPXSource* CMPXPlaybackUtility::Source()
{
    return this;
}

MMPXPlayerManager& CMPXPlaybackUtility::PlayerManager()
{
    return *this;
}

void CMPXPlaybackUtility::SetL( TMPXPlaybackProperty /*aProperty*/, TInt /*aValue*/ )
{
}

void CMPXPlaybackUtility::ValueL( MMPXPlaybackCallback& /*aCallback*/,
                                  TMPXPlaybackProperty /*aProperty*/ )
{
}

void CMPXPlaybackUtility::PropertyL( MMPXPlaybackCallback& /*aCallback*/,
                                   TMPXPlaybackProperty /*aProperty*/ )
{
}

CDesCArray* CMPXPlaybackUtility::SupportedMimeTypes()
{
    return NULL;
}

CDesCArray* CMPXPlaybackUtility::SupportedExtensions()
{
    return NULL;
}

CDesCArray* CMPXPlaybackUtility::SupportedSchemas()
{
    return NULL;
}

void CMPXPlaybackUtility::SetPriority( TInt /*aPriority*/ )
{
}

void CMPXPlaybackUtility::AddSubscriptionL( const CMPXSubscription& /*aSubscription*/ )
{
}

void CMPXPlaybackUtility::RemoveSubscriptionL( const CMPXSubscription& /*aSubscription*/ )
{
}

void CMPXPlaybackUtility::ClearSubscriptionsL()
{
}

void CMPXPlaybackUtility::InitStreamingL( const TDesC& aUri,
                                          const TDesC8* /*aType*/,
                                          const TInt aAccessPoint )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitStreamingL()"),
                   _L("aUri = %S, aAccessPoint = %d"), &aUri, aAccessPoint );

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent    = EPlaybackUtilityInitStreamingUrl;
        event->iFileName = aUri;
        event->iApId     = aAccessPoint;
        iStifObserver->HandleUtilityEvent( event );
    }
}

void CMPXPlaybackUtility::InitStreamingL( RFile& aShareableFile, const TInt aAccessPoint )
{
    MPX_ENTER_EXIT(_L("CMPXPlaybackUtility::InitStreamingL( RFile )"),
                   _L("aAccessPoint = %d"), aAccessPoint );

    if ( iStifObserver )
    {
        TCallbackEvent* event = new TCallbackEvent;
        event->iEvent    = EPlaybackUtilityInitStreamingFileHandle;
        aShareableFile.FullName( event->iFileName );
        event->iApId     = aAccessPoint;
        iStifObserver->HandleUtilityEvent( event );
    }
}


// -------------------------------------------------------------------------------------------------
//   MMPXPlayerManager Implementation
// -------------------------------------------------------------------------------------------------

void CMPXPlaybackUtility::GetPlayerTypesL( RArray<TMPXPlaybackPlayerType>& /*aTypes*/ )
{
}

HBufC* CMPXPlaybackUtility::PlayerTypeDisplayNameL( TMPXPlaybackPlayerType /*aType*/ )
{
    return NULL;
}

void CMPXPlaybackUtility::GetPlayerListL( RArray<TUid>& /*aPlayers*/ )
{
}

void CMPXPlaybackUtility::GetPlayerListL( RArray<TUid>& /*aPlayers*/,
                                          TMPXPlaybackPlayerType /*aType*/ )
{
}

void CMPXPlaybackUtility::SubPlayerNamesL( MMPXPlaybackCallback& /*aCallback*/, TUid /*aPlayer*/ )
{
}

void CMPXPlaybackUtility::SelectPlayersL( TMPXPlaybackPlayerType /*aType*/ )
{
}

void CMPXPlaybackUtility::SelectSubPlayerL( TUid /*aPlayer*/, TInt /*aSubPlayerIndex*/ )
{
}

void CMPXPlaybackUtility::SelectPlayerL( TUid /*aPlayer*/ )
{
}

void CMPXPlaybackUtility::ClearSelectPlayersL()
{
}

void CMPXPlaybackUtility::GetSelectionL( TMPXPlaybackPlayerType& /*aType*/,
                                         TUid& /*aPlayer*/,
                                         TInt& /*aSubPlayerIndex*/,
                                         HBufC*& /*aSubPlayerName*/ )
{
}

MMPXPlayer* CMPXPlaybackUtility::CurrentPlayer()
{
    return this;
}


// -------------------------------------------------------------------------------------------------
//   MMPXPlayer Implementation
// -------------------------------------------------------------------------------------------------

TMPXPlaybackPlayerType CMPXPlaybackUtility::TypeL()
{
    return EPbUnknown;
}

HBufC* CMPXPlaybackUtility::TypeNameL()
{
    return NULL;
}

void CMPXPlaybackUtility::SubPlayerNamesL( MMPXPlaybackCallback& /*aCallback*/ )
{
}

TInt CMPXPlaybackUtility::SubPlayerL() const
{
    return 0;
}

TUid CMPXPlaybackUtility::UidL() const
{
    return KNullUid;
}


// -------------------------------------------------------------------------------------------------
//   MPXSource Implementation
// -------------------------------------------------------------------------------------------------

CMPXCollectionPlaylist* CMPXPlaybackUtility::PlaylistL()
{
    return NULL;
}

RFile* CMPXPlaybackUtility::FileL()
{
    return NULL;
}

HBufC* CMPXPlaybackUtility::UriL()
{
    return NULL;
}

void CMPXPlaybackUtility::MediaL( const TArray<TMPXAttribute>& /*aAttrs*/,
                                  MMPXPlaybackCallback& /*aCallback*/ )
{
}

void CMPXPlaybackUtility::MediaL( const TArray<TMPXAttribute>& /*aAttrs*/,
                                  MMPXPlaybackCallback& /*aCallback*/,
                                  CMPXAttributeSpecs* /*aSpecs*/ )
{
}

// End of file
