/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Playback Utility implementation
*
*/

// Version : %version: %

#include <bamdesca.h>
#include <s32mem.h>
#include <e32math.h>
#include <mpxcmn.h>
#include <mpxplaybackobserver.h>
#include <mpxcollectionplaylist.h>
#include <mpxmedia.h>
#include <mpxmessagemonitor.h>
#include <mpxtaskqueue.h>
#include <mpxuser.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectionpath.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxsubscription.h>
#include <mpxlog.h>

#include "mpxplaybackutilityimpl.h"

// ============================== MEMBER FUNCTIONS ============================

// ---------------------------------------------------------------------------
// Retrieves playback utility from TLS. Creates if it's not there.
// ---------------------------------------------------------------------------
//
MMPXPlaybackUtility* CMPXPlaybackUtility::UtilityL(
    const TUid& aModeId,
    const TMPXCategory aCategory)
    {
    MMPXPlaybackUtility* utility( NULL );
    
    utility = CMPXPlaybackUtility::NewL( aModeId, NULL, aCategory );

    return utility;
    }

// ----------------------------------------------------------------------------
// Two phases constructor
// ----------------------------------------------------------------------------
//
CMPXPlaybackUtility* CMPXPlaybackUtility::NewL(const TUid& aModeId,
                                               MMPXPlaybackObserver* aObs,
                                               const TMPXCategory aCategory)
    {
    CMPXPlaybackUtility* p=new(ELeave)CMPXPlaybackUtility();
    CleanupStack::PushL(p);
    p->ConstructL(aModeId,aObs,aCategory);
    CleanupStack::Pop(p);
    return p;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXPlaybackUtility::~CMPXPlaybackUtility()
    {
    iObservers.Close();
    }

// ----------------------------------------------------------------------------
// C++ constructor
// Create a unique name out of thread ID and this pointer: no other instance of
// this object will have the same name; used to identify this object for
// recieving messages
// ----------------------------------------------------------------------------
//
CMPXPlaybackUtility::CMPXPlaybackUtility()
    : iCallbackOngoing(EFalse)
    {
    }

// ----------------------------------------------------------------------------
// 2nd construtor
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::ConstructL(const TUid& /* aModeId */,
                                     MMPXPlaybackObserver* /* aObs */, 
                                     const TMPXCategory /* aCategory */)
    {
    	iState = EPbStateInitialised;
    }

// ----------------------------------------------------------------------------
// Add a observer
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::AddObserverL(MMPXPlaybackObserver& aObs)
    {
    iObservers.AppendL(&aObs);
    }

// ----------------------------------------------------------------------------
// Remove a observer
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::RemoveObserverL(MMPXPlaybackObserver& /* aObs */)
    {

    }

// ----------------------------------------------------------------------------
// Returns PIds of clients that are using the engine in the mode
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::GetClientsL(RArray<TProcessId>& /* aClients */)
    {

    }

// ----------------------------------------------------------------------------
// Initializes a track given by aIndex in path aCollectionPath
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::InitL(
    const CMPXCollectionPlaylist& /* aPlaylist */,
    TBool /*aPlay */ )
    {

    }

// ----------------------------------------------------------------------------
// Inititialises with a single song, may not be part of any collection
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::InitL(const TDesC& /* aUri */, const TDesC8* /* aType */ )
    {

    }

// ----------------------------------------------------------------------------
// Inititialises with a single song, may not be part of any collection
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::InitL(RFile& /* aShareableFile */)
    {

    }

// ----------------------------------------------------------------------------
// Inititialises with a URI
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::InitStreamingL(const TDesC& /* aUri */, const TDesC8* /* aType */, const TInt /* aAccessPoint */)
{

}


// ----------------------------------------------------------------------------
// Inititialises with a file, may not be part of any collection
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::InitStreamingL(RFile& /* aShareableFile */, const TInt /* aAccessPoint */)
{

}

// ----------------------------------------------------------------------------
// Frees up client side resources only; a player is freed when there are no
// clients using it, and all resources are freed when the last client closed
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::Close()
    {

    }

// ----------------------------------------------------------------------------
// Stops any async operations that are currently under way
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::CancelRequest()
    {

    }

// ----------------------------------------------------------------------------
// Issue player commands, with optional data.
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::CommandL(TMPXPlaybackCommand /* aCmd */, TInt /* aData */)
    {

    }

// ----------------------------------------------------------------------------
// Issue player commands
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::CommandL(
    CMPXCommand& aCmd,
    MMPXPlaybackCallback* /* aCallback */)
    {
    TMPXPlaybackCommand cmdType = static_cast<TMPXPlaybackCommand>(
                          aCmd.ValueTObjectL<TInt>(KMPXCommandPlaybackGeneralType));
        
    switch ( cmdType )
        {
        case EPbCmdPlay:
            {
            iState = EPbStatePlaying;
            break;
            }
        case EPbCmdDecreaseVolume:
            {
            CMPXMessage* message = CMPXMessage::NewL();
            message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
            message->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXPlaybackMessage::EPropertyChanged ); 
            message->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EPbPropertyVolume );
            message->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0 );                      
            iObservers[0]->HandlePlaybackMessage( message, KErrNone );            
            }
        }
    }

// ----------------------------------------------------------------------------
// Current state of player
// ----------------------------------------------------------------------------
//
TMPXPlaybackState CMPXPlaybackUtility::StateL() const
    {
    return iState;
    }

// ----------------------------------------------------------------------------
// Determine whether there is a song by the state of the engine: if there is,
// its OK to return MMPXMedia, else NULL is returned
// ----------------------------------------------------------------------------
//
MMPXSource* CMPXPlaybackUtility::Source()
    {
    return this;
    }

// ----------------------------------------------------------------------------
// Get player manager
// ----------------------------------------------------------------------------
//
MMPXPlayerManager& CMPXPlaybackUtility::PlayerManager()
    {
    }

// ----------------------------------------------------------------------------
// Set playback property, EPropertyChanged event when complete
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::SetL(TMPXPlaybackProperty aProperty,TInt /* aValue */ )
    {
        if ( aProperty == EPbPropertyMute )
        {
            CMPXMessage* message = CMPXMessage::NewL();
            message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
            message->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXPlaybackMessage::EPropertyChanged ); 
            message->SetTObjectValueL<TInt>( KMPXMessageGeneralType, 0 );
            message->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 0 );                      
            iObservers[0]->HandlePlaybackMessage( message, KErrNone );
        } 
        else if ( aProperty == EPbPropertyPosition )
        {
            CMPXMessage* message = CMPXMessage::NewL();
            message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
            message->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, TMPXPlaybackMessage::EPropertyChanged ); 
            message->SetTObjectValueL<TInt>( KMPXMessageGeneralType, EPbPropertyPosition );
            message->SetTObjectValueL<TInt>( KMPXMessageGeneralData, 50 );                      
            iObservers[0]->HandlePlaybackMessage( message, KErrNone );            
        }
    }

// ----------------------------------------------------------------------------
// Send property request
// Result will be called back in HandleProperty
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::ValueL(MMPXPlaybackCallback& /* aCallback */,
                                 TMPXPlaybackProperty /* aProperty */)
    {

    }

// ----------------------------------------------------------------------------
// Send property request
// Result will be called back in HandleProperty
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::PropertyL(
    MMPXPlaybackCallback& /* aCallback */,
    TMPXPlaybackProperty /* aProperty */)
    {

    }

// ----------------------------------------------------------------------------
// Return a list of mime types supported by playback framework
// ----------------------------------------------------------------------------
//
CDesCArray* CMPXPlaybackUtility::SupportedMimeTypes()
    {
    CDesCArray* desArray = NULL;

    return desArray;
    }

// ----------------------------------------------------------------------------
// Return a list of mime types supported by playback framework
// ----------------------------------------------------------------------------
//
CDesCArray* CMPXPlaybackUtility::SupportedExtensions()
    {
    CDesCArray* desArray = NULL;

    return desArray;
    }

// ----------------------------------------------------------------------------
// Return a list of mime types supported by playback framework
// ----------------------------------------------------------------------------
//
CDesCArray* CMPXPlaybackUtility::SupportedSchemas()
    {
    CDesCArray* desArray = NULL;

    return desArray;
    }

// ----------------------------------------------------------------------------
// Sets the priority of the playback utility
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::SetPriority( TInt /* aPriority */ )
    {

    }


// ----------------------------------------------------------------------------
// CMPXPlaybackUtility::AddSubscriptionL
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::AddSubscriptionL(
    const CMPXSubscription& /* aSubscription */)
    {

    }

// ----------------------------------------------------------------------------
// CMPXPlaybackUtility::RemoveSubscriptionL
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::RemoveSubscriptionL(
    const CMPXSubscription& /* aSubscription */ )
    {

    }

// ----------------------------------------------------------------------------
// CMPXPlaybackUtility::ClearSubscriptionsL
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::ClearSubscriptionsL()
    {

    }
// ----------------------------------------------------------------------------
// Get a list of player types
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::GetPlayerTypesL(
    RArray<TMPXPlaybackPlayerType>& /* aTypes */ )
    {

    }

// ----------------------------------------------------------------------------
// Returns display name for custom types
// ----------------------------------------------------------------------------
//
HBufC* CMPXPlaybackUtility::PlayerTypeDisplayNameL(TMPXPlaybackPlayerType /* aType */ )
    {
		HBufC* buf=NULL;
		return buf;
    }

// ----------------------------------------------------------------------------
// Get the list of UIDs of all players
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::GetPlayerListL(RArray<TUid>& /* aPlayers */)
    {

    }

// ----------------------------------------------------------------------------
// Get the list of UIDs of players with the specific type
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::GetPlayerListL(RArray<TUid>& /* aPlayers */,
                                         TMPXPlaybackPlayerType /* aType */)
    {

    }

// ----------------------------------------------------------------------------
// Asynchronous method: when server completes message, RunL() will be called
// and then the observer is notified of the results. Data required for the
// results that is not supplied by the server is stored; data supplied by the
// server is written back into packaged descriptors in this address space
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::SubPlayerNamesL(MMPXPlaybackCallback& /* aCallback */,
                                          TUid /* aPlayer */)
    {

    }

// ----------------------------------------------------------------------------
// Selects all players with the type
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::SelectPlayersL(TMPXPlaybackPlayerType /* aType */)
    {

    }

// ----------------------------------------------------------------------------
// Selects a specific player
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::SelectPlayerL(TUid /* aPlayer */ )
    {

    }

// ----------------------------------------------------------------------------
// Selects a specific player and sub player
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::SelectSubPlayerL(TUid /* aPlayer */, TInt /* aSubPlayerIndex */)
    {

    }

// ----------------------------------------------------------------------------
// Clears all selection criteria.
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::ClearSelectPlayersL()
    {

    }

// ----------------------------------------------------------------------------
// Retreives the current selection
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::GetSelectionL(TMPXPlaybackPlayerType& /* aType */,
                                        TUid& /* aPlayer */,
                                        TInt& /* aSubPlayerIndex */,
                                        HBufC*& /* aSubPlayerName */)
    {

    }

// ----------------------------------------------------------------------------
// The current player
// ----------------------------------------------------------------------------
//
MMPXPlayer* CMPXPlaybackUtility::CurrentPlayer()
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// The 'type' of the player
// ----------------------------------------------------------------------------
//
TMPXPlaybackPlayerType CMPXPlaybackUtility::TypeL()
    {
    MPX_FUNC_EX("CMPXPlaybackUtility::TypeL()");
    return EPbLocal;
    }

// ----------------------------------------------------------------------------
// The name of player type
// ----------------------------------------------------------------------------
//
HBufC* CMPXPlaybackUtility::TypeNameL()
    {
		HBufC* buf=NULL;
		return buf;
    }

// ----------------------------------------------------------------------------
// Get sub players
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::SubPlayerNamesL(MMPXPlaybackCallback& aCallback)
    {
    MPX_FUNC_EX("CMPXPlaybackUtility::SubPlayerNamesL()");
    SubPlayerNamesL(aCallback,UidL());
    }

// ----------------------------------------------------------------------------
// Selected sub player index
// ----------------------------------------------------------------------------
//
TInt CMPXPlaybackUtility::SubPlayerL() const
    {
    return 0;
    }

// ----------------------------------------------------------------------------
// The UID identifying this player
// ----------------------------------------------------------------------------
//
TUid CMPXPlaybackUtility::UidL() const
    {
    }

// ----------------------------------------------------------------------------
// Path to the collection
// ----------------------------------------------------------------------------
//
CMPXCollectionPlaylist* CMPXPlaybackUtility::PlaylistL()
    {
    MPX_FUNC_EX("CMPXPlaybackUtility::PlaylistL()");
    CMPXCollectionPlaylist *p = NULL;

    return p;
    }

// ----------------------------------------------------------------------------
// Return file handle
// ----------------------------------------------------------------------------
//
RFile* CMPXPlaybackUtility::FileL()
    {
    RFile* file(NULL);
    return file;
    }

// ----------------------------------------------------------------------------
// URI of current song
// ----------------------------------------------------------------------------
//
HBufC* CMPXPlaybackUtility::UriL()
    {
		HBufC* buf=NULL;
		return buf;
    }

// ----------------------------------------------------------------------------
// Request for media properties.
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::MediaL(
    const TArray<TMPXAttribute>& aAttrs,
    MMPXPlaybackCallback& aCallback)
    {
    MediaL( aAttrs, aCallback, NULL );
    }

// ----------------------------------------------------------------------------
// Request for media properties.
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::MediaL(
    const TArray<TMPXAttribute>& /* aAttrs */,
    MMPXPlaybackCallback& /* aCallback */,
    CMPXAttributeSpecs* /* aSpecs */)
    {

    }

// ----------------------------------------------------------------------------
// Server has completed the message, and if there's no error, the results are
// available. The server completes with the function id (or error) so the correct
// callback is made
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::RunL()
    {

    }

// ----------------------------------------------------------------------------
// Canceling by the server
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::DoCancel()
    {

    }

// ----------------------------------------------------------------------------
// Sends error message to all observers - maybe it should only be to the
// client that calls the async method
// ----------------------------------------------------------------------------
//
TInt CMPXPlaybackUtility::RunError(TInt /* aError */)
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// Distributes messages to all observers
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::MessageReceived(TInt /* aMsgData */, TInt /* aError */)
    {

    }

// ----------------------------------------------------------------------------
// Distributes messages to all observers
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::HandleMessageL(TInt /* aMsgData */, TInt /* aError */)
    {

    }

// ----------------------------------------------------------------------------
//  Handle error in RunL
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::HandleRunErrorL(TInt /* aError */)
    {

    }


// ----------------------------------------------------------------------------
// Read a buffer from server
// ----------------------------------------------------------------------------
//
HBufC* CMPXPlaybackUtility::DesL(TInt /* aSize */)
    {
    HBufC* des=NULL;

    return des;
    }

// ----------------------------------------------------------------------------
// Adds a sync request to the queue: if there is no outstanding request,
// it will be executed immediately
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::AddRequestL(TMPXPlaybackServerOp /*aFunction */,
                                      MMPXPlaybackCallback* /* aCallback */,
                                      TInt /* aParamData */,
                                      CBufBase* /* aBuf */,
                                      TAny* /* aPtr */,
                                      CBase* /* aAlivePtr */)
    {

    }

// ----------------------------------------------------------------------------
// Executes the next request in the queue.
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::ExecuteNextRequest()
    {

    }



#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
// ----------------------------------------------------------------------------
// Inititialises with a single song, may not be part of any collection
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::Init64L(RFile64& /* aShareableFile */)
    {

    }
 
// ----------------------------------------------------------------------------
// Inititialises with a file, may not be part of any collection
// ----------------------------------------------------------------------------
//
void CMPXPlaybackUtility::InitStreaming64L(RFile64& /* aShareableFile */, const TInt /* aAccessPoint */)
    {

    }

// ----------------------------------------------------------------------------
// Return file handle
// ----------------------------------------------------------------------------
//
RFile64* CMPXPlaybackUtility::File64L()
    {
    RFile64* file(NULL);

    return file;
    }

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

// End of file
