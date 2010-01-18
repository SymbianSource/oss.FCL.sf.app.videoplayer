/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Scheduled download scheduler.*
*/




#ifndef CIPTVVODSCHEDULEDDOWNLOADSCHEDULER_H
#define CIPTVVODSCHEDULEDDOWNLOADSCHEDULER_H

//  INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"
#include "iptvvodscheduleddownloaddefinitions.h"
#include "CIptvServiceManager.h"
#include "MIptvServiceManagerObserver.h"

// CONSTANTS

// CLASS DECLARATION
class CCseScheduledProgram;
class CCseSchedulerApi;
class CIptvVODScheduledDownloadActiveScheduler;

/**
*  Scheduled download scheduler. This class handles the calculating new
*  schedules to the CSE when settings have been changed.
*
*  @lib IptvServer.exe
*  @since 
*/
NONSHARABLE_CLASS ( CIptvVODScheduledDownloadScheduler ) :
            public CBase,
            public MIptvServiceManagerObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aAppUid The application uid to use.
        */
        static CIptvVODScheduledDownloadScheduler* NewL(
                    CIptvServiceManager& aServiceManager );
        
        /**
        * Destructor.
        */
        virtual ~CIptvVODScheduledDownloadScheduler( );

    public: // From MIptvServiceManagerObserver

        void HandleSmEvent( CIptvSmEvent& aEvent );

    public: // New methods

        /**
        * Remove the active from our list,
        * it has done its work and is going to selfdestruct.
        *
        * @param aActive The active to remove.
        */
        void RemoveActive( CIptvVODScheduledDownloadActiveScheduler* aActive );

    private:

        /**
        * C++ constructor.
        *
        * @param aServiceManager The service manager.
        */
        CIptvVODScheduledDownloadScheduler(
                    CIptvServiceManager& aServiceManager );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Implementation of the MIptvServiceManagerObserver::HandleSmEvent
        *
        * @param aEvent the event to handle.
        */
        void HandleSmEventL( CIptvSmEvent& aEvent );

    private:

        /**
        * The service manager.
        */
        CIptvServiceManager& iServiceManager;

        /**
        * A list containing the scheduling objects. Owned.
        */
        RPointerArray<CIptvVODScheduledDownloadActiveScheduler>
                iActiveSchedulers;
    };

#endif      // CIPTVVODSCHEDULEDDOWNLOADSCHEDULER_H
            
// End of File
