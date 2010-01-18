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
* Description:   interface class declaration for action handler plugin dll*
*/


#ifndef C_VIDEO_ACTION_HANDLER_H
#define C_VIDEO_ACTION_HANDLER_H

#include <e32base.h>
#include <ahplugin.h>


// FORWARD DECLARATION
class CLiwMap;
class TLiwVariant;
class TVideoPlayerActivationMessage;
class CMediatorEventProvider;

class CVideoActionHandler : public CAHPlugin
	{

public:

	/**
	 * Standard Symbian 2 phase constructor
	 */
	static CVideoActionHandler* NewL();
	
	/**
	 * Standard Symbian 2 phase constructor
	 */
	static CVideoActionHandler* NewLC();

	/**
	 * Standard C++ destructor.
	 */
	~CVideoActionHandler ();

	/**
	 * Executes provided action
	 *
	 * @return status code
	 */
	TInt ExecuteActionL( const CLiwMap* aMap );
private:

	/**
	 * Standard Symbian 2nd phase constructor.
	 */
	void ConstructL();

private:

    /**
     * Method extracts data from map provided and sets 
     * activation message data based on that
     * 
     * @param aMap           map containing data from the matrix menu
     * @param aActivationMsg messatge struct where data is saved
     * 
     * @return KErrNone if message prepared ok, otherwise a systemwide error
     */
    TInt PrepareMessageL( const CLiwMap* aMap, TVideoPlayerActivationMessage& aActivationMsg );
    
    /**
     * Method starts a TV & Video -related application providing a startupmessage given,
     * 
     * @param TMpxVideoPlayerActivationMessage message for application
     * 
     * @return KErrNone if message prepared ok, otherwise a systemwide error
     */
    TInt StartAppL( TVideoPlayerActivationMessage& aMessage );
	
    /**
     * Launch the media settings application
     *
     * @param TMpxVideoPlayerActivationMessage message for application
     *
     * @return KErrNone if message prepared ok, otherwise a systemwide error
     */
    TInt LaunchMediaSettingsL( TVideoPlayerActivationMessage& aMessage );
       
    /**
     * Launch the my videos application (MPX Video Player)
     * 
     * @param TMpxVideoPlayerActivationMessage message for application
     *  
     * @return KErrNone if message prepared ok, otherwise a systemwide error
     */
    TInt LaunchMyVideosL( TVideoPlayerActivationMessage& aMessage );
       
    /**
     * Launch the given application via UID, or bring it to the foregroud with the activation message
     * 
     * @param TMpxVideoPlayerActivationMessage message for application
     * @param TUid appUID The UID3 of the application to be launched
     * @param TInt aActivationMessage The message to send the process already running
     * 
     * @return KErrNone if message prepared ok, otherwise a systemwide error
     */
    TInt LaunchMatrixTaskL( TVideoPlayerActivationMessage& aMessage, TUid appUID, TInt aActivationMessage );
    
	/**
	 * Extract Variant data. Triest to find a data based on the 
	 * aKeyName provided from the map.
	 * 
	 * @param aMap     map where to search data from
	 * @param aVariant variant where data is saved to
	 * @param aKeyName Key identification for data search
	 * 
	 * @return KErrNone if data found and extracted ok.
	 */
	TInt ExtractVariant( const CLiwMap* aMap,
						 TLiwVariant& aVariant,
						 const TDesC8& aKeyName );
	
    /**
     * Sends Mediator event 
     * 
     * @param aEvent Event to be sent.
     */
	void SendMediatorEvent( TInt aEvent );
	
    /**
     * Loads Last watched video data
     * 
     * @param TMpxVideoPlayerActivationMessage message to put the data
     */
    void LoadLwDataL( TVideoPlayerActivationMessage& aMessage );
    
    /**
     * Tries to bring the video player app to foreground
     * 
     *  @return Was the task found and successfully brought to foreground.
     */    
    TInt TryBringVideoPlayerToForegroundL();
	
// DATA
	
    /**
     * Mediator event provider
     */
	CMediatorEventProvider* iMediatorEventProvider; 

	};

#endif /* C_VIDEO_ACTION_HANDLER_H */
