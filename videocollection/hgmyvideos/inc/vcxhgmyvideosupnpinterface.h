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
* Description:      CVcxuPnPInterface class declaration*
*/




#ifndef CVCXHGUPNPINTERFACE_H
#define CVCXHGUPNPINTERFACE_H

// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <upnpcommandobserver.h>

// FORWARD DECLARATIONS
class CUpnpShowCommand;


// CLASS DECLARATION

/**
 * CVcxHgMyVideosUPnPInterface declares an interface for myvideos UI 
 * to show videos and images via upnp framework's command api
 * 
 */
NONSHARABLE_CLASS( CVcxHgMyVideosUPnPInterface ) : public CBase,
                                                   public MUpnpCommandObserver
    {
public: // Constructors and destructor

    /**
     * Destructor.
     */
    ~CVcxHgMyVideosUPnPInterface();

    /**
     * Two-phased constructor.
     * 
     * @param aExtObserver pointer to external command observer
     */
    static CVcxHgMyVideosUPnPInterface* NewL( MUpnpCommandObserver* aExtObserver = 0 );

    /**
     * Two-phased constructor.
     * 
     * @param aExtObserver pointer to external command observer
     */
    static CVcxHgMyVideosUPnPInterface* NewLC( MUpnpCommandObserver* aExtObserver = 0 );
    
public: // new methods
    
    /**
     * Returns ETrue, if home network functionality
     * is usable via upnp
     * 
     * @return TBool ETrue is usable
     */
    TBool IsApplicable();
    
    /**
     * Returns ETrue, upnp has been started succesfully
     * by the Strat() -call
     * 
     * @return TBool ETrue if started
     */
    TBool IsStarted();
    
    /**
     * Starts upnp framework for video / image showing
     * 
     * @return TInt     KErrNone if ok, KErrGeneral if start is not yet called 
     *                  succesfully. Otherwise a systemwide error code.
     */
    TInt Start( );
    
    /**
     * Shows a video in remote upnp device.
     * 
     * @param aVideoPath full path for the video including video file name
     * 
     * @return TInt     KErrNone if ok, KErrGeneral if start is not yet called 
     *                  succesfully. Otherwise a systemwide error code.
     */
    TInt ShowVideo( const TDesC& aVideoPath );
    
    /**
     * Shows an image in remote upnp device.
     * 
     * @param aPath full path for the image including image file name
     * 
     * @return TInt KErrNone if ok, other a systemwide error
     */
    TInt ShowThumbnail( const TDesC& aPath );
    
    /**
     * Notifies framework to stop showing
     * 
     * @return TInt KErrNone if ok, otherwise a systemwide error
     */
    TInt Stop();
    
public: // from MUpnpCommandObserver

    /**
     * Upnp framework calls this to return a result from 
     * Asynchronous method -calls. Only async methods are the one 
     * related to showing of image of video. In case there is external
     * observer defined, message is to it.
     * This method handles only KErrDisconnected status, where 
     * we set iStarted flag EFalse. 
     * 
     * @param aStatusCode  statuscode from the upnp framework
     */
    void CommandComplete( TInt aStatusCode );
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CVcxHgMyVideosUPnPInterface();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
    /**
     * Method creates CUpnpShowCommand object if not yet created.
     * Creation unsuuceeds, if platform does not yet support home networking
     * via upnp (Home media setup not yet done). In this case pointer to
     * CUpnpShowCommand remains NULL.  
     * 
     * @return TBool  ETrue, creation ok
     */
    TBool TryCreateCmd();

private:
    
    /**
     * pointer to Upnp Show -command interface. Owned.
     */
    CUpnpShowCommand* iShowCommand;
    
    /**
     * pointer to external Upnp command observer. Not Owned.
     */
    MUpnpCommandObserver* iExtObserver;
    
    /**
     * ETrue:  Start has been called succesfully.
     * EFalse: Start not yet called succesfully
     */
    TBool iStarted;
    
    };

#endif // CVCXHGUPNPINTERFACE_H
