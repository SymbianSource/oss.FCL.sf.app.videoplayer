/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Implementation of the scheduled download plugin*
*/



#ifndef CIPTVSCHEDULEDDOWNLOADPLUGIN_H
#define CIPTVSCHEDULEDDOWNLOADPLUGIN_H

// INCLUDES
#include <ipvideo/CCseSchedulerPluginIF.h>
#include <e32std.h>

// CONSTANTS
// None

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CCseScheduledProgram;
class MCsePluginObserver;
class CIptvScheduledDownloadPluginEngine;

// CLASS DECLARATION    
/**
*  Implementation of the scheduled download plugin
*
*  @lib iptvscheduleddownloadplugin.dll
*/
class CIptvScheduledDownloadPlugin : public CCseSchedulerPluginIF
	{
    public:// Constructors and destructor

        /**
        * Two-phased constructor.        
        */
    	static CIptvScheduledDownloadPlugin* NewL();
    	
    	/**
        * C+ destructor
        */
    	virtual ~CIptvScheduledDownloadPlugin();    	
   
    public: // From CCseSchedulerPluginIF

        /**
        * Runs given task in plugin.
        * @param    aProg       Schedule to be run in plugin.
        * @param    aObserver   Pointer back to scheduling engine. Called
        *                       when schedule is completed or error occurs.
        */	
    	void RunTaskL( CCseScheduledProgram& aProg,
    				   MCsePluginObserver* aObserver );

    private:// Constructors and destructor

        /**
        * C++ constructor
        */
    	CIptvScheduledDownloadPlugin();

        /**
        * Reschedule the given schedule.
        *
        * @param aObserver The schedule callback.
        * @param aProg The schedule to reschedule.
        * @param aDelay The delay of the reschedule.
        */
        void RescheduleL(
            MCsePluginObserver* aObserver,
            CCseScheduledProgram& aProg,
            TTimeIntervalMinutes aDelay );

    private: // Data

        /**
        * Pointer to plug-in engine. Owned.
        */
        CIptvScheduledDownloadPluginEngine* iEngine;
        
        /**
         * Boolean pointer representing if we have aborted the plugin
         * operation by deleting this class. Value is red after iEngine->RunTaskL
         * -call in RunTaskL -method. If value is true we return immediately. 
         */
        TBool* iIsDead;        
	};

#endif // CIPTVSCHEDULEDDOWNLOADPLUGIN_H
