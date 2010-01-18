/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef CIPTVEPGMANAGERIMPL_H
#define CIPTVEPGMANAGERIMPL_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"      
#include "MIptvServiceManagerObserver.h"

// FORWARD DECLARATIONS

class CIptvEpgDatabase;
class CIptvServer;
class CIptvEpgSession;
class CIptvSmEvent;
class CIptvEpgVodMsqQueue;
class CIptvEpgDb;

// CLASS DECLARATION
class CIptvEpgManagerImpl : public CBase, public MIptvServiceManagerObserver
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        static CIptvEpgManagerImpl* NewL(CIptvServer& aServer);

        /**
         * Destructor.
         */
        ~CIptvEpgManagerImpl();
        
        /**
        * Creates EPG session.
        *
        * @param aServiceId Service id.
        * @param aMsgQueue Pointer to Vod Message Queue for the session.
        * @param aError Reference to variable where possible error is stored.
        * @param aPluginCanRun Defines is plugin (related to session) is allowed to run.
        * @return Pointer to new session.
        */
        CIptvEpgSession* CreateSessionL(TUint32 aServiceId, CIptvEpgVodMsqQueue* aMsgQueue, TInt& aError, TBool aPluginCanRun = ETrue);

        /**
        * Close session.
        * @param aServiceId Service id.
        */
        void CloseSession(TUint32 aServiceId);
    
    public: // From MIptvServiceManagerObserver

        /**
         * CIptvServiceManager calls this every time a service is
         * modified in the database.
         * @param aServiceId
         */
        void HandleSmEvent(CIptvSmEvent& aEvent);
                                               
    private:    // New functions

        /**
        * C++ default constructor.
        */
        CIptvEpgManagerImpl(CIptvServer& aServer);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();

    private:

        class CDbHolder : public CBase
            {
            public:
            CDbHolder() {}
            virtual ~CDbHolder();

                TUint32           iServiceId;
                CIptvEpgDb*       iEpgDb;
                CIptvEpgDatabase* iLiveTvDb;
                TInt              iReferenceCount;
            };

        /**
        * Return the database holder for the service id, create if not found already.
        *
        * @param aServiceId The service id.
        * @param aEpgDb Create epg (ETrue) or live tv (EFalse) database.
        * @return The databse holder from the list.
        */
        CDbHolder* GetDatabaseHolderL( TUint32 aServiceId, TBool aEpgDb );

    private:    // Data members 
        CIptvServer&                iServer;
        RPointerArray<CDbHolder>    iEpgDbArray;
        RFs iFs;    	
    };

#endif  // CIPTVEPGMANAGERIMPL_H

// End of File
