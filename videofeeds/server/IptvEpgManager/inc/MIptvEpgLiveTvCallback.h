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




#ifndef MIPTVEPGLIVETVCALLBACK
#define MIPTVEPGLIVETVCALLBACK

// FORWARD DECLARATIONS
class CIptvEpgChannel;
class CIptvEpgProgram;
class CIptvEpgSchedule;
class CIptvEpgProgramWithSchedule;

// CLASS DECLARATION

class MIptvEpgLiveTvCallback
    {
    public:
    
    virtual void AddChannel( CIptvEpgChannel* aChannel ) = 0;
    
    virtual void AddProgram( CIptvEpgProgram* aProgram ) = 0;
    
    virtual void AddProgramsWithSchedules( const RPointerArray<CIptvEpgProgramWithSchedule>& aProgramWithSchedule ) = 0;
    
    virtual void AddProgramWithSchedule( CIptvEpgProgramWithSchedule& aProgramWithSchedule ) = 0;

    virtual void AddScheduleData( CIptvEpgSchedule& aScheduleData ) = 0;
    
    virtual void Started() = 0;
    
    virtual void Finished( TInt aError ) = 0;
    
    virtual void EpgDownloadStarted() = 0;
    
    virtual void EpgDownloadCompleted( TInt aErrorCode ) = 0;
    
    virtual void EpgParsingStarted() = 0;
    
    virtual void EpgParsingFinished( TInt aErrorCode ) = 0;
    
    virtual void Error( TInt aErrorCode ) = 0;
    
    virtual void ChannelUpdated( TInt64 aChannelId ) = 0;
    
    virtual TInt UpdateChannelIcon( const TUint32 aServiceId, 
									const TInt64 aChannelId, 
									const TDesC& aIconPath ) = 0;
    
    virtual void UpdateProgramIcon( TInt64 aProgramId, TDesC& aIconPath ) = 0;
    
    virtual void LatestEpgAvailableL( TTime aLatestSchedule ) = 0;
    
    virtual void SetLastModifiedDataL( const TUint32 aServiceId,
    								   const TDesC& aModifiedSince,
    								   const TDesC& aETag ) = 0;
    };

#endif  // MIPTVEPGLIVETVCALLBACK

// End of File
