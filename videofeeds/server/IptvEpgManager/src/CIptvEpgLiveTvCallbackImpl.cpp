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




// INCLUDE FILES
#include <e32base.h>
#include "IptvDebug.h"
#include "CIptvEpgDatabase.h"
#include "CIptvEpgLatestEpgAvailable.h"
#include "IptvLiveLogger.h"
#include "CIptvEpgFileSwapper.h"
#include <bautils.h>
#include <e32msgqueue.h>					// For RMsgQueue

#include "CIptvEpgLiveTvCallbackImpl.h"
#include "CIptvEpgManagerImpl.h"
#include "CIptvEpgChannel.h"
#include "CIptvEpgProgram.h"
#include "CIptvEpgProgramWithSchedule.h"
#include "CIptvEpgSchedule.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::ConstructL()
    {       
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgLiveTvCallbackImpl::ConstructL");         
    }

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::NewL
// Two-phased constructor.
// Create instance of concrete interface implementation
// ---------------------------------------------------------
//
CIptvEpgLiveTvCallbackImpl* CIptvEpgLiveTvCallbackImpl::NewL( CIptvEpgDatabase& aLiveEpgDb )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgLiveTvCallbackImpl::NewL");  

    CIptvEpgLiveTvCallbackImpl* self =
    			new(ELeave) CIptvEpgLiveTvCallbackImpl( aLiveEpgDb );
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::~CIptvEpgLiveTvCallbackImpl
// Destructor
// ---------------------------------------------------------
//
CIptvEpgLiveTvCallbackImpl::~CIptvEpgLiveTvCallbackImpl()
    {	
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgLiveTvCallbackImpl::~CIptvEpgLiveTvCallbackImpl");  
	delete iFileSwapper;
	iFileSwapper = NULL;
    }

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::CIptvEpgLiveTvCallbackImpl
// C++ default constructor
// ---------------------------------------------------------
//
CIptvEpgLiveTvCallbackImpl::CIptvEpgLiveTvCallbackImpl( CIptvEpgDatabase& aLiveEpgDb ) :
											iLiveTvEpgDb( aLiveEpgDb ), 
											iPluginRunning( EFalse )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgLiveTvCallbackImpl::CIptvEpgLiveTvCallbackImpl");
    }

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::SetServiceId
// Setter for currently active service
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::SetServiceId( TUint32 aServiceId )
	{
	iServiceId = aServiceId;
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::Started
// Callback to signalize plugin has been started.
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::Started()
	{
	LIVE_TV_TRACE1( _L("************************") );
	LIVE_TV_TRACE1( _L("") );
	LIVE_TV_TRACE1( _L("Live TV plugin started") );
	LIVE_TV_TRACE1( _L("") );
	LIVE_TV_TRACE1( _L("************************") );
	iPluginRunning = ETrue;
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::Finished
// Callback to signalize plugin has been finished
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::Finished( TInt aError )
	{
	LIVE_TV_TRACE1( _L("************************") );
	LIVE_TV_TRACE1( _L("") );
	LIVE_TV_TRACE1( _L("Live TV plugin finished") );
	LIVE_TV_TRACE1( _L("") );
	LIVE_TV_TRACE1( _L("************************") );
	iPluginRunning = EFalse;
	TInt error = KErrNone;
	if ( aError == KErrNone )
		{
		TRAP( error, UpdateChannelOrdersL() );
		if ( error != KErrNone )
			{
			LIVE_TV_TRACE2(_L("CIptvEpgLiveTvCallbackImpl::Finished: UpdateChannelOrdersL FAILED: %d"), error);
			SendMessageToUI( EEPGUpdateCompleted, error );
			TRAP( error, HandleFailedEpgUpdateL() );
			if ( error != KErrNone )
				{
				LIVE_TV_TRACE2(_L("CIptvEpgLiveTvCallbackImpl::Finished: HandleFailedEpgUpdateL FAILED: %d"), error);
				}
			}
		else
			{
			// Start doing file swap...
			TRAP( error, StartFileSwappingL() );
			if ( error != KErrNone )
				{
				LIVE_TV_TRACE2( _L("StartFileSwapping returned %d"), error );
				SendMessageToUI( EEPGUpdateCompleted, error );
				TRAP( error, HandleFailedEpgUpdateL() );
				if ( error != KErrNone )
					{
					LIVE_TV_TRACE2(_L("CIptvEpgLiveTvCallbackImpl::Finished: HandleFailedEpgUpdateL FAILED: %d"), error);
					}
				}	
			}
		
		}
	else // Something wrong with epg update -> destroy all the work done
		{
		TRAP( error, HandleFailedEpgUpdateL() );
		if ( error != KErrNone )
			{
			LIVE_TV_TRACE2( _L("HandleFailedEpgUpdateL caused leave with error %d"), error );
			}
		// Send note to UI about epg update failure
		SendMessageToUI( EEPGUpdateCompleted, aError );
		}
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::HandleFailedEpgUpdateL
// 
// ---------------------------------------------------------
//  
void CIptvEpgLiveTvCallbackImpl::HandleFailedEpgUpdateL()
	{
	delete iFileSwapper;
	iFileSwapper = NULL;
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	// First of all try to delete all content from
	// \videocenter\tmp\live\<serviceId>
	TFileName tmpFolder;
	CIptvUtil::GetPathL( fs, EIptvPathTmpLive, iServiceId, tmpFolder, EFalse );
	TInt error = KErrNone;
	error = BaflUtils::DeleteFile( fs, tmpFolder, CFileMan::ERecurse );
	if ( error != KErrNone )
		{
		LIVE_TV_TRACE2( _L("BaflUtils::DeleteFile returned %d"), error );
		}
	
	CleanupStack::PopAndDestroy( &fs );
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::AddChannel
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//  
void CIptvEpgLiveTvCallbackImpl::AddChannel( CIptvEpgChannel* aChannel )
	{
	if ( aChannel )
		{
		aChannel->SetServiceId( iServiceId );
		TUint32 temp;
		TRAP_IGNORE( iLiveTvEpgDb.InsertOrUpdateChannelL( *aChannel, temp ) );		
		}
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::AddProgram
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//  
void CIptvEpgLiveTvCallbackImpl::AddProgram( CIptvEpgProgram* aProgram )
	{
	if ( aProgram )
		{
		aProgram->SetServiceId( iServiceId );
		TUint32 temp;
		TRAP_IGNORE( iLiveTvEpgDb.InsertOrUpdateProgramL( *aProgram, temp ) );	
		}
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::AddProgramsWithSchedulesL
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//  
void CIptvEpgLiveTvCallbackImpl::AddProgramsWithSchedules(
						const RPointerArray<CIptvEpgProgramWithSchedule>& aProgramsWithSchedules )
	{
	LIVE_TV_TRACE1( _L("CIptvEpgLiveTvCallbackImpl::AddProgramsWithSchedules IN") );
	TInt error = KErrNone;
	TRAP( error, iLiveTvEpgDb.AddProgramsWithSchedulesL ( aProgramsWithSchedules ) );
	if ( error != KErrNone )
		{
		LIVE_TV_TRACE2( _L("iLiveTvEpgDb->AddProgramsWithSchedulesL caused leave with error %d"), error );
		}
	LIVE_TV_TRACE1( _L("CIptvEpgLiveTvCallbackImpl::AddProgramsWithSchedules OUT") );
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::AddProgramWithScheduleL
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//  
void CIptvEpgLiveTvCallbackImpl::AddProgramWithSchedule(
						CIptvEpgProgramWithSchedule& aProgramWithSchedule )
	{
	LIVE_TV_TRACE1( _L("CIptvEpgLiveTvCallbackImpl::AddProgramWithSchedule IN") );
	TUint32 temp;
	TInt error = KErrNone;
	TRAP( error, iLiveTvEpgDb.AddProgramWithScheduleL( aProgramWithSchedule, temp ) );
	if ( error != KErrNone )
		{
		LIVE_TV_TRACE2( _L("iLiveTvEpgDb->AddProgramWithScheduleL caused leave with error code %d"), error ); 
		}
	LIVE_TV_TRACE1( _L("CIptvEpgLiveTvCallbackImpl::AddProgramWithSchedule IN") );
	}


// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::AddScheduleData
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//  
void CIptvEpgLiveTvCallbackImpl::AddScheduleData( CIptvEpgSchedule& aScheduleData )
	{
	aScheduleData.iServiceId = iServiceId;
	// This function is used when there is actual Smart Vision epg available
	TRAP_IGNORE( iLiveTvEpgDb.InsertOrUpdateScheduleL( aScheduleData ) );
	}

void CIptvEpgLiveTvCallbackImpl::EpgDownloadStarted()
	{
	// Send notify here
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::EpgDownloadCompleted
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::EpgDownloadCompleted( TInt /*aErrorCode*/ )
	{
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::EpgParsingStarted
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::EpgParsingStarted()
	{
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::EpgParsingFinished
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::EpgParsingFinished( TInt /*aErrorCode*/ )
	{
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::StartFileSwappingL
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::StartFileSwappingL()
	{
	iUpdateDatabase.Zero();
	iDatabaseInUse.Zero();
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	CIptvUtil::GetPathL( fs, EIptvPathTmpLive, iServiceId, iUpdateDatabase, EFalse );
	if ( BaflUtils::PathExists( fs, iUpdateDatabase ) )
		{
		iUpdateDatabase.Append( KLiveTvEPGDatabaseName );
		}

	CIptvUtil::GetPathL( fs, EIptvPathEcgLive, iServiceId, iDatabaseInUse, EFalse );
	if ( BaflUtils::PathExists( fs, iDatabaseInUse ) )
		{
		iDatabaseInUse.Append( KLiveTvEPGDatabaseName );
		}
	if ( iFileSwapper )
		{
		delete iFileSwapper;
		iFileSwapper = NULL;
		}
	iFileSwapper = CIptvEpgFileSwapper::NewL( iDatabaseInUse, iUpdateDatabase, *this );
	iFileSwapper->SwapFilesL();
	CleanupStack::PopAndDestroy( &fs ); // Closes the fs-session
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::UpdateChannelOrdersL
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::UpdateChannelOrdersL()
	{
	LIVE_TV_TRACE1(_L("CIptvEpgLiveTvCallbackImpl::UpdateChannelOrdersL in"));
	iUpdateDatabase.Zero();
	iDatabaseInUse.Zero();
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	CIptvUtil::GetPathL( fs, EIptvPathTmpLive, iServiceId, iUpdateDatabase, EFalse );
	if ( BaflUtils::PathExists( fs, iUpdateDatabase ) )
		{
		iUpdateDatabase.Append( KLiveTvEPGDatabaseName );
		}

	CIptvUtil::GetPathL( fs, EIptvPathEcgLive, iServiceId, iDatabaseInUse, EFalse );
	if ( BaflUtils::PathExists( fs, iDatabaseInUse ) )
		{
		iDatabaseInUse.Append( KLiveTvEPGDatabaseName );
		}
	// Create database objects
	CIptvEpgDatabase* oldDatabase = CIptvEpgDatabase::NewL( iDatabaseInUse );
	CleanupStack::PushL( oldDatabase );
	CIptvEpgDatabase* newDatabase = CIptvEpgDatabase::NewL( iUpdateDatabase );
	CleanupStack::PushL( newDatabase );

	// Get all old database channels
	RPointerArray<CIptvEpgChannel> oldChannels;
	CleanupClosePushL( oldChannels );
	TRAPD( err, oldDatabase->GetChannelsL( iServiceId, &oldChannels ); );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("oldDatabase->GetChannelsL FAILED: %d"), err);
		oldChannels.ResetAndDestroy();
		User::Leave( err );
		}
	// Get all new database channels
	RPointerArray<CIptvEpgChannel> newChannels;
	
	TRAP( err, newDatabase->GetChannelsL( iServiceId, &newChannels ); );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("newDatabase->GetChannelsL FAILED: %d"), err);
		newChannels.ResetAndDestroy();
		newChannels.Close();
		oldChannels.ResetAndDestroy();
		User::Leave( err );
		}
	TInt i( 0 );
	TInt j( 0 );
	// ok, now compare the channels orders in old and new EPG database
	if ( oldChannels.Count() == newChannels.Count() )
		{
		// Channel counts are the same, continue
		// Check that the two databases hold the same channels
		TBool matchedChannelFound( EFalse );
		TBool channelsMatch( ETrue );
		for ( i = 0; i < oldChannels.Count() && channelsMatch; i++ )
			{
			matchedChannelFound = EFalse;
			for ( j = 0; j < newChannels.Count() && !matchedChannelFound; j++ )
				{
				// compare service and channel id's
				if ( oldChannels[i]->ServiceId() == newChannels[j]->ServiceId() && 
					 oldChannels[i]->ChannelId() == newChannels[j]->ChannelId() &&
					 oldChannels[i]->ChannelOrder() > 0 )
					{
					matchedChannelFound = ETrue;
					}
				}
			if ( !matchedChannelFound )
				{
				channelsMatch = EFalse;
				}
			}
		if ( channelsMatch )
			{
			// Channels are unchanged, now update the channel orders
			for ( i = 0; i < oldChannels.Count(); i++ )
				{
				for ( j = 0; j < newChannels.Count(); j++ )
					{
					if ( oldChannels[i]->ServiceId() == newChannels[j]->ServiceId() && 
					 	 oldChannels[i]->ChannelId() == newChannels[j]->ChannelId() )
						{
						TRAP( err, newDatabase->UpdateChannelOrderL( 
							 				oldChannels[i]->ServiceId(),
											oldChannels[i]->ChannelId(),
											oldChannels[i]->ChannelOrder() );
							);
						if ( err != KErrNone )
							{
							LIVE_TV_TRACE2(_L("newDatbase->UpdateChannelOrderL FAILED: %d"), err);
							oldChannels.ResetAndDestroy();
							newChannels.ResetAndDestroy();
							newChannels.Close();
							User::Leave( err );
							}
						}
					}
				}
			}
		else
			{
			LIVE_TV_TRACE1(_L("CIptvEpgLiveTvCallbackImpl::UpdateChannelOrdersL: The channel id's don't match, not updating channel orders!"));
			}
		}
	else
		{
		// Channel count is different, let's quit
		LIVE_TV_TRACE3(_L("CIptvEpgLiveTvCallbackImpl::UpdateChannelOrdersL: Channel counts differ, not updating order numbers! oldChannels.Count = %d, newChannels.Count = %d"), oldChannels.Count(), newChannels.Count() );
		}
	newChannels.ResetAndDestroy();
	newChannels.Close();
	oldChannels.ResetAndDestroy();
	CleanupStack::PopAndDestroy( &oldChannels );
	CleanupStack::PopAndDestroy( newDatabase );
	CleanupStack::PopAndDestroy( oldDatabase );
	CleanupStack::PopAndDestroy( &fs ); // Closes the fs-session
	LIVE_TV_TRACE1(_L("CIptvEpgLiveTvCallbackImpl::UpdateChannelOrdersL out"));
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::Error
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::Error( TInt /*aErrorCode*/ )
	{
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::ChannelUpdated
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::ChannelUpdated( TInt64 /*aChannelId*/ )
	{
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::UpdateChannelIcon
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//
TInt CIptvEpgLiveTvCallbackImpl::UpdateChannelIcon( const TUint32 aServiceId, 
								const TInt64 aChannelId, const TDesC& aIconPath )
	{
	TRAPD( err, iLiveTvEpgDb.UpdateChannelIconPathL( 
										aServiceId, aChannelId, aIconPath ) );
	if ( err != KErrNone ) 
		{
		LIVE_TV_TRACE2(_L("CIptvEpgLiveTvCallbackImpl::UpdateChannelIcon() UpdateChannelIconL FAILED: %d"), err);
		}
	return err;
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::UpdateProgramIcon
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::UpdateProgramIcon( TInt64 /*aProgramId*/, TDesC& /*aIconPath*/ )
	{
	}

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::PrintScheduleData
// Method to print out to log given schedule data
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::PrintScheduleData( const CIptvEpgSchedule& aSchedule ) const
	{
	LIVE_TV_TRACE1( _L(""));
	LIVE_TV_TRACE1( _L("Adding schedule:"));
	LIVE_TV_TRACE1( _L(""));
	LIVE_TV_TRACE2( _L("aSchedule.iServiceId = %u"), aSchedule.iServiceId );
	LIVE_TV_TRACE2( _L("aSchedule.iChannelId = %Li"), aSchedule.iChannelId );
	LIVE_TV_TRACE2( _L("aSchedule.iProgramId = %Li"), aSchedule.iProgramId );
	TTime start( aSchedule.iStartTime );
	TBuf<50> buf;
	_LIT( KFormat, "%D %M %Y %H %T %S" );
	TRAP_IGNORE( start.FormatL( buf, KFormat() ) );
	LIVE_TV_TRACE2( _L("aSchedule.iStartTime = %S"), &buf );
	TTime end( aSchedule.iEndTime );
	buf.Zero();
	TRAP_IGNORE( end.FormatL( buf, KFormat() ) );
	LIVE_TV_TRACE2( _L("aSchedule.iEndTime = %S"), &buf );
	LIVE_TV_TRACE1( _L(""));
	LIVE_TV_TRACE1( _L("End of program data:"));
	LIVE_TV_TRACE1( _L(""));
	}
	
// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::LatestEpgAvailable
// From MIptvEpgLiveTvCallback interface
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::LatestEpgAvailableL( TTime aLatestSchedule )
	{
	CIptvEpgLatestEpgAvailable* latestEpgAvailable = CIptvEpgLatestEpgAvailable::NewL();
	CleanupStack::PushL( latestEpgAvailable );
	latestEpgAvailable->SetServiceId( iServiceId );
	latestEpgAvailable->SetStartTime( aLatestSchedule );
	iLiveTvEpgDb.InsertOrUpdateLatestEpgAvailableL( *latestEpgAvailable );
	CleanupStack::PopAndDestroy( latestEpgAvailable );						
	}
	
void CIptvEpgLiveTvCallbackImpl::SetLastModifiedDataL( const TUint32 aServiceId,
													   const TDesC& aETag,
													   const TDesC& aLastModified )
	{
	LIVE_TV_TRACE1( _L("CIptvEpgLiveTvCallbackImpl::SetLastModifiedDataL IN" ) );
	iLiveTvEpgDb.SetLastModifiedDataL( aServiceId, aETag, aLastModified );
	LIVE_TV_TRACE1( _L("CIptvEpgLiveTvCallbackImpl::SetLastModifiedDataL OUT" ) );
	}
	

// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::FileSwapComplete
// From MIptvEpgFileSwapObserver
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::FileSwapComplete( TInt aError )
	{
	delete iFileSwapper;
	iFileSwapper = NULL;
	// Send notification to UI about file swap complete.
	SendMessageToUI( EEPGUpdateCompleted, aError );
	}
	
// ---------------------------------------------------------
// CIptvEpgLiveTvCallbackImpl::SendMessageToUI
// From MIptvEpgFileSwapObserver
// ---------------------------------------------------------
//
void CIptvEpgLiveTvCallbackImpl::SendMessageToUI( TIptvQueueMsg aMessage, TInt aError ) const
	{
	RMsgQueue<SIptvQueueEntry> queue;
	TInt err = queue.OpenGlobal( KIptvUiEPGMsgQueueName );
	if ( err == KErrNone )
		{
		SIptvQueueEntry entry;
		entry.iServiceId = iServiceId;
		entry.iMsg = aMessage;
		entry.iErr = aError;
		err = queue.Send( entry );
		if ( err != KErrNone )
			{
			LIVE_TV_TRACE2( _L("CIptvEpgLiveTvCallbackImpl::SendMessageToUI: queue.Send failed with error %d"), err );
			}
		queue.Close();
		}
	}

// end of file
