/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __CIPTVSERVERSESSION_H__
#define __CIPTVSERVERSESSION_H__

// INCLUDE FILES
#include <e32base.h>

#include "CIptvServiceManager.h"

// FORWARD DECLARATIONS
class CIptvServer;
class CIptvServiceMsgHandler;
class CIptvEpgMsgHandler;
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
class CIptvVodDownloadMsgHandler;
class CIptvMyVideosMsgHandler;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
class CIptvNetworkSelectionMsgHandler;

// CLASS DECLARATION
/**
* CTimeServerSession.
*  An instance of class CIptvServerSession is created for each client.
*/
class CIptvServerSession : public CSession2
    {
    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * @param aClient The Client's thread.
        * @param aServer The server.
        * @return Pointer to created CIptvServerSession object.
        */
        static CIptvServerSession* NewL( RThread& aClient, 
                                         CIptvServer& aServer);

        /**
        * NewLC.
        * Two-phased constructor.
        * @param aClient The Client's thread.
        * @param aServer The server.
        * @return Pointer to created CIptvServerSession object.
        */
        static CIptvServerSession* NewLC( RThread& aClient, 
                                          CIptvServer& aServer);

        /**
        * ~CIptvServerSession.
        * Destructor.
        */
        virtual ~CIptvServerSession();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CSession, ServiceL.
        * Service request from client.
        * @param aMessage Message from client
        *                 (containing requested operation and any data).
        */
        void ServiceL( const RMessage2& aMessage );

    private: // Constructors and destructors

        /**
        * CIptvServerSession.
        * C++ default constructor.
        * @param aClient The Client's thread.
        * @param aServer The server.
        */
        CIptvServerSession( RThread& aClient, CIptvServer& aServer);

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New methods

        /**
        * PanicClient.
        * Causes the client thread to panic.
        * @param aPanic Panic code.
        */
        void PanicClient( TInt aPanic ) const;

        /**
        * HandleMsgHandlerLeave
        * Handles message handlers leaves. Messages are completed
        * with error status if their handle is not null.
        * @param aError system wide error code
        * @param aMsgHandlerName Module name which message handler leaved.
        * @param aMessage message which handling leaved.
        */
        void HandleMsgHandlerLeave(TInt aError,
                                   const TDesC& aMsgHandlerName,
                                   const RMessage2& aMessage);

    private: // Data

        /**
        * iServer, reference to the server.
        */
        CIptvServer& iServer;

        CIptvServiceMsgHandler*          iServiceMsgHandler;
        CIptvEpgMsgHandler*              iEpgMsgHandler;
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)        
        CIptvVodDownloadMsgHandler*      iVodDownloadMsgHandler;
        CIptvMyVideosMsgHandler*         iMyVideosMsgHandler;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
        CIptvNetworkSelectionMsgHandler* iNetworkSelectionMsgHandler;
		
		
    };

#endif // __CIPTVSERVERSESSION_H__
