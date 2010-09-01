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
* Description:    Offers interface to IPTV Settings database*
*/




// INCLUDE FILES
#include <bautils.h>    // file helpers
#include "IptvLiveLogger.h"
#include "CIptvSettingsDatabase.h"

// CONSTANTS

// Column numbers
const TInt KIptvParentalControlColNo = 1;
const TInt KIptvRecordingTimeColNo = 2;
const TInt KIptvReminderTimeColNo = 3;
const TInt KIptvLastUsedServiceColNo = 4;
const TInt KIptvLastUsedChannelColNo = 5;
const TInt KIptvIsLastUsedColNo = 6;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::CIptvSettingsDatabase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CIptvSettingsDatabase::CIptvSettingsDatabase( const TFileName& aDbFile ) : 
	iDbFile( aDbFile )
    {
    }

// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::ConstructL
// Second phase construction. Leaves, if RFs session cannot be created.
// -----------------------------------------------------------------------------
void CIptvSettingsDatabase::ConstructL()
	{
	LIVE_TV_TRACE1(_L("CIptvSettingsDatabase::ConstructL()"));
    User::LeaveIfError( iFsSession.Connect() );
    CreateDbL();
	TRAPD( error, CreateDbSessionL() );
	if ( error != KErrNone )
        {
        LIVE_TV_TRACE2(_L("CIptvSettingsDatabase:: Could not open session to db (%d)"), error);

        if ( error != KErrNoMemory && 
             error != KErrLocked && 
             error != KErrDisMounted &&
             error != KErrDiskFull &&
             error != KErrNotReady )
            {
            // Delete and recreate database file. Cannot recover other way. 
            LIVE_TV_TRACE1(_L("CIptvSettingsDatabase:: fatal error occured while opening db, recreating db"));
            
            iFsSession.Delete( iDbFile ); //ignore error
            
            TRAP( error, CreateDbL() );
            if ( error != KErrNone )
                {
                LIVE_TV_TRACE2(_L("CIptvSettingsDatabase:: couldnt recreate db (%d), leaving."), error);
                User::Leave( error );
                }
                
            TRAP( error, CreateDbSessionL() );
            if ( error != KErrNone )
                {
                LIVE_TV_TRACE2(_L("CIptvSettingsDatabase:: couldnt open session to db (%d), leaving."), error);
                User::Leave( error );
                }
                
            }
        else
            {
            LIVE_TV_TRACE1(_L("CIptvSettingsDatabase:: temporary error occured while opening db, leaving db intact, leaving."));
            User::Leave( error );
            }
        }
	}

// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CIptvSettingsDatabase* CIptvSettingsDatabase::NewL( 
									const TFileName& aDbFile )
    {
    LIVE_TV_TRACE1(_L("CIptvSettingsDatabase::NewL()"));
    CIptvSettingsDatabase* self = new ( ELeave ) CIptvSettingsDatabase( aDbFile );    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );    
    return self;
    }
    
// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::~CIptvSettingsDatabase()
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C CIptvSettingsDatabase::~CIptvSettingsDatabase()
    {
    LIVE_TV_TRACE1(_L("CIptvSettingsDatabase::~CIptvSettingsDatabase in"));    
    iFsSession.Close(); 
    iDb.Close();
    iDbSession.Close();
    LIVE_TV_TRACE1(_L("CIptvSettingsDatabase::~CIptvSettingsDatabase out"));    
    }

// ---------------------------------------------------------------------------
// CIptvSettingsDatabase::CreateDbL()
//
// Create a new database. 
// ---------------------------------------------------------------------------
void CIptvSettingsDatabase::CreateDbL()
    {
    LIVE_TV_TRACE1(_L("CIptvSettingsDatabase::CreateDbL()"));
    if ( !BaflUtils::FileExists( iFsSession, iDbFile ) )
        {
        LIVE_TV_TRACE1( _L("Database file wasn't found, creating new"));
    	RDbNamedDatabase database;
		CleanupClosePushL( database );
    	User::LeaveIfError( database.Create( iFsSession, iDbFile ) );
		CreateTableL( database );
        SetDefaultValuesL( database );
        CleanupStack::PopAndDestroy( &database );
        }
    }

// ---------------------------------------------------------------------------
// CIptvSettingsDatabase::CreateDbSessionL()
//
// Open a new database session and the database
// ---------------------------------------------------------------------------
void CIptvSettingsDatabase::CreateDbSessionL()
    {
    LIVE_TV_TRACE1(_L("CIptvSettingsDatabase::CreateDbSessionL()"));
    User::LeaveIfError( iDbSession.Connect() );
    User::LeaveIfError( iDb.Open( iDbSession, iDbFile ) );
    User::LeaveIfError( iDb.Compact() );
    }

// ---------------------------------------------------------------------------
// CIptvSettingsDatabase::CreateTableL()
//
// Creates table. Leaves, if the table cannot be created.
// ---------------------------------------------------------------------------
void CIptvSettingsDatabase::CreateTableL( RDbNamedDatabase& aDatabase ) const
    {
	LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::CreateTableL in" ) );
    TDbCol column;
    CDbColSet* columns = CDbColSet::NewLC();
    
    // create columns and add them to the column set
    column = TDbCol( KIptvParentalControlCol, EDbColInt32 );
    columns->AddL( column );
                
    column = TDbCol( KIptvRecordingTimeCol, EDbColInt32 );
    columns->AddL( column );

    column = TDbCol( KIptvReminderTimeCol, EDbColInt32 );
    columns->AddL( column );

    // last used service+channel can be here in creating db
    column = TDbCol( KIptvLastUsedServiceCol, EDbColUint32 );
    columns->AddL( column );

    column = TDbCol( KIptvLastUsedChannelCol, EDbColInt64 );
    columns->AddL( column );

    column = TDbCol( KIptvIsLastUsedCol, EDbColInt32 );
    columns->AddL( column );

    // create table
    User::LeaveIfError( aDatabase.CreateTable( KIptvSettingsTable, *columns ) );

    CleanupStack::PopAndDestroy( columns );
    LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::CreateTableL out" ) ); 
    }

// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::SaveSettingsL
// Writes settings to the database.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvSettingsDatabase::SaveSettingsL( 
									const TIptvLiveUISettings& aSettings )
    {
    RDbView view;
    CleanupClosePushL( view );

    User::LeaveIfError( view.Prepare( iDb, TDbQuery( KIptvQuerySettingsSql ), 
        TDbWindow::EUnlimited ) );
    User::LeaveIfError( view.EvaluateAll() );

    view.FirstL();
	if ( view.AtRow() )
		{
		view.UpdateL();
	    // write given settings to the database
	    view.SetColL( KIptvParentalControlColNo, aSettings.iParentalControl ); 
	    view.SetColL( KIptvRecordingTimeColNo, aSettings.iRecordingTimeReserve );
	    view.SetColL( KIptvReminderTimeColNo, aSettings.iReminderTime );
	    
	    view.PutL();	
		}
    CleanupStack::PopAndDestroy( &view );    // closes the view
    }   
    
// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::ReadSettingsL
// Reads settings from the database.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvSettingsDatabase::ReadSettingsL( TIptvLiveUISettings& aSettings )
    {
    RDbView view;
    CleanupClosePushL( view );

    User::LeaveIfError( view.Prepare( iDb, TDbQuery( KIptvQuerySettingsSql ), 
        TDbWindow::EUnlimited ) );
    User::LeaveIfError( view.EvaluateAll() );
    
    view.NextL();
	if ( view.AtRow() )
		{
		view.GetL();
	    // write settings to the settings structure
	    aSettings.iParentalControl = view.ColInt( KIptvParentalControlColNo );
	    aSettings.iRecordingTimeReserve = view.ColInt( KIptvRecordingTimeColNo );
	    aSettings.iReminderTime = view.ColInt( KIptvReminderTimeColNo );
		}
    CleanupStack::PopAndDestroy( &view );    // closes the view
    }

// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::SetDefaultValuesL
// Sets default values to the database.
// -----------------------------------------------------------------------------
//
void CIptvSettingsDatabase::SetDefaultValuesL( 
    RDbNamedDatabase& aDatabase ) const
    {
    LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::SetDefaultValuesL in" ) );
    RDbView view;
    CleanupClosePushL( view );

    User::LeaveIfError( view.Prepare( aDatabase, 
		TDbQuery( KIptvQuerySettingsSql ), TDbWindow::EUnlimited ) );
    User::LeaveIfError( view.EvaluateAll() );

    view.InsertL();
    // default values, settings
    view.SetColL( KIptvParentalControlColNo, KIptvDefaultParentalControl );
    view.SetColL( KIptvRecordingTimeColNo, KIptvDefaultRecordingTimeReserve );
    view.SetColL( KIptvReminderTimeColNo, KIptvDefaultReminderTime );    

    view.SetColL( KIptvLastUsedServiceColNo, KIptvDefaultLastUsedServiceId );
    view.SetColL( KIptvLastUsedChannelColNo, KIptvDefaultLastUsedChannelId );
    view.SetColL( KIptvIsLastUsedColNo, KIptvDefaultIsLastUsed );    

    view.PutL();      

    CleanupStack::PopAndDestroy( &view );    // closes the view
    LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::SetDefaultValuesL out" ) );
    }   

// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::SaveLastUsageL
// Writes last usage to the database, separated from actual settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvSettingsDatabase::SaveLastUsageL( 
									const TIptvLiveUISettings& aSettings )
    {
    LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::SaveLastUsageL in" ) );    
    RDbView view;
    CleanupClosePushL( view );

    User::LeaveIfError( view.Prepare( iDb, TDbQuery( KIptvQuerySettingsSql ), 
        TDbWindow::EUnlimited ) );
    User::LeaveIfError( view.EvaluateAll() );

    // Reset value, which is for checking Active Space UI at first 
    view.FirstL();		
    while ( view.AtRow() )
        {
        view.GetL();            
        view.UpdateL();
    	view.SetColL( KIptvIsLastUsedColNo, KIptvDefaultIsLastUsed );
    	view.PutL();
        view.NextL();
        }

    TBool found( EFalse );
    view.FirstL();		
    while ( view.AtRow() && !found )
        {
        view.GetL();
        if ( aSettings.iLastUsedServiceId == 
            view.ColUint32( KIptvLastUsedServiceColNo ) )
            {
    	    // Write ch id to the database            
    		view.UpdateL();
    	    view.SetColL( KIptvLastUsedChannelColNo, 
    	        aSettings.iLastUsedChannelId );
    	    // Set this for the Active Space UI row    
    	    view.SetColL( KIptvIsLastUsedColNo, 1 );    	        
    	    view.PutL();                
            found = ETrue;
            }
        view.NextL();
        }
	
	// New service id, add new	
	if ( !found )
	    {
		view.InsertL();
        view.SetColL( KIptvLastUsedServiceColNo, aSettings.iLastUsedServiceId );
	    view.SetColL( KIptvLastUsedChannelColNo, aSettings.iLastUsedChannelId );
    	view.SetColL( KIptvIsLastUsedColNo, 1 );	    
	    view.PutL();	    
	    }
			
    CleanupStack::PopAndDestroy( &view );    // closes the view
    
    LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::SaveLastUsageL out" ) );
    }   
    
// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::ReadLastUsageByServiceL
// Reads last watched channel by given service from the database. Separated 
// from actual settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvSettingsDatabase::ReadLastUsageByServiceL( 
    const TUint32 aServiceId, TIptvLiveUISettings& aSettings )
    {
    LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::ReadLastUsageByServiceL in" ) );    
    RDbView view;
    CleanupClosePushL( view );

    User::LeaveIfError( view.Prepare( iDb, TDbQuery( KIptvQuerySettingsSql ), 
        TDbWindow::EUnlimited ) );    
    User::LeaveIfError( view.EvaluateAll() );
    
    TBool found( EFalse );
    view.FirstL();
    while ( view.AtRow() && !found )
        {
        view.GetL();
        // Service found, get ch id
        if ( aServiceId == view.ColUint32( KIptvLastUsedServiceColNo ) )
            {
    	    // write last usage values to the settings structure
    	    aSettings.iLastUsedChannelId = 
    	        view.ColInt64( KIptvLastUsedChannelColNo );                
            found = ETrue;
            }
        view.NextL();
        }
    CleanupStack::PopAndDestroy( &view );    // closes the view
    
    // Last usage not found, leave so that values will be zero
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
    
    LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::ReadLastUsageByServiceL out" ) );
    }

// -----------------------------------------------------------------------------
// CIptvSettingsDatabase::ReadLastUsageL
// Reads the actual last watched channel and service from the database. 
// Separated from actual settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvSettingsDatabase::ReadLastUsageL( 
    TIptvLiveUISettings& aSettings )
    {
    LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::ReadLastUsageL in" ) );    
    RDbView view;
    CleanupClosePushL( view );

    User::LeaveIfError( view.Prepare( iDb, TDbQuery( KIptvQuerySettingsSql ), 
        TDbWindow::EUnlimited ) );    
    User::LeaveIfError( view.EvaluateAll() );
    
    TBool found( EFalse );
    view.FirstL();
    while ( view.AtRow() && !found )
        {
        view.GetL();
        // Space UI value found
        if ( view.ColInt32( KIptvIsLastUsedColNo ) == 1 )
            {
    	    // write last usage values to the settings structure
    	    aSettings.iLastUsedServiceId = 
    	        view.ColUint32( KIptvLastUsedServiceColNo );
    	    aSettings.iLastUsedChannelId = 
    	        view.ColInt64( KIptvLastUsedChannelColNo );                
            found = ETrue;
            }
        view.NextL();
        }
    CleanupStack::PopAndDestroy( &view );    // closes the view
    
    // Last usage not found, leave so that values will be zero
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
    
    LIVE_TV_TRACE1( _L("CIptvSettingsDatabase::ReadLastUsageL out" ) );
    }

//  End of File
