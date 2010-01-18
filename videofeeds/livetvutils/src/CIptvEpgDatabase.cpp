/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Offers interface to IPTV Epg database*
*/




// INCLUDE FILES
#include <badesca.h>    // CDesCArrayFlat
#include <bautils.h>    // file helpers
#include <d32dbms.h>
#include <eikenv.h>
#include "CIptvEpgLatestEpgAvailable.h"
#include "IptvLiveLogger.h"

#include "CIptvEpgDatabase.h"
#include "CIptvEpgChannel.h"
#include "CIptvEpgProgram.h"
#include "CIptvEpgProgramWithSchedule.h"
#include "CIptvEpgSchedule.h"
#include "CIptvEpgScheduleSearch.h"

// CONSTANTS

// constant for program guide: day changes at 04:00 AM
// Note: If you modify this, the range is 0-23, otherwise
// USER 3 panic will happen in GetSchedulesByChannelAndDayL
const TInt KIptvDefaultDayOffsetHours( 4 ); 
const TInt KIptvDefaultUriCount( 5 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvEpgDatabase::CIptvEpgDatabase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CIptvEpgDatabase::CIptvEpgDatabase( const TFileName& aDbFile ) : 
	iDbFile( aDbFile ), iLocalState( EReady )
    {
    }

// -----------------------------------------------------------------------------
// CIptvEpgDatabase::ConstructL
// Second phase construction. Leaves, if RFs session cannot be created.
// -----------------------------------------------------------------------------
void CIptvEpgDatabase::ConstructL()
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::ConstructL()"));
    User::LeaveIfError( iFsSession.Connect() );
    MakeSqlStrings();
    
    // Create database and connect into it
	CreateDbL();

	TRAPD( error, CreateMulticastDbSessionL());
    if ( error != KErrNone )
        {
        LIVE_TV_TRACE2(_L("CIptvEpgDatabase:: Could not open session to db (%d)"), error);

        if ( error != KErrNoMemory && 
             error != KErrLocked && 
             error != KErrDisMounted &&
             error != KErrDiskFull &&
             error != KErrNotReady )
            {
            // Delete and recreate database file. Cannot recover other way. 
            LIVE_TV_TRACE1(_L("CIptvEpgDatabase:: fatal error occured while opening db, recreating db"));
            
            iFsSession.Delete( iDbFile ); //ignore error
            
            TRAP( error, CreateDbL() );
            if ( error != KErrNone )
                {
                LIVE_TV_TRACE2(_L("CIptvEpgDatabase:: couldnt recreate db (%d), leaving."), error);
                User::Leave( error );
                }
                
            TRAP( error, CreateMulticastDbSessionL() );
            if ( error != KErrNone )
                {
                LIVE_TV_TRACE2(_L("CIptvEpgDatabase:: couldnt open session to db (%d), leaving."), error);
                User::Leave( error );
                }
                
            }
        else
            {
            LIVE_TV_TRACE1(_L("CIptvEpgDatabase:: temporary error occured while opening db, leaving db intact, leaving."));
            User::Leave( error );
            }
        }
	
	iLocalState = EReady;
	
	// Register to listen the file for backup
	iBackupWrapper = CBaBackupSessionWrapper::NewL();
	iBackupWrapper->RegisterFileL( iDbFile, *this );
	
	iActiveWait = new (ELeave) CActiveSchedulerWait();
    }

// -----------------------------------------------------------------------------
// CIptvEpgDatabase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CIptvEpgDatabase* CIptvEpgDatabase::NewL(const TFileName& aDbFile)
    {
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::NewL()"));
    CIptvEpgDatabase* self = new ( ELeave ) CIptvEpgDatabase( aDbFile );    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );    
    return self;
    }
    
// -----------------------------------------------------------------------------
// CIptvEpgDatabase::~CIptvEpgDatabase()
// Destructor
// -----------------------------------------------------------------------------
CIptvEpgDatabase::~CIptvEpgDatabase()
    {
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::~CIptvEpgDatabase in"));
	if ( iBackupWrapper )
		{
		iBackupWrapper->DeregisterFile( iDbFile );
		}
	delete iBackupWrapper;
    iFsSession.Close();
    CloseMulticastDbSession();
    
    if ( iActiveWait && iActiveWait->IsStarted() )
    	{
    	iActiveWait->AsyncStop();
    	}
    delete iActiveWait;
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::~CIptvEpgDatabase out"));    
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CreateDbL()
//
// Create a new database. 
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CreateDbL()
    {
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::CreateDbL()"));
    if ( !BaflUtils::FileExists( iFsSession, iDbFile ) )
        {
        LIVE_TV_TRACE1( _L("Database file wasn't found, creating new"));
    	RDbNamedDatabase database;
		CleanupClosePushL( database );
		BaflUtils::EnsurePathExistsL( iFsSession, iDbFile );
    	User::LeaveIfError( database.Create( iFsSession, iDbFile ) );
        
		CreateChannelTableL( database );
        
        CreateProgramTableL( database );
        
        CreateScheduleTableL( database );

		CreateLatestEpgAvailableTableL( database );
		
		CreateLastModifiedTableL( database );

        CleanupStack::PopAndDestroy( &database );
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CreateMulticastDbSessionL()
//
// Open a new database session and the database
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CreateMulticastDbSessionL()
    {
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::CreateMulticastDbSessionL()"));
    User::LeaveIfError( iMulticastDbSession.Connect() );
    User::LeaveIfError( iMulticastDb.Open( iMulticastDbSession, iDbFile ) );
    User::LeaveIfError( iMulticastDb.Compact() );
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CloseMulticastDbSession()
//
// Closes a database session and the database
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CloseMulticastDbSession()
    {
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::CloseMulticastDbSession()"));
    iMulticastDb.Close();
    iMulticastDbSession.Close();
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CreateScheduleTableL()
//
// Creates schedule database table
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CreateScheduleTableL( RDbNamedDatabase& aDatabase ) const
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::CreateScheduleTableL()"));
	TDbCol keyCol( KIptvEpgScheduleTableKeyCol, EDbColUint32 );
	keyCol.iAttributes = TDbCol::EAutoIncrement;
	
	TDbCol serviceIdCol( KIptvEpgScheduleServiceProviderIdCol, EDbColUint32 );
	
	TDbCol channelIdCol( KIptvEpgScheduleChannelIdCol, EDbColInt64 );
	
	TDbCol programIdCol( KIptvEpgScheduleProgramIdCol, EDbColInt64 );
	
	TDbCol startTimeCol( KIptvEpgScheduleStartTimeCol, EDbColDateTime );
	
	TDbCol endTimeCol( KIptvEpgScheduleEndTimeCol, EDbColDateTime );

    CDbColSet* scheduleColSet = CDbColSet::NewLC();
    scheduleColSet->AddL( keyCol );
    scheduleColSet->AddL( serviceIdCol );
    scheduleColSet->AddL( channelIdCol );
    scheduleColSet->AddL( programIdCol );
    scheduleColSet->AddL( startTimeCol );
    scheduleColSet->AddL( endTimeCol );
        
    User::LeaveIfError( aDatabase.CreateTable( KIptvEpgScheduleTable, *scheduleColSet ) );
    CleanupStack::PopAndDestroy( scheduleColSet );
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CreateChannelProgramTableL()
//
// Creates ChannelProgram table. Leaves, if the table cannot be created.
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CreateChannelProgramTableL( 
										RDbNamedDatabase& /*aDatabase*/ ) const
    {
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CreateProgramTableL()
//
// Creates Program table. Leaves, if the table cannot be created.
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CreateProgramTableL( RDbNamedDatabase& aDatabase ) const
    {
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::CreateProgramTableL" ) );
    // Key  
    TDbCol keyCol( KIptvEpgProgramDbKeyCol, EDbColUint32 );
    keyCol.iAttributes = TDbCol::EAutoIncrement;
    
    TDbCol idCol( KIptvEpgProgramIdCol, EDbColInt64 );
    
    TDbCol channelIdCol( KIptvEpgProgramChannelId, EDbColInt64 );
    
	TDbCol servProvIdCol( KIptvEpgProgramServProviderIdCol, EDbColUint32 );
    
    TDbCol uriCol( KIptvEpgProgramURICol, EDbColText, KIptvEpgUriMaxLength );
    
    TDbCol genreCol( KIptvEpgProgramGenreCol, EDbColText );
    
    TDbCol nameCol( KIptvEpgProgramNameCol, EDbColText, KIptvEpgProgramMaxLength );
    
    TDbCol descriptionCol( KIptvEpgProgramDescriptionCol, EDbColLongText );
    
    TDbCol languageCol( KIptvEpgProgramLanguageCol, EDbColText );
    
    TDbCol parentalCol( KIptvEpgProgramParentalRatingCol, EDbColText );
    
    CDbColSet* programColSet = CDbColSet::NewLC();
    programColSet->AddL( keyCol );
    programColSet->AddL( idCol );
    programColSet->AddL( channelIdCol );
	programColSet->AddL( servProvIdCol );
    programColSet->AddL( uriCol );
    programColSet->AddL( genreCol );
    programColSet->AddL( nameCol );
    programColSet->AddL( descriptionCol );
    programColSet->AddL( languageCol );
    programColSet->AddL( parentalCol );
        
    TInt error = aDatabase.CreateTable( KIptvEpgProgramTable, 
										*programColSet );
    if ( error != KErrNone )
        {
        LIVE_TV_TRACE2( _L("\t\tCIptvEpgDatabase::CreateProgramTableL CreateTable returned %d"), error );
        User::Leave( error );
        }
    
    CleanupStack::PopAndDestroy( programColSet ); 
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CreateChannelTableL()
//
// Creates Channel table. Leaves, if the table cannot be created.
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CreateChannelTableL( RDbNamedDatabase& aDatabase ) const
    {
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::CreateChannelTableL" ) );
    // Key    
    TDbCol keyCol( KIptvEpgChannelDbKeyCol, EDbColUint32 );
    keyCol.iAttributes = TDbCol::EAutoIncrement;

    TDbCol idCol( KIptvEpgChannelIdCol, EDbColInt64 );
    
    TDbCol servProvIdCol( KIptvEpgChannelServProviderIdCol, EDbColUint32 );
    
    TDbCol nameCol( KIptvEpgChannelNameCol, EDbColText, KIptvEpgChannelNameMaxLength );
    
    TDbCol logoPathCol( KIptvEpgChannelLogoPathCol, EDbColText, KIptvEpgLogoPathMaxLength );
    
    TDbCol descriptionCol( KIptvEpgChannelDescriptionCol, EDbColText,
                           KIptvEpgDescrMaxLength );
    
    TDbCol uriCol( KIptvEpgChannelURICol, EDbColText, KIptvEpgUriMaxLength );
    
    TDbCol sdpCol( KIptvEpgChannelSDPCol, EDbColLongText );

	TDbCol orderCol( KIptvEpgChannelOrderCol, EDbColUint32 );

	// Create column set and add defined columns in to the set
    CDbColSet* channelColSet = CDbColSet::NewLC();
    channelColSet->AddL( keyCol );
    channelColSet->AddL( idCol );
    channelColSet->AddL( servProvIdCol );
    channelColSet->AddL( nameCol );
    channelColSet->AddL( logoPathCol );
    channelColSet->AddL( descriptionCol );
    channelColSet->AddL( uriCol );
    channelColSet->AddL( sdpCol );
    channelColSet->AddL( orderCol );

    // Create new table to the database with created columnset
    TInt error = aDatabase.CreateTable( KIptvEpgChannelTable, 
										*channelColSet );
    if ( error != KErrNone )
    	{
    	LIVE_TV_TRACE2( _L("\n\t\tCIptvEpgDatabase::CreateChannelTableL: CreateTable returned %d" ), error );
        User::Leave( error );
        }
    
    CleanupStack::PopAndDestroy( channelColSet );
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CreateLastModifiedTableL()
//
// Creates table for last modified data
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CreateLastModifiedTableL( RDbNamedDatabase& aDatabase ) const
	{
	// First create columns
	TDbCol keyCol( KIptvEpgLastModifiedTableKeyCol, EDbColUint32 );
    keyCol.iAttributes = TDbCol::EAutoIncrement;
    TDbCol serviceIdCol( KIptvEpgLastModifiedTableServiceIdCol, EDbColUint32 );
	TDbCol eTagCol( KIptvEpgLastModifiedTableETagCol, EDbColText );
	TDbCol lastModifiedDateTime( KIptvEpgLastModifiedTableTimeCol, EDbColText );
	
	CDbColSet* lastModifiedColSet = CDbColSet::NewLC();
	lastModifiedColSet->AddL( keyCol );
	lastModifiedColSet->AddL( serviceIdCol );
	lastModifiedColSet->AddL( eTagCol );
	lastModifiedColSet->AddL( lastModifiedDateTime );
	
	TInt error = aDatabase.CreateTable( KIptvEpgLastModifiedTable, *lastModifiedColSet );
	
	CleanupStack::PopAndDestroy( lastModifiedColSet );
	
	if ( error != KErrNone )
		{
		LIVE_TV_TRACE2( _L("aDatabase.CreateTable for last modified table caused leave: %d"), error );
		User::Leave( error );
		}
	}


// SET OF GETTERS FOR CHANNEL AND PROGRAM DATA

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetChannelsL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::GetChannelsL( const TUint32 aServiceProviderId,
											  RPointerArray<CIptvEpgChannel>*
											  aResultArray )
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::GetChannelsL()"));    	

	if ( iLocalState == EBackup )
		{
		return;
		}
	
    if ( !aResultArray )
        {
        User::Leave( KErrArgument );
        }
	TBuf<KCustomSqlLength> sqlStatement;
    sqlStatement.Append( _L("SELECT * FROM "));
    sqlStatement.Append( KIptvEpgChannelTable );
    sqlStatement.Append( _L(" WHERE ") );
    sqlStatement.Append( KIptvEpgChannelServProviderIdCol );
    sqlStatement.Append( _L(" = ") );
    sqlStatement.AppendNum( aServiceProviderId );
	DoGetChannelsL( sqlStatement, aResultArray );    
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::DoGetChannelsL()
// 
// ---------------------------------------------------------------------------
//
void CIptvEpgDatabase::DoGetChannelsL( const TDesC& aQuery,
											  RPointerArray<CIptvEpgChannel>*
											  aResultArray )
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::DoGetChannelsL() in"));

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	TInt err( KErrNone ); 
	
    if ( !aResultArray )
        {
        User::Leave( KErrArgument );
        }
    
    RDbView view;
    CleanupClosePushL( view );
    LIVE_TV_TRACE1( _L("CIptvEpgDatabase::DoGetChannelsL about to prepare") ); 
    err =  view.Prepare( iMulticastDb,
    					 TDbQuery(aQuery),
    					 TDbWindow::EUnlimited,
    					 RDbView::EReadOnly );
    LIVE_TV_TRACE2( _L("CIptvEpgDatabase::DoGetChannelsL  prepare = %d"), err ); 
    User::LeaveIfError ( err ); 
    LIVE_TV_TRACE1( _L("CIptvEpgDatabase::DoGetChannelsL about to eval") );        								  
    User::LeaveIfError( view.EvaluateAll() );
    LIVE_TV_TRACE1( _L("CIptvEpgDatabase::DoGetChannelsL about to get colset ") );        
    CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL( colSet );
    TInt idColNo = colSet->ColNo( KIptvEpgChannelIdCol );
    TInt servProbIdColNo = colSet->ColNo( KIptvEpgChannelServProviderIdCol );
    TInt nameColNo = colSet->ColNo( KIptvEpgChannelNameCol );
    TInt logoColNo = colSet->ColNo( KIptvEpgChannelLogoPathCol );
    TInt descrColNo = colSet->ColNo( KIptvEpgChannelDescriptionCol );
    TInt uriColNo = colSet->ColNo( KIptvEpgChannelURICol );
    TInt sdpColNo = colSet->ColNo( KIptvEpgChannelSDPCol );
	TInt orderColNo = colSet->ColNo( KIptvEpgChannelOrderCol );

	CleanupStack::PopAndDestroy( colSet );
    LIVE_TV_TRACE1( _L("CIptvEpgDatabase::DoGetChannelsL getting first") );        
    view.FirstL();
    while ( view.AtRow() )
        {
        view.GetL();            
        CIptvEpgChannel* channel = CIptvEpgChannel::NewL();
        CleanupStack::PushL( channel );

		channel->SetChannelId( view.ColInt64( idColNo ) );
		channel->SetServiceId( view.ColUint32( servProbIdColNo ) );
		channel->SetChannelName( view.ColDes( nameColNo ).AllocL() );
		channel->SetChannelLogoPath( view.ColDes( logoColNo ).AllocL() );
		channel->SetChannelDescription( view.ColDes( descrColNo ).AllocL() );
		channel->SetChannelURI( view.ColDes( uriColNo ).AllocL() );

		// SDP data is stored in long text field, so it needs to be read
		// through stream
		TInt sdpLength = view.ColLength( sdpColNo );
		HBufC* temp = HBufC::NewL( sdpLength );
		CleanupStack::PushL( temp );
		RDbColReadStream read;
		read.OpenLC( view, sdpColNo );
		TPtr ptr = temp->Des();
		read.ReadL( ptr, sdpLength );
		read.Close();
		CleanupStack::PopAndDestroy(); // read;
		channel->SetChannelSDP( temp ); // Ownership transferred
		CleanupStack::Pop( temp );

		channel->SetChannelOrder( view.ColUint32( orderColNo ) );
		
		TLinearOrder<CIptvEpgChannel> order(
							CIptvEpgChannel::LinearOrderByOrderNum );
		// Insert the channel to the array ordered by 
		// CIptvEpgChannel::LinearOrderByOrderNum function
		User::LeaveIfError( aResultArray->InsertInOrderAllowRepeats( channel,
							order ) );
        
        CleanupStack::Pop( channel ); // ownership transferred to aResultArray
        view.NextL();
        }

	LIVE_TV_TRACE2( _L("Added %d channels to aResultArray"), aResultArray->Count() );

	CleanupStack::PopAndDestroy( &view ); // closes view
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::DoGetChannelsL() out"));
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetProgramsByChannelIdL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::GetProgramsByChannelIdL( 
								const TUint32 aServiceProviderId,
								const TInt64 aChannelKey,
								RPointerArray<CIptvEpgProgram>* aResultArray )
    {
    LIVE_TV_TRACE3(_L("CIptvEpgDatabase::GetProgramsByChannelIdL() ser %d cha %Li"), (int)aServiceProviderId, aChannelKey);

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	if ( !aResultArray )
		{
		User::Leave( KErrArgument );
		}
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgProgramTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgProgramChannelId );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aChannelKey );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgProgramServProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );

    LIVE_TV_TRACE2(_L("query >%S<"), &sqlStatement);
    
	FetchProgramsFromTableL( sqlStatement, aResultArray );
	
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetNextProgramL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CIptvEpgSchedule* CIptvEpgDatabase::GetNextProgramL( 
								const TUint32 aServiceProviderId,
								const TInt64 aChannelKey,
								const TTime& aRefTime )
    {
	LIVE_TV_TRACE3(_L("CIptvEpgDatabase::GetNextProgramL() ser %d cha %Li"), (int)aServiceProviderId, aChannelKey);

	if ( iLocalState == EBackup )
		{
		return NULL;
		}
	
	TBuf<KCustomSqlLength> sqlStatement;    
    sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgScheduleTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgScheduleChannelIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aChannelKey );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgScheduleServiceProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );
    LIVE_TV_TRACE2(_L("query >%S<"), &sqlStatement);

    CIptvEpgSchedule* retval = NULL;
	
	RPointerArray<CIptvEpgSchedule> resultArray;
	CleanupClosePushL( resultArray );
	TRAPD( err, FetchSchedulesFromTableL( sqlStatement, &resultArray ) );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("CIptvEpgDatabase::GetNextProgramL: FetchSchedulesFromTableL FAILED: %d"), err);
		resultArray.ResetAndDestroy();
		User::Leave( err );
		}
	
	const TInt cnt( resultArray.Count() );
	TInt index( KErrNotFound );
	for ( TInt i( 0 ); i < cnt; i++ )
	    {
	    // quit looping when the 1st prg which starting time is greater
	    // than current time
	    if ( resultArray[i]->GetStartTime() > aRefTime )
	        {
	        retval = resultArray[i];
	        index = i;
	        i = cnt; // quit looping
	        }
	    }
	
	if ( cnt > 0 )
		{
		for ( TInt j( 0 ); j < cnt; j++ ) // note, starts from 1, not 0. 
			{
			if ( j != index )
			    {
    			delete resultArray[j]; 
    			resultArray[j] = NULL;
			    }
			}
		
		if ( index < 0 )
		    {
		    resultArray.Reset();
	        CleanupStack::PopAndDestroy( &resultArray ); // closes array	
	        return retval;
		    }
		if ( retval )
			{
			// match the found schedule with correct program data, query program table
			TBuf<KCustomSqlLength> sqlStatementForPrograms;
			sqlStatementForPrograms.Append( _L("SELECT * FROM ") );
			sqlStatementForPrograms.Append( KIptvEpgProgramTable );
			sqlStatementForPrograms.Append( _L(" WHERE ") );
			sqlStatementForPrograms.Append( KIptvEpgProgramChannelId );
			sqlStatementForPrograms.Append( _L(" = ") );
			sqlStatementForPrograms.AppendNum( aChannelKey );
			sqlStatementForPrograms.Append( _L(" AND ") );
			sqlStatementForPrograms.Append( KIptvEpgProgramServProviderIdCol );
			sqlStatementForPrograms.Append( _L(" = ") );
			sqlStatementForPrograms.AppendNum( aServiceProviderId );
			sqlStatementForPrograms.Append( _L(" AND ") );
			sqlStatementForPrograms.Append( KIptvEpgProgramIdCol );
			sqlStatementForPrograms.Append( _L(" = ") );
			sqlStatementForPrograms.AppendNum( retval->GetProgramId() );
			
			RPointerArray<CIptvEpgProgram> resultProgArray;
			CleanupClosePushL( resultProgArray );
		    
			TRAP( err, FetchProgramsFromTableL( sqlStatementForPrograms, 
				  &resultProgArray ) );
			if ( err != KErrNone )
				{
				LIVE_TV_TRACE2(_L("FetchProgramsFromTableL FAILED: %d"), err);
				resultArray.ResetAndDestroy();
				resultProgArray.ResetAndDestroy();
				User::Leave( err );
				}

			const TInt cntProg( resultProgArray.Count() );
			if ( cntProg > 0 )
				{
				retval->iProgram = resultProgArray[0];// note, index 0 is the matching prog
				for ( TInt k( 1 ); k < cntProg; k++ ) // note, starts from 1, not 0. 
					{
					delete resultProgArray[k]; 
					resultProgArray[k] = NULL;
					}
				}
			else
				{
				// no matching program found	
				}
			resultProgArray.Reset();			
			CleanupStack::PopAndDestroy( &resultProgArray ); // closes array					
			}	
		}
		
    	    
	resultArray.Reset();
	CleanupStack::PopAndDestroy( &resultArray ); // closes array	
	return retval;
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetProgramL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CIptvEpgSchedule* CIptvEpgDatabase::GetProgramL( 
								const TUint32 aServiceProviderId,
								const TInt64 aChannelId,
								const TInt64 aProgramId )
    {
	LIVE_TV_TRACE4(_L("CIptvEpgDatabase::GetProgramL() in aServiceProviderId %d aChannelId %Li, aProgramId %Li"), (int)aServiceProviderId, aChannelId, aProgramId);

	if ( iLocalState == EBackup )
		{
		return NULL;
		}
	
	TBuf<KCustomSqlLength> sqlStatement;    
    sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgScheduleTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgScheduleChannelIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aChannelId );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgScheduleServiceProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgScheduleProgramIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aProgramId );
	
    CIptvEpgSchedule* retval = NULL;
	
	RPointerArray<CIptvEpgSchedule> resultArray;
	CleanupClosePushL( resultArray );
	TRAPD( err, FetchSchedulesFromTableL( sqlStatement, &resultArray ) );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("FetchSchedulesFromTableL FAILED: %d"), err);
		resultArray.ResetAndDestroy();
		User::Leave( err );
		}
		
	if ( resultArray.Count() > 0 )
		{
		// Select first one found
		retval = resultArray[0];
		resultArray.Remove( 0 );
		}
	resultArray.ResetAndDestroy();
	CleanupStack::PopAndDestroy( &resultArray );

	TBuf<KCustomSqlLength> sqlStatementForPrograms;
	sqlStatementForPrograms.Append( _L("SELECT * FROM ") );
	sqlStatementForPrograms.Append( KIptvEpgProgramTable );
	sqlStatementForPrograms.Append( _L(" WHERE ") );
	sqlStatementForPrograms.Append( KIptvEpgProgramChannelId );
	sqlStatementForPrograms.Append( _L(" = ") );
	sqlStatementForPrograms.AppendNum( aChannelId );
	sqlStatementForPrograms.Append( _L(" AND ") );
	sqlStatementForPrograms.Append( KIptvEpgProgramServProviderIdCol );
	sqlStatementForPrograms.Append( _L(" = ") );
	sqlStatementForPrograms.AppendNum( aServiceProviderId );
	sqlStatementForPrograms.Append( _L(" AND ") );
	sqlStatementForPrograms.Append( KIptvEpgProgramIdCol );
	sqlStatementForPrograms.Append( _L(" = ") );
	sqlStatementForPrograms.AppendNum( aProgramId );
	
	RPointerArray<CIptvEpgProgram> resultProgArray;
	CleanupClosePushL( resultProgArray );
	    
	TRAP( err, FetchProgramsFromTableL( sqlStatementForPrograms, 
		  &resultProgArray ) );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("FetchProgramsFromTableL FAILED: %d"), err);
		resultProgArray.ResetAndDestroy();
		User::Leave( err );
		}
	if ( resultProgArray.Count() > 0 && retval )
		{
		retval->iProgram = resultProgArray[0];
		resultProgArray.Remove( 0 );
		}
	resultProgArray.ResetAndDestroy();	
	CleanupStack::PopAndDestroy( &resultProgArray ); // closes array					
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::GetProgramL out"));
	return retval;
    }


// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetProgramNamesByServiceIdL()
// 
// ---------------------------------------------------------------------------
//
void CIptvEpgDatabase::GetProgramNamesByServiceIdL( 
								const TUint32 aServiceProviderId,
								RPointerArray<CIptvEpgProgram>* aResultArray )
    {
    LIVE_TV_TRACE2(_L("CIptvEpgDatabase::GetProgramsByServiceIdL() ser %d"), (TInt)aServiceProviderId);

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	if ( !aResultArray )
		{
		User::Leave( KErrArgument );
		}
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT ") );
	sqlStatement.Append( KIptvEpgProgramIdCol );
	sqlStatement.Append( _L(", ") );
	sqlStatement.Append( KIptvEpgProgramNameCol );
	sqlStatement.Append( _L(" FROM "));
	sqlStatement.Append( KIptvEpgProgramTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgProgramServProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );

    LIVE_TV_TRACE2(_L("query >%S<"), &sqlStatement);
    
	FetchProgramNamesFromTableL( sqlStatement, aResultArray );
	
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetSchedulesByServiceIdL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::GetSchedulesByServiceIdL( 
						const TUint32 aServiceProviderId,
						RPointerArray<CIptvEpgScheduleSearch>* aResultArray )
    {
    LIVE_TV_TRACE2(_L("CIptvEpgDatabase::GetSchedulesByServiceIdL() ser %d"), (TInt)aServiceProviderId);

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	if ( !aResultArray )
		{
		User::Leave( KErrArgument );
		}
		
	CIptvEpgProgram* programToUseForFinding = CIptvEpgProgram::NewL(); 
	CleanupStack::PushL( programToUseForFinding ); 
    
	RPointerArray<CIptvEpgProgram> programs;
	CleanupClosePushL( programs );
	TRAPD( err, GetProgramNamesByServiceIdL( aServiceProviderId, &programs ) );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("GetProgramNamesByServiceIdL FAILED: %d"), err);
		programs.ResetAndDestroy();
		User::Leave( err );
		}
    TLinearOrder<CIptvEpgProgram> o( 
							CIptvEpgProgram::LinearOrderOfProgramsById );
	programs.Sort( o ); 
    
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgScheduleTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgScheduleServiceProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );
    LIVE_TV_TRACE2(_L("query >%S<"), &sqlStatement);
    
	TRAP( err, FetchSearchSchedulesFromTableL( sqlStatement, aResultArray ) );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("FetchSearchSchedulesFromTableL FAILED: %d"), err);
		programs.ResetAndDestroy();
		User::Leave( err );
		}
	
	TInt indexFound( KErrNone ); 
	TInt i( 0 );
	TInt64 programId( 0 );
	for ( i = 0; i < aResultArray->Count(); i++ ) 
		{
		programId = ((*aResultArray)[i])->iProgramId;
		programToUseForFinding->SetProgramId( programId );
		indexFound = programs.FindInOrder( programToUseForFinding, o ); 
		if ( indexFound != KErrNotFound && indexFound < programs.Count() ) 
			{
			((*aResultArray)[i])->iProgramName = 
						programs[indexFound]->ProgramName().AllocL();	
			}
		}

	TLinearOrder<CIptvEpgScheduleSearch> order( 
						CIptvEpgScheduleSearch::LinearOrderOfSchedulesByName );
	aResultArray->Sort( order );

	for ( i = 0; i < programs.Count(); i++ )
		{
		delete programs[i];
		programs[i] = NULL;
		}
	programs.Reset();
	CleanupStack::PopAndDestroy( &programs );
	CleanupStack::PopAndDestroy( programToUseForFinding );
	LIVE_TV_TRACE2(_L("CIptvEpgDatabase::GetSchedulesByServiceIdL out, return %d items"), aResultArray->Count() );	
    }
    

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetSchedulesByChannelIdL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::GetSchedulesByChannelIdL( 
								const TUint32 aServiceProviderId,
								const TInt64 aChannelKey,
								RPointerArray<CIptvEpgSchedule>* aResultArray )
    {
    LIVE_TV_TRACE3(_L("CIptvEpgDatabase::GetSchedulesByChannelIdL() ser %d cha %Li"), (int)aServiceProviderId, aChannelKey);

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	if ( !aResultArray )
		{
		User::Leave( KErrArgument );
		}
		
	TInt i,c; 
	CIptvEpgProgram* programToUseForFinding = CIptvEpgProgram::NewL(); 
	CleanupStack::PushL(programToUseForFinding); 
    LIVE_TV_TRACE1(_L("GetSchedulesByChannelIdL starting get prog"));
	RPointerArray<CIptvEpgProgram> programs;
	CleanupClosePushL( programs );
    TRAPD( err, GetProgramsByChannelIdL(aServiceProviderId,	aChannelKey, &programs ) );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("GetProgramsByChannelIdL FAILED: %d"), err);
		programs.ResetAndDestroy();
		User::Leave( err );
		}
    LIVE_TV_TRACE2(_L("GetSchedulesByChannelIdL starting sort n = %d"),programs.Count());
    TLinearOrder<CIptvEpgProgram> o(CIptvEpgProgram::LinearOrderOfProgramsById); 
	programs.Sort( o );
    LIVE_TV_TRACE1(_L("GetSchedulesByChannelIdL starting fetch schedules"));

	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgScheduleTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgScheduleChannelIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aChannelKey );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgScheduleServiceProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );
    LIVE_TV_TRACE2(_L("query >%S<"), &sqlStatement);
    
	TRAP( err, FetchSchedulesFromTableL( sqlStatement, aResultArray ) );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("FetchSchedulesFromTableL FAILED: %d"), err);
		programs.ResetAndDestroy();
		User::Leave( err );
		}
	TLinearOrder<CIptvEpgSchedule> order(CIptvEpgSchedule::LinearOrderOfSchedulesByTime); 
	aResultArray->Sort(order); 
	c = aResultArray->Count(); 	
	TInt indexFound; 
	TInt programsFound = 0;
	LIVE_TV_TRACE2(_L("GetSchedulesByChannelIdL staring finding programs for %d schedules"),c);
	TInt64 programId( 0 );
	for ( i = 0; i < c; i ++ ) 
		{
		programId = ((*aResultArray)[i])->GetProgramId();
		programToUseForFinding->SetProgramId( programId );
		indexFound = programs.FindInOrder( programToUseForFinding, o ); 
		if ( indexFound != KErrNotFound && indexFound < programs.Count() ) 
			{
			if ( programs[indexFound]->ServiceId() == aServiceProviderId &&
				 programs[indexFound]->ChannelId() == aChannelKey )
				{
				((*aResultArray)[i])->iProgram = programs[indexFound];
				programs.Remove( indexFound );
				programsFound++; 	
				}
			}
		}
	programs.ResetAndDestroy();
	CleanupStack::PopAndDestroy( &programs );
	CleanupStack::PopAndDestroy( programToUseForFinding );
	LIVE_TV_TRACE2(_L("GetSchedulesByChannelIdL matched %d programs "),programsFound);	
    }
    
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetSchedulesByChannelAndDayL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::GetSchedulesByChannelAndDayL( 
								const TUint32 aServiceProviderId,
								const TInt64 aChannelKey,
								const TTime& aStartTime,
								RPointerArray<CIptvEpgSchedule>* aResultArray )
    {
    LIVE_TV_TRACE3(_L("CIptvEpgDatabase::GetSchedulesByChannelIdL() ser %d cha %Li"), (int)aServiceProviderId, aChannelKey);

    if ( iLocalState == EBackup )
        {
        return;
        }
	
    if ( !aResultArray )
        {
        User::Leave( KErrArgument );
        }
    // Assume aStartTime is set to HomeTime() (+ possible day offset)
    TTime startTime( aStartTime );
    
    // fix for error JAIA-7FAF4U: if clock is less than 4 AM, 
    // we're showing still the previous day's programs
    startTime -= TTimeIntervalHours( KIptvDefaultDayOffsetHours );
    TTime endTime( startTime );
    // For some reason we need to set endTime to two days from home time to get 
    // programs of tommorow from database
    endTime += TTimeIntervalDays( 2 );
    
    startTime -= TTimeIntervalDays( 1 );
    TDateTime startDateTime( startTime.DateTime() );
    startDateTime.SetHour( 0 );
    startDateTime.SetMinute( 0 );
    startDateTime.SetSecond( 0 );
    startDateTime.SetMicroSecond( 0 );
    startTime = startDateTime;
    startTime += TTimeIntervalHours( KIptvDefaultDayOffsetHours );

    TBuf<100> startTimeBuf;
    	
    _LIT( KDateTimeFormat,"#%1%/1%2%/2%3#" ); 
    startTime.FormatL(startTimeBuf, KDateTimeFormat); 
     
    TBuf<100> endTimeBuf;

    endTime.FormatL(endTimeBuf, KDateTimeFormat);  
    	
    TInt i,c; 
    CIptvEpgProgram* programToUseForFinding = CIptvEpgProgram::NewL(); 
    CleanupStack::PushL(programToUseForFinding); 

    RPointerArray<CIptvEpgProgram> programs;
    CleanupClosePushL( programs );
    LIVE_TV_TRACE1(_L("GetSchedulesByChannelIdL starting get prog"));
    TRAPD( err, GetProgramsByChannelIdL( aServiceProviderId, aChannelKey, &programs ) );
    if ( err != KErrNone )
        {
        LIVE_TV_TRACE2(_L("CIptvEpgDatabase::GetSchedulesByChannelAndDayL GetProgramsByChannelIdL FAILED: %d"), err);
        programs.ResetAndDestroy();
        User::Leave( err );
        }

    TLinearOrder<CIptvEpgProgram> o(CIptvEpgProgram::LinearOrderOfProgramsById); 
    programs.Sort( o ); 
    LIVE_TV_TRACE1(_L("GetSchedulesByChannelIdL starting fetch schedules"));
    TBuf<KCustomSqlLength> sqlStatement;
    sqlStatement.Append( _L("SELECT * FROM ") );
    sqlStatement.Append( KIptvEpgScheduleTable );
    sqlStatement.Append( _L(" WHERE ") );
    sqlStatement.Append( KIptvEpgScheduleChannelIdCol );
    sqlStatement.Append( _L(" = ") );
    sqlStatement.AppendNum( aChannelKey );
    sqlStatement.Append( _L(" AND ") );
    sqlStatement.Append( KIptvEpgScheduleServiceProviderIdCol );
    sqlStatement.Append( _L(" = ") );
    sqlStatement.AppendNum( aServiceProviderId );
    sqlStatement.Append( _L(" AND ") );
    sqlStatement.Append( KIptvEpgScheduleStartTimeCol );
    sqlStatement.Append( _L(" >= ") );
    sqlStatement.Append( startTimeBuf );
    sqlStatement.Append( _L(" AND ") );
    sqlStatement.Append( KIptvEpgScheduleStartTimeCol );
    sqlStatement.Append( _L(" <= ") );
    sqlStatement.Append( endTimeBuf );

    LIVE_TV_TRACE2(_L("query >%S<"), &sqlStatement);

    TRAP( err, FetchSchedulesFromTableL( sqlStatement, aResultArray ) );
    if ( err != KErrNone )
        {
        LIVE_TV_TRACE2(_L("CIptvEpgDatabase::GetSchedulesByChannelAndDayL: FetchSchedulesFromTableL FAILED: %d"), err);
        programs.ResetAndDestroy();
        User::Leave( err );
        }
    TTime progStartTime;
    TTime progEndTime;
    // Remove schedules from aResultArray that do not belong for today
    // taking hour offset into account (e.g. when the day changes)
    if ( aResultArray->Count() > 0 )
        {
        TTime endT( endTime - TTimeIntervalDays( 1 ) );
        TDateTime endTDate = endT.DateTime();
        endTDate.SetHour( KIptvDefaultDayOffsetHours );
        endTDate.SetMinute( 0 );
        endTDate.SetSecond( 0 );
        endTDate.SetMicroSecond( 0 );
        endT = endTDate;
        const TTime startT( startTime + TTimeIntervalDays( 1 ) );
        for ( i = aResultArray->Count() - 1; i >= 0; i-- )
            {
            progStartTime = (*aResultArray)[i]->GetStartTime() + 
                User::UTCOffset();
            progEndTime = (*aResultArray)[i]->GetEndTime() + User::UTCOffset();
            
            if ( progStartTime >= endT )
                {
                delete (*aResultArray)[i];
                (*aResultArray)[i] = NULL;
                aResultArray->Remove( i );    
                }
            else if ( progStartTime < startT && progEndTime <= startT )
                {
                delete (*aResultArray)[i];
                (*aResultArray)[i] = NULL;
                aResultArray->Remove( i );
                }
            }    
        aResultArray->Compress();
        }

    TLinearOrder<CIptvEpgSchedule> order( CIptvEpgSchedule::LinearOrderOfSchedulesByTime ); 
    aResultArray->Sort( order );
    c = aResultArray->Count(); 	
    TInt indexFound; 
    TInt programsFound = 0;
    LIVE_TV_TRACE2(_L("GetSchedulesByChannelIdL starting to find programs for %d schedules"),c);
    TInt64 programId( 0 );
    for ( i = 0; i < c; i ++ ) 
    	{
    	programId = ((*aResultArray)[i])->GetProgramId();
    	programToUseForFinding->SetProgramId( programId );
    	indexFound = programs.FindInOrder( programToUseForFinding, o ); 
    	if ( indexFound != KErrNotFound ) 
    		{
    		((*aResultArray)[i])->iProgram = programs[indexFound];    
    		programs.Remove( indexFound );
    		programsFound ++; 
    		}
    	else
    		{
    		LIVE_TV_TRACE1(_L("Program not found in iProgramsPointedBySchedules!"));
    		}
    	}
    programs.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &programs );
    CleanupStack::PopAndDestroy( programToUseForFinding ); 	
    LIVE_TV_TRACE2(_L("GetSchedulesByChannelIdL matched %d programs "),programsFound);	
    } 

// -----------------------------------------------------------------------------
// CIptvEpgDatabase::GetProgramByTimeL()
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvEpgSchedule* CIptvEpgDatabase::GetProgramByTimeL(
												  const TUint32 aServiceProviderId, 
												  const TInt64 aChannelKey,
												  const TTime& aStartTime )
	{
	LIVE_TV_TRACE3(_L("CIptvEpgDatabase::GetProgramByTimeL() ser %d cha %Li"), (TInt)aServiceProviderId, aChannelKey);

	if ( iLocalState == EBackup )
		{
		return NULL;
		}
	
	CIptvEpgSchedule* retval = NULL;
		
	TBuf<100> startTimeBuf;
	
	// If date separator is set to ':' then the database will
	// return KErrArgument (for some reason) if we use ':' also 
	// as time separator.
	// Avoid the error by selecting time format based on current
	// date separator locale setting.
	TLocale locale;
	TChar dateSeparator = locale.DateSeparator( 1 );	
	_LIT( KDateTimeFormatDoubleDot,"#%1%/1%2%/2%3 %H:%T:%S#" ); 
	_LIT( KDateTimeFormatSingleDot,"#%1%/1%2%/2%3 %H.%T.%S#" ); 
	TBuf<64> dateTimeFormat;
	if ( dateSeparator == ':' )
		{
		dateTimeFormat = KDateTimeFormatSingleDot();
		}
	else
		{
		dateTimeFormat = KDateTimeFormatDoubleDot();
		}
	
	aStartTime.FormatL(startTimeBuf, dateTimeFormat);  

	// query the schedules table for the correct schedule
	// that covers the given time in its duration			
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgScheduleTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgScheduleChannelIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aChannelKey );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgScheduleServiceProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgScheduleStartTimeCol );
	sqlStatement.Append( _L(" <= ") );
	sqlStatement.Append( startTimeBuf );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgScheduleEndTimeCol );
	sqlStatement.Append( _L(" > ") );
	sqlStatement.Append( startTimeBuf );
	LIVE_TV_TRACE2(_L("query >%S<"), &sqlStatement);
    
    RPointerArray<CIptvEpgSchedule> resultArray;
	CleanupClosePushL( resultArray );
    TRAPD( err, FetchSchedulesFromTableL( sqlStatement, &resultArray ) );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("FetchSchedulesFromTableL FAILED: %d"), err);
		resultArray.ResetAndDestroy();
		User::Leave( err );
		}
		
	const TInt cnt( resultArray.Count() );
	
	if ( cnt > 0 )
		{
		retval = resultArray[0];        // note, index 0 is the retval, not deleted
		for ( TInt j( 1 ); j < cnt; j++ ) // note, starts from 1, not 0. 
			{
			delete resultArray[j]; 
			resultArray[j] = NULL;
			}
		// match the found schedule with correct program data, query program table
		TBuf<KCustomSqlLength> sqlStatementForPrograms;
		sqlStatementForPrograms.Append( _L("SELECT * FROM ") );
		sqlStatementForPrograms.Append( KIptvEpgProgramTable );
		sqlStatementForPrograms.Append( _L(" WHERE ") );
		sqlStatementForPrograms.Append( KIptvEpgProgramChannelId );
		sqlStatementForPrograms.Append( _L(" = ") );
		sqlStatementForPrograms.AppendNum( aChannelKey );
		sqlStatementForPrograms.Append( _L(" AND ") );
		sqlStatementForPrograms.Append( KIptvEpgProgramServProviderIdCol );
		sqlStatementForPrograms.Append( _L(" = ") );
		sqlStatementForPrograms.AppendNum( aServiceProviderId );
		sqlStatementForPrograms.Append( _L(" AND ") );
		sqlStatementForPrograms.Append( KIptvEpgProgramIdCol );
		sqlStatementForPrograms.Append( _L(" = ") );
		sqlStatementForPrograms.AppendNum( retval->GetProgramId() );
		
		RPointerArray<CIptvEpgProgram> resultProgArray;
		CleanupClosePushL( resultProgArray );
	    
		TRAP( err, FetchProgramsFromTableL( sqlStatementForPrograms, 
			  &resultProgArray ) );
		if ( err != KErrNone )
			{
			LIVE_TV_TRACE2(_L("FetchProgramsFromTableL FAILED: %d"), err);
			resultArray.ResetAndDestroy();
			resultProgArray.ResetAndDestroy();
			User::Leave( err );
			}

		const TInt cntProg( resultProgArray.Count() );
	
		if ( cntProg > 0 )
			{
			
			retval->iProgram = resultProgArray[0];// note, index 0 is the matching prog
			for ( TInt k( 1 ); k < cntProg; k++ ) // note, starts from 1, not 0. 
				{
				delete resultProgArray[k]; 
				resultProgArray[k] = NULL;
				}
			}
		else
			{
			// no matching program found	
			}
		resultProgArray.Reset();			
		CleanupStack::PopAndDestroy( &resultProgArray ); // closes array					
		}
	else
	    {
	    // no schedule found for the given time	
	    }
	resultArray.Reset();
	CleanupStack::PopAndDestroy( &resultArray ); // closes array   		
	return retval;
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::FetchProgramsFromTableL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::FetchProgramsFromTableL( const TDesC& aSqlStatement,
							RPointerArray<CIptvEpgProgram>* aResultArray )
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::FetchProgramsFromTableL in"));

	if ( iLocalState == EBackup || !aResultArray )
		{
		return;
		}
	
	RDbView view;
	CleanupClosePushL( view );

	User::LeaveIfError( view.Prepare( iMulticastDb, 
									   TDbQuery( aSqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	
	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL( colSet );
	TInt programIdColNo = colSet->ColNo( KIptvEpgProgramIdCol );
	TInt channelIdColNo = colSet->ColNo( KIptvEpgProgramChannelId );
	TInt serviceIdColNo = colSet->ColNo( KIptvEpgProgramServProviderIdCol );
	TInt uriColNo = colSet->ColNo( KIptvEpgProgramURICol );
	TInt genreColNo = colSet->ColNo( KIptvEpgProgramGenreCol );
	TInt nameColNo = colSet->ColNo( KIptvEpgProgramNameCol );
	TInt descrColNo = colSet->ColNo( KIptvEpgProgramDescriptionCol );
	TInt languageColNo = colSet->ColNo( KIptvEpgProgramLanguageCol );
	TInt parentalColNo = colSet->ColNo( KIptvEpgProgramParentalRatingCol );
	CleanupStack::PopAndDestroy( colSet );
	
	view.FirstL();
	while ( view.AtRow() )
		{
		view.GetL();
		CIptvEpgProgram* program = CIptvEpgProgram::NewL();
		CleanupStack::PushL( program );
		program->SetProgramId( view.ColInt64( programIdColNo ) );
		program->SetChannelId( view.ColInt64( channelIdColNo ) );
		program->SetServiceId( view.ColUint32( serviceIdColNo ) );
		program->SetProgramURI (view.ColDes( uriColNo ).AllocL() );
		program->SetProgramGenre( view.ColDes( genreColNo ).AllocL() );
		program->SetProgramName( view.ColDes( nameColNo ).AllocL() );
		program->SetProgramLanguage( view.ColDes( languageColNo ).AllocL() );
		program->SetProgramParentalRating( view.ColDes( parentalColNo ).AllocL() );
		
		// Description data is stored in long text field, so it needs to be read
		// through stream
		TInt descrLength = view.ColLength( descrColNo );
		HBufC* temp = HBufC::NewL( descrLength );
		CleanupStack::PushL( temp );
		RDbColReadStream read;
		read.OpenLC( view, descrColNo );
		TPtr ptr = temp->Des();
		read.ReadL( ptr, descrLength );
		read.Close();
		CleanupStack::PopAndDestroy(); // read;

		program->SetProgramDescription( temp ); // Ownership transferred
		CleanupStack::Pop( temp );

		// Since now ownership of CIptvEpgProgram object is in RPointerArray	
		aResultArray->AppendL( program );
		
		CleanupStack::Pop( program );
		
		view.NextL();
		}
	CleanupStack::PopAndDestroy( &view ); // closes view
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::FetchProgramsFromTableL out"));
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::FetchProgramNamesFromTableL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::FetchProgramNamesFromTableL( const TDesC& aSqlStatement,
							RPointerArray<CIptvEpgProgram>* aResultArray )
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::FetchProgramNamesFromTableL in"));

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	if ( !aResultArray ) 
		{
		User::Leave( KErrArgument );
		}
	RDbView view;
	CleanupClosePushL( view );

	User::LeaveIfError( view.Prepare( iMulticastDb, 
									   TDbQuery( aSqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	
	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL( colSet );
	TInt programIdColNo = colSet->ColNo( KIptvEpgProgramIdCol );
	TInt nameColNo = colSet->ColNo( KIptvEpgProgramNameCol );
	CleanupStack::PopAndDestroy( colSet );
	
	view.FirstL();
	TInt err( KErrNone );
	TLinearOrder<CIptvEpgProgram> order( 
						CIptvEpgProgram::LinearOrderOfProgramsByName );
	CIptvEpgProgram* program = NULL;

	while ( view.AtRow() )
		{
		view.GetL();
		program = CIptvEpgProgram::NewL();
		CleanupStack::PushL( program );
		program->SetProgramId( view.ColInt64( programIdColNo ) );
		program->SetProgramName( view.ColDes( nameColNo ).AllocL() );
		
		err = aResultArray->InsertInOrder( program, order );
		if ( err != KErrNone )
			{
			LIVE_TV_TRACE2(_L("CIptvEpgDatabase::FetchProgramNamesFromTableL() InsertInOrder FAILED: %d"), err);
			// KErrAlreadyExists == duplicate entry
			if ( err != KErrAlreadyExists ) 
				{
				CleanupStack::PopAndDestroy( program );
				User::Leave( err );
				}
			else
				{
				TInt index( aResultArray->FindInOrder( program, order ) );
				if ( index != KErrNotFound )
					{
					User::LeaveIfError( aResultArray->Insert( program, index ) );
					}
				else
					{
					LIVE_TV_TRACE1(_L("aResultArray->FindInOrder( program, order ) == KErrNotFound!)"));
					}
				}
			
			}
		CleanupStack::Pop( program );
		view.NextL();
		}
	CleanupStack::PopAndDestroy( &view ); // closes view
	LIVE_TV_TRACE2(_L("CIptvEpgDatabase::FetchProgramNamesFromTableL out, return %d items"), aResultArray->Count() );
	}


// ---------------------------------------------------------------------------
// CIptvEpgDatabase::FetchSchedulesFromTableL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::FetchSchedulesFromTableL( const TDesC& aSqlStatement,
							RPointerArray<CIptvEpgSchedule>* aResultArray )
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::FetchSchedulesFromTableL in"));

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	if ( !aResultArray )
		{
		User::Leave( KErrArgument );
		}
	RDbView view;
	CleanupClosePushL( view );

	User::LeaveIfError( view.Prepare( iMulticastDb, 
									  TDbQuery( aSqlStatement ) ) );
									  
	User::LeaveIfError( view.EvaluateAll() );
	
	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL( colSet );
	TInt programIdColNo = colSet->ColNo( KIptvEpgScheduleProgramIdCol );
	TInt channelIdColNo = colSet->ColNo( KIptvEpgScheduleChannelIdCol );
	TInt serviceIdColNo = colSet->ColNo( KIptvEpgScheduleServiceProviderIdCol );
	TInt startTimeColNo = colSet->ColNo( KIptvEpgScheduleStartTimeCol );
	TInt endTimeColNo = colSet->ColNo( KIptvEpgScheduleEndTimeCol );
	CleanupStack::PopAndDestroy( colSet );
	
	view.FirstL();
	while ( view.AtRow() )
		{
		view.GetL();
		CIptvEpgSchedule* schedule= CIptvEpgSchedule::NewL();
		CleanupStack::PushL( schedule );
		schedule->iProgramId = view.ColInt64( programIdColNo );
		schedule->iChannelId = view.ColInt64( channelIdColNo );
		schedule->iServiceId = view.ColUint32( serviceIdColNo );
		schedule->iStartTime = view.ColTime( startTimeColNo );
		schedule->iEndTime = view.ColTime( endTimeColNo );

		// Since now ownership of CIptvEpgProgram object is in RPointerArray	
		aResultArray->AppendL( schedule );
		
		CleanupStack::Pop( schedule );
		
		view.NextL();
		}
		
	CleanupStack::PopAndDestroy( &view ); // closes view
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::FetchSchedulesFromTableL out"));
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::FetchSearchSchedulesFromTableL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::FetchSearchSchedulesFromTableL( const TDesC& aSqlStatement,
						RPointerArray<CIptvEpgScheduleSearch>* aResultArray )
	{

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::FetchSearchSchedulesFromTableL in"));
	if ( !aResultArray )
		{
		User::Leave( KErrArgument );
		}
	RDbView view;
	CleanupClosePushL( view );

	User::LeaveIfError( view.Prepare( iMulticastDb, 
									  TDbQuery( aSqlStatement ) ) );
									  
	User::LeaveIfError( view.EvaluateAll() );
	
	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL( colSet );
	TInt programIdColNo = colSet->ColNo( KIptvEpgScheduleProgramIdCol );
	TInt channelIdColNo = colSet->ColNo( KIptvEpgScheduleChannelIdCol );
	//TInt serviceIdColNo = colSet->ColNo( KIptvEpgScheduleServiceProviderIdCol );
	TInt startTimeColNo = colSet->ColNo( KIptvEpgScheduleStartTimeCol );
	TInt endTimeColNo = colSet->ColNo( KIptvEpgScheduleEndTimeCol );

	CleanupStack::PopAndDestroy( colSet );

	CIptvEpgScheduleSearch* scheduleSearch = NULL;

	view.FirstL();
	while ( view.AtRow() )
		{
		view.GetL();
		scheduleSearch = CIptvEpgScheduleSearch::NewL();
		CleanupStack::PushL( scheduleSearch );
		scheduleSearch->iProgramId = view.ColInt64( programIdColNo );
		scheduleSearch->iChannelId = view.ColInt64( channelIdColNo );
		//scheduleSearch->iServiceId = view.ColUint32( serviceIdColNo );
		scheduleSearch->iStartTime = view.ColTime( startTimeColNo );
		scheduleSearch->iEndTime = view.ColTime( endTimeColNo );
		aResultArray->AppendL( scheduleSearch );
		// Since now ownership of object is in RPointerArray	
		CleanupStack::Pop( scheduleSearch );
		
		view.NextL();
		}
	CleanupStack::PopAndDestroy( &view ); // closes view
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::FetchSearchSchedulesFromTableL out"));
	}


// ---------------------------------------------------------------------------
// CIptvEpgDatabase::InsertOrUpdateScheduleL()
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::InsertOrUpdateScheduleL( const CIptvEpgSchedule& aSchedule )
	{

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	// Just add schedule because we are updating the epg to an empty database
	AddScheduleL( aSchedule );
	}
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::AddScheduleL()
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::AddScheduleL( const CIptvEpgSchedule& aSchedule )
	{

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	TBuf<50> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgScheduleTable );
    RDbView view;
    CleanupClosePushL( view );
    User::LeaveIfError( view.Prepare( iMulticastDb,
    					TDbQuery( sqlStatement ),
    					TDbWindow::EUnlimited,
    					RDbView::EInsertOnly ) );
    view.InsertL();
    
    CDbColSet* scheduleColSet = view.ColSetL();
    // Ownership of CDbColSet object is now here!
    CleanupStack::PushL( scheduleColSet );
    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleServiceProviderIdCol ), 
										 aSchedule.iServiceId );
    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleChannelIdCol ),
										 aSchedule.iChannelId );
    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleProgramIdCol ), 
										 aSchedule.iProgramId );
    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleStartTimeCol ), 
										 aSchedule.iStartTime );
    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleEndTimeCol ), 
										 aSchedule.iEndTime );
										 
	CleanupStack::PopAndDestroy( scheduleColSet );
	view.PutL();

	CleanupStack::PopAndDestroy( &view ); // closes view
	}
	
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::InsertOrUpdateChannelL()
// 
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::InsertOrUpdateChannelL(
								const CIptvEpgChannel& aChannel,
								TUint32& aChannelKey )
	{

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	// Just call AddChannelL because we are using an empty database
	AddChannelL( aChannel, aChannelKey );
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::AddSchedulesL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::AddSchedulesL( const RPointerArray<CIptvEpgSchedule>& aSchedules )
	{

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	TBuf<50> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgScheduleTable );
    RDbView view;
    CleanupClosePushL( view );
    User::LeaveIfError( view.Prepare( iMulticastDb,
    					TDbQuery( sqlStatement ),
    					TDbWindow::EUnlimited,
    					RDbView::EInsertOnly ) );
    
    CIptvEpgSchedule* schedule = NULL;
    LIVE_TV_TRACE2( _L("About to enter loop to insert %d schedules"), aSchedules.Count() );
    for( TInt i = 0; i < aSchedules.Count(); i++ )
    	{
    	schedule = aSchedules[i];
	    view.InsertL();
	    
	    CDbColSet* scheduleColSet = view.ColSetL();
	    // Ownership of CDbColSet object is now here!
	    CleanupStack::PushL( scheduleColSet );
	    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleServiceProviderIdCol ), 
											 schedule->iServiceId );
	    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleChannelIdCol ),
											 schedule->iChannelId );
	    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleProgramIdCol ), 
											 schedule->iProgramId );
	    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleStartTimeCol ), 
											 schedule->iStartTime );
	    view.SetColL( scheduleColSet->ColNo( KIptvEpgScheduleEndTimeCol ), 
											 schedule->iEndTime );
											 
		CleanupStack::PopAndDestroy( scheduleColSet );
		view.PutL();
		}

	// Compact database
	iMulticastDb.Compact();

	CleanupStack::PopAndDestroy( &view ); // closes view
	}
	

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::AddChannelL()
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::AddChannelL( const CIptvEpgChannel& aChannel,
											 TUint32& aChannelKey )
    {	 
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::AddChannelL()") );
    LIVE_TV_TRACE2( _L("aChannel.iChannelId = %Li"), aChannel.ChannelId() );

	if ( iLocalState == EBackup )
		{
		return;
		}
	
    RDbView view;
    CleanupClosePushL( view );
    User::LeaveIfError( view.Prepare( iMulticastDb,
    					TDbQuery( iSqlChannel ),
    					TDbWindow::EUnlimited,
    					RDbView::EInsertOnly ) );
    view.InsertL();
    
    CDbColSet* channelColSet = view.ColSetL();
    // Ownership of CDbColSet object is now here!
    CleanupStack::PushL( channelColSet );
    view.SetColL( channelColSet->ColNo( KIptvEpgChannelIdCol ), 
										aChannel.ChannelId() );
    view.SetColL( channelColSet->ColNo( KIptvEpgChannelServProviderIdCol ),
										aChannel.ServiceId() );
    if ( aChannel.ChannelName().Length() > KIptvEpgChannelNameMaxLength )
        {
        // Channel name is too long, cut it
        TBuf <KIptvEpgChannelNameMaxLength> channelName = 
            aChannel.ChannelName().Left( KIptvEpgChannelNameMaxLength );
        view.SetColL( channelColSet->ColNo( KIptvEpgChannelNameCol ), 
										    channelName );
        }
     else
        {
        view.SetColL( channelColSet->ColNo( KIptvEpgChannelNameCol ), 
										aChannel.ChannelName() );    
        }
    
    view.SetColL( channelColSet->ColNo( KIptvEpgChannelLogoPathCol ), 
										aChannel.ChannelLogoPath() );
    
    if ( aChannel.ChannelDescription().Length() > KIptvEpgDescrMaxLength )
        {
        // Channel description is too long, cut it
        TBuf<KIptvEpgDescrMaxLength> channelDesc = 
            aChannel.ChannelDescription().Left( KIptvEpgDescrMaxLength );
        view.SetColL( channelColSet->ColNo( KIptvEpgChannelDescriptionCol ), 
										channelDesc );
        }
    else
        {
        view.SetColL( channelColSet->ColNo( KIptvEpgChannelDescriptionCol ), 
										aChannel.ChannelDescription() );    
        }
    
    view.SetColL( channelColSet->ColNo( KIptvEpgChannelURICol ), 
										aChannel.ChannelURI() );
    
	// SDP is long one, it needs to be handled by stream
	RDbColWriteStream write;
	write.OpenLC( view, channelColSet->ColNo( KIptvEpgChannelSDPCol ) );
	write.WriteL( aChannel.ChannelSDP() );
	write.Close();
	CleanupStack::PopAndDestroy(); // write

    TInt keyColumnNo = channelColSet->ColNo( KIptvEpgChannelDbKeyCol );                                     
 	
	// Order
	view.SetColL( channelColSet->ColNo( KIptvEpgChannelOrderCol ),
				  aChannel.ChannelOrder() );
	CleanupStack::PopAndDestroy( channelColSet );
    view.PutL();

    // Get new channel key
    aChannelKey = view.ColUint32( keyColumnNo );
    
	CleanupStack::PopAndDestroy( &view ); // closes view

	// Finally update order numbers because new channel was addded
	UpdateChannelOrdersL( aChannel.ServiceId() );
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::InsertOrUpdateProgramL()
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::InsertOrUpdateProgramL(
										const CIptvEpgProgram& aProgram,
        							 	TUint32& aProgramKey )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}
	
	// Just add program because we are using an empty db
	AddProgramL( aProgram, aProgramKey );
	}


// ---------------------------------------------------------------------------
// CIptvEpgDatabase::AddProgramL()
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::AddProgramL( const CIptvEpgProgram& aProgram,
									TUint32& aProgramKey )
    {
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::AddProgramL() in"));
	if ( iLocalState == EBackup )
		{
		return;
		}
	
    RDbView view;
    TInt err = KErrNone;
    CleanupClosePushL( view );
    err = view.Prepare( iMulticastDb,
    					TDbQuery( iSqlProgram ),
    					TDbWindow::EUnlimited,
    					RDbView::EInsertOnly );
    if ( err != KErrNone )
    	{
    	LIVE_TV_TRACE1( _L(""));
    	LIVE_TV_TRACE2( _L("CIptvEpgDatabase::AddProgramL FAILED with code %d"), err );
    	LIVE_TV_TRACE1( _L(""));
    	User::Leave( err );
    	}
    view.InsertL();
    
    CDbColSet* programColSet = view.ColSetL();
    CleanupStack::PushL( programColSet );

	view.SetColL( programColSet->ColNo( KIptvEpgProgramIdCol ), 
										aProgram.ProgramId() );
	view.SetColL( programColSet->ColNo( KIptvEpgProgramChannelId ),
										aProgram.ChannelId() );
	view.SetColL( programColSet->ColNo( KIptvEpgProgramServProviderIdCol ),
										aProgram.ServiceId() );
	view.SetColL( programColSet->ColNo( KIptvEpgProgramURICol ), 
										aProgram.ProgramURI() );
	view.SetColL( programColSet->ColNo( KIptvEpgProgramGenreCol ), 
										aProgram.ProgramGenre() );
    if ( aProgram.ProgramName().Length() > KIptvEpgProgramMaxLength )
        {
        TBuf<KIptvEpgProgramMaxLength> programName = 
            aProgram.ProgramName().Left( KIptvEpgProgramMaxLength );
        view.SetColL( programColSet->ColNo( KIptvEpgProgramNameCol ), 
										programName );
        }
    else
        {
        view.SetColL( programColSet->ColNo( KIptvEpgProgramNameCol ), 
										aProgram.ProgramName() );    
        }
	
	view.SetColL( programColSet->ColNo( KIptvEpgProgramLanguageCol ), 
										aProgram.ProgramLanguage() );
	view.SetColL( programColSet->ColNo( KIptvEpgProgramParentalRatingCol ), 
										aProgram.ProgramParentalRating() );

	// Description is long one, it needs to be handled by stream
	RDbColWriteStream write;
	write.OpenLC( view, programColSet->ColNo( KIptvEpgProgramDescriptionCol ) );
	write.WriteL( aProgram.ProgramDescription() );
	write.Close();
	CleanupStack::PopAndDestroy(); // write

	TInt keyColumnNo = programColSet->ColNo( KIptvEpgProgramDbKeyCol );
    CleanupStack::PopAndDestroy( programColSet );
    view.PutL();

    // Get new program key
    aProgramKey = view.ColUint32( keyColumnNo );
    CleanupStack::PopAndDestroy( &view ); // closes view
    
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::AddProgramL() out"));
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::AddProgramsL()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::AddProgramsL(
						const RPointerArray<CIptvEpgProgram>& aPrograms )
	{
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::AddProgramsL() in"));
	
	if ( iLocalState == EBackup )
		{
		return;
		}
	
    RDbView view;
    TInt err = KErrNone;
    CleanupClosePushL( view );
    err = view.Prepare( iMulticastDb,
    					TDbQuery( iSqlProgram ),
    					TDbWindow::EUnlimited,
    					RDbView::EInsertOnly );
    if ( err != KErrNone )
    	{
    	LIVE_TV_TRACE1( _L(""));
    	LIVE_TV_TRACE2( _L("CIptvEpgDatabase::AddProgramsL FAILED with code %d"), err );
    	LIVE_TV_TRACE1( _L(""));
    	User::Leave( err );
    	}
    
    CIptvEpgProgram* program = NULL;
    LIVE_TV_TRACE2( _L("About to enter loop to insert %d programs to the database"), aPrograms.Count() );
    for( TInt i = 0; i < aPrograms.Count(); i++ )
    	{
	    view.InsertL();
    	program = aPrograms[i];
    	
	    CDbColSet* programColSet = view.ColSetL();
	    CleanupStack::PushL( programColSet );

		view.SetColL( programColSet->ColNo( KIptvEpgProgramIdCol ), 
											program->ProgramId() );
		view.SetColL( programColSet->ColNo( KIptvEpgProgramChannelId ),
											program->ChannelId() );
		view.SetColL( programColSet->ColNo( KIptvEpgProgramServProviderIdCol ),
											program->ServiceId() );
		view.SetColL( programColSet->ColNo( KIptvEpgProgramURICol ), 
											program->ProgramURI() );
		view.SetColL( programColSet->ColNo( KIptvEpgProgramGenreCol ), 
											program->ProgramGenre() );

        if ( program->ProgramName().Length() > KIptvEpgProgramMaxLength )
            {
            TBuf<KIptvEpgProgramMaxLength> programName = 
                program->ProgramName().Left( KIptvEpgProgramMaxLength );
            view.SetColL( programColSet->ColNo( KIptvEpgProgramNameCol ), 
                programName );
            }
        else
            {
            view.SetColL( programColSet->ColNo( KIptvEpgProgramNameCol ), 
                program->ProgramName() );
            }
		view.SetColL( programColSet->ColNo( KIptvEpgProgramLanguageCol ), 
											program->ProgramLanguage() );
		view.SetColL( programColSet->ColNo( KIptvEpgProgramParentalRatingCol ), 
											program->ProgramParentalRating() );

		// Description is long one, it needs to be handled by stream
		RDbColWriteStream write;
		write.OpenLC( view, programColSet->ColNo( KIptvEpgProgramDescriptionCol ) );
		write.WriteL( program->ProgramDescription() );
		write.Close();
		CleanupStack::PopAndDestroy(); // write
	    CleanupStack::PopAndDestroy( programColSet );
	    view.PutL();
    	}
    CleanupStack::PopAndDestroy( &view ); // closes view

	iMulticastDb.Compact();
    
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::AddProgramsL() out"));
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::AddProgramWithScheduleL()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::AddProgramWithScheduleL(
							CIptvEpgProgramWithSchedule& aProgramWithSchedule,
							TUint32& aProgramKey )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}
	
	AddProgramL( aProgramWithSchedule, aProgramKey );
	AddScheduleL( aProgramWithSchedule.Schedule() );
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::AddProgramsWithSchedulesL()
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::AddProgramsWithSchedulesL(
					const RPointerArray<CIptvEpgProgramWithSchedule>& aProgramsWithSchedules )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}
	
	RPointerArray<CIptvEpgSchedule> scheduleArray;
	CleanupClosePushL( scheduleArray );
	RPointerArray<CIptvEpgProgram> programArray;
	CleanupClosePushL( programArray );

	CIptvEpgProgramWithSchedule* progWithSchedule = NULL;
	TInt err( KErrNone );
	for ( TInt i = 0; i < aProgramsWithSchedules.Count() && err == KErrNone; i++ )
		{
		progWithSchedule = aProgramsWithSchedules[i];
		CIptvEpgSchedule& schedule = progWithSchedule->Schedule();
		err = scheduleArray.Append( &schedule );
		if ( err == KErrNone )
			{
			err = programArray.Append( progWithSchedule );
			}
		}
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("CIptvEpgDatabase::AddProgramsWithSchedulesL: Append in for-loop FAILED: %d"), err);
		scheduleArray.Reset();
		programArray.Reset();
		User::Leave( err );
		}
	if ( programArray.Count() > 0 )
		{
		AddProgramsL( programArray );
		if ( scheduleArray.Count() > 0 )
			{
			AddSchedulesL( scheduleArray );
			}
		}
	programArray.Reset();
	scheduleArray.Reset();
	CleanupStack::PopAndDestroy( &programArray );
	CleanupStack::PopAndDestroy( &scheduleArray );
	}

// ---------------------------------------------------------
// CIptvEpgDatabase::UpdateChannelOrderL()
//
// ---------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::UpdateChannelOrderL( const TUint32 aServiceId,
													 const TInt64  aChannelId,
													 const TUint32 aOrder )
    {
	if ( iLocalState == EBackup )
		{
		return;
		}
	
	LIVE_TV_TRACE4(_L("CIptvEpgDatabase::UpdateChannelOrderL in, aServiceId: %u, aChannelId: %Li, aOrder:%u"), aServiceId, aChannelId, aOrder );
    TBuf<KCustomSqlLength> sqlStatement;
    sqlStatement.Append( _L("SELECT ") );
	sqlStatement.Append( KIptvEpgChannelOrderCol );
    sqlStatement.Append( _L(" FROM ") );
    sqlStatement.Append( KIptvEpgChannelTable );
    sqlStatement.Append( _L(" WHERE ") );
    sqlStatement.Append( KIptvEpgChannelIdCol );
    sqlStatement.Append( _L(" = ") );
    sqlStatement.AppendNum( aChannelId );
    sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgChannelServProviderIdCol );
	sqlStatement.Append( _L(" = ") );
    sqlStatement.AppendNum( aServiceId );
   
    RDbView view;
    CleanupClosePushL( view );
	User::LeaveIfError( iMulticastDb.Begin() );
    User::LeaveIfError( view.Prepare( iMulticastDb,
    								  TDbQuery( sqlStatement ) ) );
    User::LeaveIfError( view.EvaluateAll() );
    
    // Ownership of this pointer is now here. It needs to be deleted also
    CDbColSet* channelColSet = view.ColSetL();
    CleanupStack::PushL( channelColSet );
    
	// sql statement found a row from table
    if ( view.FirstL() )
    	{
		view.UpdateL();
		
	    view.SetColL( channelColSet->ColNo( KIptvEpgChannelOrderCol ), 
					  aOrder );
		
		view.PutL();
    	}
    CleanupStack::PopAndDestroy( channelColSet );
    CleanupStack::PopAndDestroy( &view ); // closes view
	User::LeaveIfError( iMulticastDb.Commit() );
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::UpdateChannelOrderL() out"));
    }    

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::DeleteChannelL()
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::DeleteChannelL( 
										const TUint32 aServiceProviderId, 
										const TInt64 aChannelId )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}
	
	// First of all we need to delete all the programs which belongs to
	// channel identified by aServiceProviderId and aChannelId
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT ") );
	sqlStatement.Append( KIptvEpgProgramIdCol );
	sqlStatement.Append( _L(" FROM ") );
	sqlStatement.Append( KIptvEpgProgramTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgProgramChannelId );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aChannelId );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgProgramServProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );

	RDbView view;
	CleanupClosePushL( view );
	User::LeaveIfError( view.Prepare( iMulticastDb, 
									  TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	view.FirstL();
	CDbColSet* colSet = view.ColSetL();
	CleanupStack::PushL( colSet );
	TInt programIdColNo = colSet->ColNo( KIptvEpgProgramIdCol );
	CleanupStack::PopAndDestroy( colSet );
	
	RArray<TInt64> programIdArray;
	CleanupClosePushL( programIdArray );
	while ( view.AtRow() )
		{
		view.GetL();
		User::LeaveIfError( programIdArray.Append( 
								view.ColInt64( programIdColNo ) ) );
		view.NextL();
		}
	for ( TInt i = 0; i < programIdArray.Count(); i++ )
		{
		DeleteProgramL( aServiceProviderId, programIdArray[i] );
		}
	programIdArray.Reset();
	CleanupStack::PopAndDestroy( &programIdArray ); // closes programIdArray
	CleanupStack::PopAndDestroy( &view ); // closes view

	// Handle channel removal
	sqlStatement.Zero();
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgChannelTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgChannelIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aChannelId );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgChannelServProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );

	User::LeaveIfError( iMulticastDb.Begin() );
	CleanupClosePushL( view );
	User::LeaveIfError( view.Prepare( iMulticastDb, 
									  TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );

	while ( view.NextL() )
		{
		view.DeleteL();
		}
	CleanupStack::PopAndDestroy( &view ); // Closes view
	User::LeaveIfError( iMulticastDb.Commit() );

	// Update the channel order numbers
	UpdateChannelOrdersL( aServiceProviderId );
	}
	

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::UpdateChannelOrdersL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::UpdateChannelOrdersL( const TUint32 aServiceProviderId ) 
	{
	if ( iLocalState == EBackup )
		{
		return;
		}

	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT " ) );
	sqlStatement.Append( KIptvEpgChannelServProviderIdCol );
	sqlStatement.Append( _L(", "));
	sqlStatement.Append( KIptvEpgChannelIdCol );
	sqlStatement.Append( _L(", "));
	sqlStatement.Append( KIptvEpgChannelURICol );
	sqlStatement.Append( _L(", "));
	sqlStatement.Append( KIptvEpgChannelOrderCol );
	sqlStatement.Append( _L(" FROM ") );
	sqlStatement.Append( KIptvEpgChannelTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgChannelServProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );
	sqlStatement.Append( _L(" ORDER BY ") );
	sqlStatement.Append( KIptvEpgChannelOrderCol );
	
	RDbView view;
	CleanupClosePushL( view );
	User::LeaveIfError( view.Prepare( iMulticastDb, 
						TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	view.FirstL();
	CDbColSet* colSetOrder = view.ColSetL();
	CleanupStack::PushL( colSetOrder );
	const TInt serviceColNo = colSetOrder->ColNo( KIptvEpgChannelServProviderIdCol );
	const TInt channelIdColNo = colSetOrder->ColNo( KIptvEpgChannelIdCol );
	const TInt orderColNo = colSetOrder->ColNo( KIptvEpgChannelOrderCol );
	const TInt uriColNo = colSetOrder->ColNo( KIptvEpgChannelURICol );
	CleanupStack::PopAndDestroy( colSetOrder );

	RArray<TUint32> orderArray;
	CleanupClosePushL( orderArray );
	RArray<TUint32> serviceArray;
	CleanupClosePushL( serviceArray );
	RArray<TInt64> channelIdArray;
	CleanupClosePushL( channelIdArray );
    CDesCArray* uriArray = new ( ELeave ) CDesCArrayFlat( KIptvDefaultUriCount  );
	CleanupStack::PushL( uriArray );

	while ( view.AtRow() ) 
		{
		view.GetL();
		User::LeaveIfError( orderArray.Append( 
								view.ColUint32( orderColNo ) ) );
		User::LeaveIfError( serviceArray.Append( 
							view.ColUint32( serviceColNo ) ) );
		User::LeaveIfError( channelIdArray.Append( 
							view.ColInt64( channelIdColNo ) ) );
	    uriArray->AppendL( view.ColDes( uriColNo ) );
		view.NextL();
		}
	
	RArray<TUint32> zeroServiceIdArray;
	CleanupClosePushL( zeroServiceIdArray );
	RArray<TInt64> zeroChanIdArray;
	CleanupClosePushL( zeroChanIdArray );
	CDesCArray* zeroUriArray = new ( ELeave ) CDesCArrayFlat( KIptvDefaultUriCount  );
	CleanupStack::PushL( zeroUriArray );
    
	// Get all channels whose order is zero
	for ( TInt i = orderArray.Count() - 1; i >= 0; i-- ) 
		{
		if ( orderArray[i] == 0 ) 
			{
			if ( i < serviceArray.Count() && i < channelIdArray.Count() &&
			     i < uriArray->MdcaCount() )
			    {
			    zeroServiceIdArray.AppendL( serviceArray[i] );
			    zeroChanIdArray.AppendL( channelIdArray[i] );
			    zeroUriArray->AppendL( uriArray->MdcaPoint( i )  );    
			    }
			orderArray.Remove( i );
			serviceArray.Remove( i );
			channelIdArray.Remove( i );
			uriArray->Delete( i );
			}
		}
    TUint32 prev( 0 );
    TUint32 current( 0 );		
	// remove all deactivated channels
	// by default all channels are active so if there are
	// any channels that have been deactivated, it means that user
	// has deactivated them -> we don't need to care about
	// channels that have no stream URL
	for ( TInt j = orderArray.Count() - 1; j >= 0; j-- ) 
		{
		current = orderArray[j];
		if ( current == KMaxTUint32 ) 
			{
			orderArray.Remove( j );
			serviceArray.Remove( j );
			channelIdArray.Remove( j );
			uriArray->Delete( j );
			}
		else if ( j < uriArray->Count() && uriArray->MdcaPoint( j ).Length() == 0 )
		    {
		    // Channel has no stream URL, put those also to zero array
		    // they are handled later
		    zeroServiceIdArray.AppendL( serviceArray[j] );
		    zeroChanIdArray.AppendL( channelIdArray[j] );
		    zeroUriArray->AppendL( uriArray->MdcaPoint( j ) );
		    orderArray.Remove( j );
		    serviceArray.Remove( j );
		    channelIdArray.Remove( j );
		    uriArray->Delete( j );
		    }
		}
	// now orderArray should contain only activated channels
	// check if channel orders are "in order" i.e. UI does not want to see 
	// orders like 1, 2, 4, 5, 6 if one channel has been deleted.
    for ( TInt k = 0; k < orderArray.Count(); k++ ) 
        {
        current = orderArray[k];
        if ( k == 0 && current != 1 )
            {
            // First one is not number 1, fix
            orderArray[k] = 1;
            UpdateChannelOrderL( serviceArray[k], channelIdArray[k],
                                 orderArray[k] );
            }
        if ( k > 0 ) 
            {
            prev = orderArray[k-1];
            }
		
        if ( current < KMaxTUint32 && k > 0 && 
             prev < KMaxTUint32 )
            {
            const TInt offset( TInt( current - prev ) );

            if ( offset != 1 ) 
                {
                // channel orders have gone wrong, fix
                orderArray[k] = prev + 1;
                UpdateChannelOrderL( serviceArray[k], channelIdArray[k],
                                     orderArray[k] );
				}
			}
		}
	
	// now set channel order for channels whose order is 0 and
	// stream URL is available
	TUint32 max( 0 );
	if ( orderArray.Count() > 0  )
		{
        max = orderArray[orderArray.Count()-1];    
		}
    TInt l( 0 );
    for ( l = zeroServiceIdArray.Count() - 1; l >= 0; l-- )
        {
        if ( max < ( KMaxTUint32 - 1 ) ) 
            {
            if ( zeroUriArray->MdcaPoint( l ).Length() > 0 &&
			     max < ( KMaxTUint32 - 1 ) )
                {
                max++;
                UpdateChannelOrderL( zeroServiceIdArray[l], zeroChanIdArray[l], 
							 	 max );
                zeroServiceIdArray.Remove( l );
                zeroChanIdArray.Remove( l );
                zeroUriArray->Delete( l );
                }
		    }
        }
    // Let's move the channels that have no stream URL to last position
    // in active channel list
    for ( l = 0; l < zeroServiceIdArray.Count() && l < zeroUriArray->MdcaCount() &&
          l < zeroChanIdArray.Count(); l++ )
		{
		if ( zeroUriArray->MdcaPoint( l ).Length() == 0 && max < ( KMaxTUint32 - 1 ) )
		    {
		    max++;
		    UpdateChannelOrderL( zeroServiceIdArray[l], zeroChanIdArray[l],
		                         max );
		    }
		}    
    
    zeroServiceIdArray.Reset();
	zeroChanIdArray.Reset();
	orderArray.Reset();
	channelIdArray.Reset();
	serviceArray.Reset();
	
	CleanupStack::PopAndDestroy( zeroUriArray );
	CleanupStack::PopAndDestroy( &zeroChanIdArray );
	CleanupStack::PopAndDestroy( &zeroServiceIdArray );
	CleanupStack::PopAndDestroy( uriArray );
	CleanupStack::PopAndDestroy( &channelIdArray ); // Closes array
	CleanupStack::PopAndDestroy( &serviceArray ); // Closes array
	CleanupStack::PopAndDestroy( &orderArray ); // Closes array
	CleanupStack::PopAndDestroy( &view ); // Closes view
	}
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::DeleteProgramL()
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::DeleteProgramL( 
										const TUint32 aServiceProviderId,
										const TInt64 aProgramId )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}
	
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgProgramTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgProgramIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aProgramId );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgProgramServProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );

	User::LeaveIfError( iMulticastDb.Begin() );
	
	RDbView view;
	CleanupClosePushL( view );
	User::LeaveIfError( view.Prepare( iMulticastDb, 
									  TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );

	while ( view.NextL() )
		{
		view.DeleteL();
		}
	CleanupStack::PopAndDestroy( &view ); // closes view
	User::LeaveIfError( iMulticastDb.Commit() );
	}
    
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::ClearChannelTableL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::ClearChannelTableL( const TUint32 aServiceId )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}
	
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgChannelTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgChannelServProviderIdCol );
	sqlStatement.Append( _L(" = " ) );
	sqlStatement.AppendNum( aServiceId );
	
	RDbView view;
	CleanupClosePushL( view );
	User::LeaveIfError( iMulticastDb.Begin() );
	User::LeaveIfError( view.Prepare( iMulticastDb, 
									  TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	
	while ( view.NextL() )
		{
		view.DeleteL();
		}
	CleanupStack::PopAndDestroy( &view ); // closes view
	User::LeaveIfError( iMulticastDb.Commit() );
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::ClearProgramTableL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::ClearProgramTableL( const TUint32 aServiceId )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}
	
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgProgramTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgProgramServProviderIdCol );
	sqlStatement.Append( _L(" = " ) );
	sqlStatement.AppendNum( aServiceId );

	
	RDbView view;
	CleanupClosePushL( view );
	User::LeaveIfError( iMulticastDb.Begin() );
	User::LeaveIfError( view.Prepare( iMulticastDb, 
									  TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	while ( view.NextL() )
		{
		view.DeleteL();
		}
	CleanupStack::PopAndDestroy( &view ); // closes view
	User::LeaveIfError( iMulticastDb.Commit() );
	}
    
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::ClearScheduleTableL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::ClearScheduleTableL( const TUint32 aServiceId )
	{

	if ( iLocalState == EBackup )
		{
		return;
		}
	
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgScheduleTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgScheduleServiceProviderIdCol );
	sqlStatement.Append( _L(" = " ) );
	sqlStatement.AppendNum( aServiceId );

	
	RDbView view;
	CleanupClosePushL( view );
	User::LeaveIfError( iMulticastDb.Begin() );
	User::LeaveIfError( view.Prepare( iMulticastDb, 
									  TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	while ( view.NextL() )
		{
		view.DeleteL();
		}
	CleanupStack::PopAndDestroy( &view ); // closes view
	User::LeaveIfError( iMulticastDb.Commit() );
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::ClearLatestEpgTableL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::ClearLatestEpgTableL( const TUint32 aServiceId )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}
	
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgLatestEpgAvailableTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgLatestEpgAvailableServiceProviderIdCol );
	sqlStatement.Append( _L(" = " ) );
	sqlStatement.AppendNum( aServiceId );

	RDbView view;
	CleanupClosePushL( view );
	User::LeaveIfError( iMulticastDb.Begin() );
	User::LeaveIfError( view.Prepare( iMulticastDb, 
									  TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	while ( view.NextL() )
		{
		view.DeleteL();
		}
	CleanupStack::PopAndDestroy( &view ); // closes view
	User::LeaveIfError( iMulticastDb.Commit() );
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::ClearLastModifiedTableL()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::ClearLastModifiedTableL( const TUint32 aServiceId )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgLastModifiedTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgLastModifiedTableServiceIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceId );
	
	RDbView view;
	CleanupClosePushL( view );
	User::LeaveIfError( iMulticastDb.Begin() );
	User::LeaveIfError( view.Prepare( iMulticastDb,
									  TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	while ( view.NextL() )
		{
		view.DeleteL();
		}
	CleanupStack::PopAndDestroy( &view );
	User::LeaveIfError( iMulticastDb.Commit() );
	}


// ---------------------------------------------------------------------------
// CIptvEpgDatabase::MakeSqlStrings()
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::MakeSqlStrings()
    {   
    LIVE_TV_TRACE1(_L("CIptvEpgDatabase::MakeSqlStrings()"));
    // Channel
    iSqlChannel.Zero(); 
    iSqlChannel.Append( _L("SELECT * FROM " ) );
    iSqlChannel.Append( KIptvEpgChannelTable );   
    
    // Program
    iSqlProgram.Zero(); 
    iSqlProgram.Append( _L("SELECT * FROM ") );
    iSqlProgram.Append( KIptvEpgProgramTable );
    }

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetChannelL()
//
// ---------------------------------------------------------------------------
EXPORT_C CIptvEpgChannel* CIptvEpgDatabase::GetChannelL( TInt aService,
        								                 TInt64 aChannel )
	{
	if ( iLocalState == EBackup )
		{
		return NULL;
		}

	CIptvEpgChannel* retval = NULL;
	TBuf<KCustomSqlLength> sqlStatement;
    sqlStatement.Append( _L("SELECT * FROM "));
    sqlStatement.Append( KIptvEpgChannelTable );
    sqlStatement.Append( _L(" WHERE ") );
    sqlStatement.Append( KIptvEpgChannelServProviderIdCol );
    sqlStatement.Append( _L(" = ") );
    sqlStatement.AppendNum( aService );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgChannelIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aChannel );

	RPointerArray<CIptvEpgChannel> resultArray;
	CleanupClosePushL( resultArray );
	TRAPD( err, DoGetChannelsL( sqlStatement, &resultArray ) );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("DoGetChannelsL FAILED: %d"), err);
		resultArray.ResetAndDestroy();
		User::Leave( err );
		}
	const TInt c = resultArray.Count();
	if ( c > 0 )
		{
		retval = resultArray[0];        // note, index 0 is the retval, not deleted
		for ( TInt i( 1 ); i < c; i++ ) // note, starts from 1, not 0. 
			{
			delete resultArray[i]; 
			resultArray[i] = NULL;
			}
		}
	CleanupStack::PopAndDestroy( &resultArray ); // closes array
    return retval; 
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::UpdateChannelIconPathL()
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::UpdateChannelIconPathL( const TUint32 aServiceId,
									  const TInt64  aChannelId,
									  const TDesC&  aIconPath ) 
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::UpdateChannelIconPathL() in") );

	if ( iLocalState == EBackup )
		{
		return;
		}

	TBuf<KCustomSqlLength> sqlStatement;
    sqlStatement.Append( _L("SELECT ") );
	sqlStatement.Append( KIptvEpgChannelLogoPathCol );
	sqlStatement.Append( _L(" FROM ") );
    sqlStatement.Append( KIptvEpgChannelTable );
    sqlStatement.Append( _L(" WHERE ") );
    sqlStatement.Append( KIptvEpgChannelIdCol );
    sqlStatement.Append( _L(" = ") );
    sqlStatement.AppendNum( aChannelId );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgChannelServProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceId );
   
    RDbView view;
    CleanupClosePushL( view );
	User::LeaveIfError( iMulticastDb.Begin() );
    User::LeaveIfError( view.Prepare( iMulticastDb,
    								  TDbQuery( sqlStatement ) ) );
    User::LeaveIfError( view.EvaluateAll() );
    
    // Ownership of this pointer is now here. It needs to be deleted also
    CDbColSet* channelColSet = view.ColSetL();
    CleanupStack::PushL( channelColSet );
    
    // sql statement found a row from table
    if ( view.FirstL() )
    	{
    	view.UpdateL();
	    view.SetColL( channelColSet->ColNo( KIptvEpgChannelLogoPathCol ),
	    			  aIconPath );
	    view.PutL();
    	}
    CleanupStack::PopAndDestroy( channelColSet );
    CleanupStack::PopAndDestroy( &view ); // closes view
	User::LeaveIfError( iMulticastDb.Commit() );
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::UpdateChannelIconPathL() out") );
	}

// METHODS FOR LATEST EPG AVAILABLE TABLE

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CreateLatestEpgAvailableTableL()
//
// Creates database table for holding the start time of the latest epg data
// available for each service provider
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CreateLatestEpgAvailableTableL( 
									RDbNamedDatabase& aDatabase ) const
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::CreateLatestEpgAvailableTableL"));
	TDbCol keyCol( KIptvEpgLatestEpgAvailableTableKeyCol, EDbColUint32 );
	keyCol.iAttributes = TDbCol::EAutoIncrement;
	
	TDbCol serviceIdCol( KIptvEpgLatestEpgAvailableServiceProviderIdCol, EDbColUint32 );
	
	TDbCol startTimeCol( KIptvEpgLatestEpgAvailableStartTimeCol, EDbColDateTime );
	
    CDbColSet* latestEpgAvailableColSet = CDbColSet::NewLC();
    latestEpgAvailableColSet->AddL( keyCol );
    latestEpgAvailableColSet->AddL( serviceIdCol );
	latestEpgAvailableColSet->AddL( startTimeCol );

        
    User::LeaveIfError( aDatabase.CreateTable( KIptvEpgLatestEpgAvailableTable, 
    												*latestEpgAvailableColSet ) );
    CleanupStack::PopAndDestroy( latestEpgAvailableColSet );
	}
	
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::AddLatestEpgAvailableL
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::AddLatestEpgAvailableL
							( CIptvEpgLatestEpgAvailable& aLatestEpgAvailable )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}

	TBuf<50> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgLatestEpgAvailableTable );
    RDbView view;
    CleanupClosePushL( view );
    User::LeaveIfError( view.Prepare( iMulticastDb,
    					TDbQuery( sqlStatement ),
    					TDbWindow::EUnlimited,
    					RDbView::EInsertOnly ) );
    view.InsertL();
    
    CDbColSet* latestEpgAvailableColSet = view.ColSetL();
    // Ownership of CDbColSet object is now here!
    CleanupStack::PushL( latestEpgAvailableColSet );
    view.SetColL( latestEpgAvailableColSet->ColNo( KIptvEpgLatestEpgAvailableServiceProviderIdCol ), 
										 aLatestEpgAvailable.iServiceId );

    view.SetColL( latestEpgAvailableColSet->ColNo( KIptvEpgLatestEpgAvailableStartTimeCol ), 
										 aLatestEpgAvailable.iStartTime );
										 
	CleanupStack::PopAndDestroy( latestEpgAvailableColSet );
	view.PutL();

	CleanupStack::PopAndDestroy( &view ); // closes view
	}
	
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::InsertOrUpdateLatestEpgAvailableL()
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::InsertOrUpdateLatestEpgAvailableL
									( CIptvEpgLatestEpgAvailable& aLatestEpgAvailable )
	{
	if ( iLocalState == EBackup )
		{
		return;
		}

    TBuf<KCustomSqlLength> sqlStatement;
    sqlStatement.Append( _L("SELECT * FROM ") );
    sqlStatement.Append( KIptvEpgLatestEpgAvailableTable );
    sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgLatestEpgAvailableServiceProviderIdCol );
    sqlStatement.Append( _L(" = "));
	sqlStatement.AppendNum( aLatestEpgAvailable.GetServiceId() );

    RDbView view;
    CleanupClosePushL( view );
    User::LeaveIfError( view.Prepare( iMulticastDb,
    								  TDbQuery( sqlStatement ) ) );
    User::LeaveIfError( view.EvaluateAll() );
    
    // Ownership of this pointer is now here. It needs to be deleted also
    CDbColSet* latestEpgAvailableColSet = view.ColSetL();
    CleanupStack::PushL( latestEpgAvailableColSet );
    
    // sql statement found a row from table
    if ( view.FirstL() ) // Update
    	{
    	view.UpdateL();
    	
		view.SetColL( latestEpgAvailableColSet->ColNo
							( KIptvEpgLatestEpgAvailableServiceProviderIdCol ),
					  		 aLatestEpgAvailable.GetServiceId() );
					  
		view.SetColL( latestEpgAvailableColSet->ColNo( KIptvEpgLatestEpgAvailableStartTimeCol ),
					  aLatestEpgAvailable.GetStartTime() );

		view.PutL();
    	}	
	
	else
		{
		AddLatestEpgAvailableL( aLatestEpgAvailable );	
		}
	
	CleanupStack::PopAndDestroy( latestEpgAvailableColSet );
    CleanupStack::PopAndDestroy( &view ); // closes view
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetLatestScheduleTimeL
//
// ---------------------------------------------------------------------------
EXPORT_C TTime CIptvEpgDatabase::GetLatestScheduleTimeL( const TUint32 aServiceId )
	{
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::GetLatestScheduleTimeL IN") );
	if ( iLocalState == EBackup )
		{
		return TTime( 0 );
		}
	
    TBuf<KCustomSqlLength> sqlStatement;
    sqlStatement.Append( _L("SELECT * FROM ") );
    sqlStatement.Append( KIptvEpgLatestEpgAvailableTable );
    sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgLatestEpgAvailableServiceProviderIdCol );
    sqlStatement.Append( _L(" = "));
	sqlStatement.AppendNum( aServiceId );

    RDbView view;
    CleanupClosePushL( view );
    User::LeaveIfError( view.Prepare( iMulticastDb,
    								  TDbQuery( sqlStatement ) ) );
    User::LeaveIfError( view.EvaluateAll() );
    
    // Ownership of this pointer is now here. It needs to be deleted also
    CDbColSet* latestEpgAvailableColSet = view.ColSetL();
    CleanupStack::PushL( latestEpgAvailableColSet );
    TInt startTimeColNo = latestEpgAvailableColSet->ColNo( KIptvEpgLatestEpgAvailableStartTimeCol );
    
    TTime latestTime;
    // sql statement found a row from table,fetch that
    if ( view.FirstL() )  
    	{
		view.GetL();
		
		latestTime = view.ColTime( startTimeColNo );				
		}
	else
		{ 
		User::Leave( KErrNotFound );
		}	
	
	CleanupStack::PopAndDestroy( latestEpgAvailableColSet );
    CleanupStack::PopAndDestroy( &view ); // closes view
    return latestTime;
	}	    		
	
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::RemoveScheduledDataFromChannelL
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::RemoveScheduledDataFromChannelL( const CIptvEpgChannel& aChannel )
	{
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::RemoveScheduledDataFromChannelL IN") );
	if ( iLocalState == EBackup )
		{
		return;
		}

    TBuf<KCustomSqlLength> sqlStatement;
    _LIT(KDeleteStatementTemplate, "SELECT %S FROM %S WHERE %S = %u AND %S = %Li"); 
    sqlStatement.Format( KDeleteStatementTemplate,
    					 &KIptvEpgScheduleServiceProviderIdCol,
                         &KIptvEpgScheduleTable,
                         &KIptvEpgScheduleServiceProviderIdCol,
                         aChannel.ServiceId(),
                         &KIptvEpgScheduleChannelIdCol,
                         aChannel.ChannelId() );
	RDbView view;
	CleanupClosePushL( view );
	User::LeaveIfError( view.Prepare( iMulticastDb,
									  TDbQuery( sqlStatement ) ) );
	User::LeaveIfError( view.EvaluateAll() );
	User::LeaveIfError( iMulticastDb.Begin() );
	view.FirstL();
#if defined(LIVE_TV_FILE_TRACE) || defined(LIVE_TV_RDEBUG_TRACE)
	TInt deleteCount = 0; 
#endif
	while ( view.AtRow() )
		{
		view.DeleteL();
		view.NextL();
#if defined(LIVE_TV_FILE_TRACE) || defined(LIVE_TV_RDEBUG_TRACE)		
		deleteCount++; 
#endif
		}
#if defined(LIVE_TV_FILE_TRACE) || defined(LIVE_TV_RDEBUG_TRACE)				
	LIVE_TV_TRACE2(_L("RemoveScheduledDataFromChannelL removed %d schedules"), deleteCount );		
#endif
	CleanupStack::PopAndDestroy( &view ); // closes the view
	// iMulticastDb.Compact(); 
	User::LeaveIfError( iMulticastDb.Commit() );	
	}
	
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetSchedulesByProgIdL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CIptvEpgDatabase::GetSchedulesByProgIdL( 
								const TUint32 aServiceProviderId,
								const TInt64 aChannelKey,
								const TInt64 aProgId,
								RPointerArray<CIptvEpgSchedule>* aResultArray )
    {
    LIVE_TV_TRACE3(_L("CIptvEpgDatabase::GetSchedulesByProgIdL() ser %d program id %Li"), (int)aServiceProviderId, aProgId );

	if ( iLocalState == EBackup )
		{
		return;
		}
    
	if ( !aResultArray )
		{
		User::Leave( KErrArgument );
		}
	RPointerArray<CIptvEpgProgram> programs;
	CleanupClosePushL( programs );
    TRAPD( err, GetProgramsByChannelIdL( aServiceProviderId, aChannelKey, 
		   &programs ) ); 	
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("GetProgramsByChannelIdL FAILED: %d"), err) ;
		programs.ResetAndDestroy();
		User::Leave( err );
		}
    TLinearOrder<CIptvEpgProgram> o( CIptvEpgProgram::LinearOrderOfProgramsById ); 
	programs.Sort( o ); 

	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT * FROM ") );
	sqlStatement.Append( KIptvEpgScheduleTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgScheduleServiceProviderIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aServiceProviderId );
	sqlStatement.Append( _L(" AND ") );	
	sqlStatement.Append( KIptvEpgScheduleChannelIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aChannelKey );
	sqlStatement.Append( _L(" AND ") );
	sqlStatement.Append( KIptvEpgScheduleProgramIdCol );
	sqlStatement.Append( _L(" = ") );
	sqlStatement.AppendNum( aProgId );
		
    LIVE_TV_TRACE2(_L("query >%S<"), &sqlStatement);
    
	FetchSchedulesFromTableL( sqlStatement, aResultArray );
	
	const TInt c = aResultArray->Count(); 	

	CIptvEpgProgram* programToUseForFinding = CIptvEpgProgram::NewL(); 
	CleanupStack::PushL( programToUseForFinding ); 

	TInt indexFound( KErrNotFound ); 
	TInt64 programId( 0 );
	for ( TInt i = 0; i < c; i ++ ) 
		{
		programId = ((*aResultArray)[i])->GetProgramId();
		programToUseForFinding->SetProgramId( programId );
		indexFound = programs.FindInOrder( programToUseForFinding, o ); 
		if ( indexFound != KErrNotFound ) 
			{
			((*aResultArray)[i])->iProgram = programs[indexFound];
			programs.Remove( indexFound );
			}
		}
	programs.ResetAndDestroy();
	CleanupStack::PopAndDestroy( programToUseForFinding );
	CleanupStack::PopAndDestroy( &programs );
	LIVE_TV_TRACE2(_L("GetSchedulesByProgIdL getting %d schedules"),c);
    }

// METHODS FOR LAST MODIFIED TABLE
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::GetLastModifiedDataL
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::GetLastModifiedDataL( const TUint32 aServiceId,
											 		  TDes& aETag,
											 		  TDes& aLastModifiedDateTime )
	{
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::GetLastModifiedDataL IN") );
	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT ") );
	sqlStatement.Append( KIptvEpgLastModifiedTableETagCol );
	sqlStatement.Append( _L(" , ") );
	sqlStatement.Append( KIptvEpgLastModifiedTableTimeCol );
	sqlStatement.Append( _L(" FROM ") );
	sqlStatement.Append( KIptvEpgLastModifiedTable );
	sqlStatement.Append( _L(" WHERE ") );
	sqlStatement.Append( KIptvEpgLastModifiedTableServiceIdCol );
	sqlStatement.Append( _L(" = " ) );
	sqlStatement.AppendNum( aServiceId );

	LIVE_TV_TRACE2( _L("SQL-statement = %S"), &sqlStatement );

    RDbView view;
    CleanupClosePushL( view );
    TInt err( KErrNone );
    err = view.Prepare( iMulticastDb, TDbQuery( sqlStatement ) );
    LIVE_TV_TRACE2( _L("view.Prepare returned %d"), err );
    if ( err != KErrNone )
    	{
    	User::Leave( err );
    	}
    err = view.EvaluateAll();
    LIVE_TV_TRACE2( _L("view.EvaluateAll() returned %d"), err );
    if ( err != KErrNone )
    	{
    	User::Leave( err );
    	}
    
    
    // Ownership of this pointer is now here. It needs to be deleted also
    CDbColSet* lastModifiedColSet = view.ColSetL();
    CleanupStack::PushL( lastModifiedColSet );
    TInt lastModifiedDateTimeColNo = lastModifiedColSet->ColNo( KIptvEpgLastModifiedTableTimeCol );
    TInt eTagColNo = lastModifiedColSet->ColNo( KIptvEpgLastModifiedTableETagCol );

    // sql statement found a row from table,fetch that
    if ( view.FirstL() )
    	{
		view.GetL();
		
		aLastModifiedDateTime = view.ColDes( lastModifiedDateTimeColNo );
		aETag = view.ColDes( eTagColNo );
		}
	
	CleanupStack::PopAndDestroy( lastModifiedColSet );
    CleanupStack::PopAndDestroy( &view ); // closes view
    LIVE_TV_TRACE3( _L("on return aETag = %S and aLastModifiedDateTime = %S"), &aETag, &aLastModifiedDateTime );
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::GetLastModifiedDataL OUT") );
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::SetLastModifiedDataL
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::SetLastModifiedDataL( const TUint32 aServiceId,
													  const TDesC& aETag,
													  const TDesC& aLastModifiedDateTime )
	{
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::SetLastModifiedDataL IN") );
	if ( iLocalState == EBackup )
		{
		return;
		}

	LIVE_TV_TRACE3( _L("aETag = %S, aLastModifiedDateTime = %S"), &aETag, &aLastModifiedDateTime );

	// We can safely clear this because there is (or at least should) only one row
	// in this table. This is because there is individual database for each and
	// every service, one service has only one service address.
	ClearLastModifiedTableL( aServiceId );

	TBuf<KCustomSqlLength> sqlStatement;
	sqlStatement.Append( _L("SELECT ") );
	sqlStatement.Append( KIptvEpgLastModifiedTableServiceIdCol );
	sqlStatement.Append( _L(" , ") );
	sqlStatement.Append( KIptvEpgLastModifiedTableETagCol );
	sqlStatement.Append( _L(" , ") );
	sqlStatement.Append( KIptvEpgLastModifiedTableTimeCol );
	sqlStatement.Append( _L(" FROM ") );
	sqlStatement.Append( KIptvEpgLastModifiedTable );
    RDbView view;
    CleanupClosePushL( view );
    User::LeaveIfError( view.Prepare( iMulticastDb,
    					TDbQuery( sqlStatement ),
    					TDbWindow::EUnlimited,
    					RDbView::EInsertOnly ) );
    view.InsertL();
    
    // Ownership of CDbColSet object is now here!
	CDbColSet* lastModifiedColSet = view.ColSetL();
    CleanupStack::PushL( lastModifiedColSet );

    // Get the column numbers we need here
    TInt servIdColNo = lastModifiedColSet->ColNo( KIptvEpgLastModifiedTableServiceIdCol );
    TInt eTagColNo = lastModifiedColSet->ColNo( KIptvEpgLastModifiedTableETagCol );
    TInt lastModifiedDateTimeCol = lastModifiedColSet->ColNo( KIptvEpgLastModifiedTableTimeCol );
    
    // Set values to the columns
    view.SetColL( servIdColNo, aServiceId );
    view.SetColL( eTagColNo, aETag );
    view.SetColL( lastModifiedDateTimeCol, aLastModifiedDateTime );
    
    // Delete allocated object
    CleanupStack::PopAndDestroy( lastModifiedColSet );
    
    // Commit changes to the database
    view.PutL();
    
    CleanupStack::PopAndDestroy( &view );
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::SetLastModifiedDataL OUT") );
	}

// ---------------------------------------------------------------------------
// CIptvEpgDatabase::ClearServiceEPGDataL
//
// ---------------------------------------------------------------------------
EXPORT_C void CIptvEpgDatabase::ClearServiceEPGDataL( const TUint32 aServiceId )
	{
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::ClearServiceEPGDataL in"));

	if ( iLocalState == EBackup )
		{
		return;
		}

	ClearChannelTableL( aServiceId );

	ClearProgramTableL( aServiceId );

	ClearScheduleTableL( aServiceId );

	ClearLatestEpgTableL( aServiceId );
	
	ClearLastModifiedTableL( aServiceId );

	User::LeaveIfError( iMulticastDb.Compact() );
	LIVE_TV_TRACE1(_L("CIptvEpgDatabase::ClearServiceEPGDataL out"));
	}
	
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::ChangeFileLockL
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::ChangeFileLockL( const TDesC& aFileName, TFileLockFlags aFlags )
	{
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::ChangeFileLockL IN") );
	if ( aFileName.Compare( iDbFile ) == 0 )
		{
		switch( aFlags )
			{
			case MBackupObserver::ETakeLock:
			    {
			    LIVE_TV_TRACE1( _L("aFlags = MBackupObserver::ETakeLock") );
				CreateDbL();
				CreateMulticastDbSessionL();    
				iLocalState = EReady;
				LIVE_TV_TRACE1( _L("Setting iLocalState to EReady") );
				if ( iActiveWait->IsStarted() )
					{
					LIVE_TV_TRACE1( _L("Stopping iActiveWait") );
					iActiveWait->AsyncStop();
					}
			    }
				break;
			case MBackupObserver::EReleaseLockNoAccess:
			    {
			    LIVE_TV_TRACE1( _L("aFlags = MBackupObserver::EReleaseLockNoAccess") );
				CloseMulticastDbSession();
				LIVE_TV_TRACE1( _L("Setting iLocalState to EBackup") );
				iLocalState = EBackup;
			    }
				break;
			default:
				break;
			}
		}
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::ChangeFileLockL OUT") );
	}
	
// ---------------------------------------------------------------------------
// CIptvEpgDatabase::CheckBackupStatus
//
// ---------------------------------------------------------------------------
void CIptvEpgDatabase::CheckBackupStatus()
	{
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::CheckBackupStatus IN") );
	if ( iLocalState == EBackup )
		{
		// Start waiting to see backup ending to continue.
		if ( !iActiveWait->IsStarted() )
			{
			LIVE_TV_TRACE1( _L("iLocalState == EBackup -> starting active wait") );
			iActiveWait->Start();	
			}
		else
			{
			LIVE_TV_TRACE1(_L("CIptvEpgDatabase::CheckBackupStatus: iActiveWait already started!"));
			}
		}
	LIVE_TV_TRACE1( _L("CIptvEpgDatabase::CheckBackupStatus OUT") );
	}

//  End of File  
