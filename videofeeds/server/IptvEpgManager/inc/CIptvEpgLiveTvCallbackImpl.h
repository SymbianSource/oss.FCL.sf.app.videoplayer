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




#ifndef CIPTVEPGLIVETVCALLBACKIMPL_H
#define CIPTVEPGLIVETVCALLBACKIMPL_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include "MIptvEpgLiveTvCallback.h"
#include "MIptvEpgFileSwapObserver.h"
#include "IptvLiveDataStructures.h"

// FORWARD DECLARATIONS
class CIptvEpgDatabase;
class CIptvEpgManagerImpl;
class CIptvEpgFileSwapper;

// CLASS DECLARATION
class CIptvEpgLiveTvCallbackImpl : public CBase,
								   public MIptvEpgLiveTvCallback,
								   public MIptvEpgFileSwapObserver
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        static CIptvEpgLiveTvCallbackImpl* NewL( CIptvEpgDatabase& aLivTvEpgDb );

        /**
         * Destructor.
         */
        virtual ~CIptvEpgLiveTvCallbackImpl();

        /**
         * Is plugin running.
         */
        TBool IsPluginRunning() const { return iPluginRunning; };
        
        void SetServiceId( TUint32 aServiceId );
                         
    public: // From MIptvEpgLiveTvCallback interface

    	void Started();
    	
    	void Finished( TInt aError );

	    void AddChannel( CIptvEpgChannel* aChannel );
	    
	    void AddProgram( CIptvEpgProgram* aProgram );
	    
	    void AddProgramsWithSchedules( const RPointerArray<CIptvEpgProgramWithSchedule>& aProgramWithSchedule );
	    
	    void AddProgramWithSchedule( CIptvEpgProgramWithSchedule& aProgramWithSchedule );
	    
	    void AddScheduleData( CIptvEpgSchedule& aScheduleData );
	    
	    void EpgDownloadStarted();
	    
	    void EpgDownloadCompleted( TInt aErrorCode );
	    
	    void EpgParsingStarted();
	    
	    void EpgParsingFinished( TInt aErrorCode );
	    
	    void Error( TInt aErrorCode );
	    
	    void ChannelUpdated( TInt64 aChannelId );
	    
	    TInt UpdateChannelIcon( const TUint32 aServiceId, 
								const TInt64 aChannelId, 
								const TDesC& aIconPath );
	    
	    void UpdateProgramIcon( TInt64 aProgramId, TDesC& aIconPath );
	    
	    void LatestEpgAvailableL( TTime aLatestSchedule );

	    void SetLastModifiedDataL( const TUint32 aServiceId, const TDesC& aETag, const TDesC& aLastModified );
	    
	    void StartFileSwappingL();
	    
	public: // From MIptvEpgFileSwapObserver
		/**
		*	Callback method from file swapper to announce when file swapping
		*	has been completed.
		*	@param	aError	Possible error code occured during the file swap
		*	@return	None
		*/
		void FileSwapComplete( TInt aError );
	    
    private:    // New functions           
        /**
        * C++ default constructor.
        */
        CIptvEpgLiveTvCallbackImpl( CIptvEpgDatabase& aLiveTvEpgDb );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();
        
        /**
        * Handler for cases when plugin has failed.
        * @param 
        */
        void HandleFailedEpgUpdateL();
        
        /**
        * Sends aMessage with aError to the ui via message queue.
        * @param
        */
        void SendMessageToUI( TIptvQueueMsg aMessage, TInt aError ) const;
        
        void PrintScheduleData( const CIptvEpgSchedule& aSchedule ) const;

		/**
		* Copy channel order values from old database to the new one.
		*/
		void UpdateChannelOrdersL();
                                          
    private:    // Data members                
        CIptvEpgDatabase& iLiveTvEpgDb;
        TBool iPluginRunning;
        TUint32 iServiceId;
        
        CIptvEpgFileSwapper* iFileSwapper;
        
        TFileName iUpdateDatabase;
        
        TFileName iDatabaseInUse;
    };

#endif  // CIPTVEPGLIVETVCALLBACKIMPL_H

// End of File
