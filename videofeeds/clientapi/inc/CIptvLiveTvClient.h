/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    IP LiveTv TV Client API header file*
*/





#ifndef __CIPTV_LIVE_TV_CLIENT_H
#define __CIPTV_LIVE_TV_CLIENT_H


// INCLUDES
#include <e32base.h>
#include "RIptvClientSession.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  IP LiveTv TV Client API
*
*  @lib IptvClientApi.dll
*  @since
*/
class CIptvLiveTvClient : public CBase
    {
            
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static IMPORT_C CIptvLiveTvClient* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvLiveTvClient();

    public: // New functions
		/**
		* Issues update EPG command, asynchronous
		* @param aStatus request status
		*/
		IMPORT_C void UpdateEPG( TUint32 aServiceProviderId, TRequestStatus& aStatus );
		
		/**
		* Issues add service id command, asynchronous
		* @param aStatus request status
		*/
		IMPORT_C void AddServiceIdL( TUint32 aServiceId, TRequestStatus& aStatus );

		/**	
		* Update channel object's order number. Synchronous.
		* @param aServiceId Service provider id.
		* @param aChannelId Channel id.
		* @param aOrder New order number for channel.
		* @return KErrNone if update was ok, otherwise system-wide error codes.
		*/
		IMPORT_C TInt UpdateChannelOrderL( const TUint32 aServiceId, 
										   const TInt64 aChannelId,
									 	   const TUint32 aOrder );
		/**
		* Sets used IAP. Synchronous method.
		* @param aIap is id of iap to use. 
		* @param aStatus request status
		*/
		IMPORT_C TInt SetIapL( TUint32 aIap );		
		
    private:

        /**
        * C++ default constructor.
        */
        CIptvLiveTvClient();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private: // Data

        /**
        * iSession, the server session.
        */
        RIptvClientSession iSession;

        /**
        * iMsg Heap object for IPC messaging.
        */
        HBufC8* iMsg;

        /**
        * iMsgPtr Passed to RIptvClientSession 
        *          Points to iMsg heap object.
        */
        TPtr8   iMsgPtr;
    };

#endif // __CIPTV_LIVE_TV_CLIENT_H
// End of File
