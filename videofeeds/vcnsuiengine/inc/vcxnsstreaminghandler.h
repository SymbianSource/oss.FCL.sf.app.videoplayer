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
* Description:   Streaming handler*
*/


// Version : %version: 2 %


#ifndef CVCXNSSTREAMINGHANDLER_H
#define CVCXNSSTREAMINGHANDLER_H

#include <e32base.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackobserver.h>

class CVcxNsContent;
class CVcxNsUiEngine;

/**
*  Streaming handler. Listens playback events and
*  sets and gets the play positions for the stream
*
*  @lib vcxnsuiengine.lib
*/
NONSHARABLE_CLASS( CVcxNsStreamingHandler ) : public CBase,
                                              public MMPXPlaybackObserver,
                                              public MMPXPlaybackCallback
    {
    
public:

    static CVcxNsStreamingHandler* NewL( CVcxNsUiEngine& aUiEngine );

    virtual ~CVcxNsStreamingHandler();
    
    /**
     * Start listening messages for a stream.
     * 
     * @param aContent Content to be watched.
     */
    void HandleOpenStreamL( CVcxNsContent* aContent );

    /**
     * Stop listening messages for a stream.
     */
    void HandleCloseStream();

// From MMPXPlaybackObserver

    /**
     * From MMPXPlaybackObserver
     * Handle playback message
     *
     * @since 3.1
     * @param aMessage playback message
     * @param aErr system error code.
     */
    void HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError );

// From MMPXPlaybackCallback

    /**
     *  From MMPXPlaybackCallback
     *  Handle playback property.
     *
     *  @since S60 3.2.3
     *  @param aProperty the property
     *  @param aValue the value of the property
     *  @param aError error code
     */
    void HandlePropertyL(TMPXPlaybackProperty aProperty,
                                TInt aValue, 
                                 TInt aError);
    
    /**
     *  From MMPXPlaybackCallback
     *  Method is called continously until aComplete=ETrue, signifying that 
     *  it is done and there will be no more callbacks.
     *  Only new items are passed each time.
     *
     *  @since S60 3.2.3
     *  @param aPlayer UID of the subplayer
     *  @param aSubPlayers a list of sub players
     *  @param aComplete ETrue no more sub players. EFalse more subplayer
     *                   expected
     *  @param aError error code
     */
    void HandleSubPlayerNamesL(TUid aPlayer, 
                                       const MDesCArray* aSubPlayers,
                                       TBool aComplete,
                                       TInt aError);
    
    /**
     *  From MMPXPlaybackCallback
     *  Call back of media request.
     *
     *  @since S60 3.2.3
     *  @param aMedia media 
     *  @param aError error code    
     */
    void HandleMediaL(const CMPXMedia& aProperties,
                              TInt aError);

private:
    
    CVcxNsStreamingHandler( CVcxNsUiEngine& aUiEngine );

    void ConstructL();
     
    /**
     * Handle playback message
     * 
     * @param aMessage playback message
     */
    void DoHandlePlaybackMessageL( const CMPXMessage& aMessage );
    
    /**
     * Handle state change message.
     * 
     * @param aMessage playback message
     */
    void HandleStateChangedL( const CMPXMessage& aMessage );
    
    /**
     * Handle property change message.
     * 
     * @param aMessage playback message
     */
    void HandlePropertyChangedL( const CMPXMessage& aMessage );

    /**
     * Seek To Last PosL
     */
    void SeekToLastPosL();

    /**
     * Store Last PosL
     */
    void StoreLastPosL();
    
    /**
     * Get the current stream's duration
     */
    void GetDurationL();

     /**
      * MMPXPlaybackUtility
      * 
      * own.
      */
    MMPXPlaybackUtility* iPlaybackUtility;
    
    /**
     * CVcxNsContent
     * 
     * not own.
     */
    CVcxNsContent* iContent;
    
    /**
     * CVcxNsUiEngine
     */
    CVcxNsUiEngine& iUiEngine;
    
    /**
     * Should the last position be resumed when 
     * playback has been initialized
     */
    TBool iLastPosPending;

    /**
     * Position to be resumed, if the
     * iLastPosPending has been set.
     */
    TInt iResumePos;
    
    /**
     * Playback state.
     */
    TInt iCurrentState;
    
    /**
     * Played stream's total duration.
     */
    TInt iDuration;
    
    };

#endif // CVCXNSSTREAMINGHANDLER_H
