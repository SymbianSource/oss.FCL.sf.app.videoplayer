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
* Description: 
*
*/




#include "CIptvEpgFileSwapper.h"
#include "IptvLiveLogger.h"
#include <babackup.h>					// For CBaBackupSessionWrapper
#include "MIptvEpgFileSwapObserver.h"
#include <bautils.h>					// For BaflUtils

// Constants
const TInt KMicrosecondsToWasteAtOnce = 2000000; // 0.2 seconds
const TInt KMaxSwapTries = 10; // We wait max 10*0.2 seconds for file releases

// -----------------------------------------------------------------------------
// CIptvEpgFileSwapper::NewL
// Symbian two phase constructor
// -----------------------------------------------------------------------------
EXPORT_C CIptvEpgFileSwapper* CIptvEpgFileSwapper::NewL( const TFileName& aOldFileName,
														 const TFileName& aNewFileName,
														 MIptvEpgFileSwapObserver& aSwapObserver )
	{
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::NewL IN") );
	CIptvEpgFileSwapper* self = new (ELeave) CIptvEpgFileSwapper( aOldFileName,
																  aNewFileName,
																  aSwapObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::NewL OUT") );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CIptvEpgFileSwapper::CIptvEpgFileSwapper
// Default constructor
// -----------------------------------------------------------------------------
CIptvEpgFileSwapper::CIptvEpgFileSwapper( const TFileName& aOldFileName,
										  const TFileName& aNewFileName,
										  MIptvEpgFileSwapObserver& aSwapObserver ) :
										  CActive( EPriorityLow ),
										  iOldFileName( aOldFileName ),
										  iNewFileName( aNewFileName ),
										  iSwapObserver( aSwapObserver )
	{
	}
	
// -----------------------------------------------------------------------------
// CIptvEpgFileSwapper::ConstructL
// Second phase construction.
// -----------------------------------------------------------------------------
void CIptvEpgFileSwapper::ConstructL()
	{
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::ConstructL IN") );
	// First create backupwrapper
	iBackupWrapper = CBaBackupSessionWrapper::NewL();
	User::LeaveIfError( iTimeWaster.CreateLocal() );
	User::LeaveIfError( iFs.Connect() );
	CActiveScheduler::Add( this );
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::ConstructL OUT") );
	}
	
// -----------------------------------------------------------------------------
// CIptvEpgFileSwapper::~CIptvEpgFileSwapper
// Destructor
// -----------------------------------------------------------------------------
CIptvEpgFileSwapper::~CIptvEpgFileSwapper()
	{
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::~CIptvEpgFileSwapper IN") );
	Cancel();
	iTimeWaster.Close();
	iFs.Close();
	delete iBackupWrapper;
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::~CIptvEpgFileSwapper OUT") );
	}
	
// -----------------------------------------------------------------------------
// CIptvEpgFileSwapper::SwapFilesL
// 
// -----------------------------------------------------------------------------
EXPORT_C void CIptvEpgFileSwapper::SwapFilesL()
	{
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::SwapFilesL IN") );
	// If one of the files to be swapped does not exist what do people expect
	// us to do here? Yes, we'll leave!
	if( !BaflUtils::FileExists( iFs, iNewFileName ) ||
		!BaflUtils::FileExists( iFs, iOldFileName ) )
		{
		LIVE_TV_TRACE1( _L("Either iNewFileName or iOldFileName was not found -> we are leaving with KErrPathNotFound") );
		User::Leave( KErrPathNotFound );
		}
	// Request backup wrapper to signalize observers to close handles to
	// files to allow swapping.
	iBackupWrapper->CloseFileL( iNewFileName, MBackupObserver::EReleaseLockNoAccess );
	iBackupWrapper->CloseFileL( iOldFileName, MBackupObserver::EReleaseLockNoAccess );
	Cancel();
	iTimeWaster.After( iStatus, KMicrosecondsToWasteAtOnce );
	SetActive();
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::SwapFilesL OUT") );
	}
	
// -----------------------------------------------------------------------------
// CIptvEpgFileSwapper::ReleaseFileLocks
// 
// -----------------------------------------------------------------------------
void CIptvEpgFileSwapper::ReleaseFileLocks()
	{
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::ReleaseFileLocks IN") );
	iBackupWrapper->RestartFile( iOldFileName );
	iBackupWrapper->RestartFile( iNewFileName );
	iSwapTriesAmount = 0;
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::ReleaseFileLocks OUT") );
	}
	
// Methods from CActive
// -----------------------------------------------------------------------------
// CIptvEpgFileSwapper::RunL
// 
// -----------------------------------------------------------------------------
TInt CIptvEpgFileSwapper::RunError( TInt aError )
	{
	LIVE_TV_TRACE2( _L("CIptvEpgFileSwapper::RunError( %d ) IN"), aError );
	if( aError != KErrNone )
		{
		// What shall we do... Any ideas? Anyone?
		}
	// In any case we need to cancel backup requests -> Tell observer to allow
	// files to be opened again.
	ReleaseFileLocks();
	
	iSwapObserver.FileSwapComplete( aError );
	
	// Return KErrNone to avoid active scheduler panic
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CIptvEpgFileSwapper::RunL
// 
// -----------------------------------------------------------------------------
void CIptvEpgFileSwapper::RunL()
	{
	LIVE_TV_TRACE2( _L("CIptvEpgFileSwapper::RunL IN, iStatus: %d"), iStatus.Int() );

	if ( iStatus.Int() == KErrNone )
		{
		// Check if both files are not used anymore
		TBool oldFileOpen = ETrue;
		TBool newFileOpen = ETrue;
		
		// Check if files are open...
		User::LeaveIfError( iFs.IsFileOpen( iOldFileName, oldFileOpen ) );
		User::LeaveIfError( iFs.IsFileOpen( iNewFileName, newFileOpen ) );
		
		if( !oldFileOpen && !newFileOpen ) // Both files are closed -> we can do the swap
			{
			LIVE_TV_TRACE1( _L("both files are closed -> we can start swapping") );

			// Get path to the "official" directory		
			TFileName oldFileNamePath = BaflUtils::DriveAndPathFromFullName( iOldFileName );
			
			// Get path to the temp directory that contains the new stuff
			TFileName newFileNamePath = BaflUtils::DriveAndPathFromFullName( iNewFileName );
			
			TInt error = KErrNone;
			// Get rid of content in "official" directory (meaning the one under \videocenter\ecg\live\<serviceId>
			LIVE_TV_TRACE2( _L("Deleting all files from %S"), &oldFileNamePath );
			error = BaflUtils::DeleteFile( iFs, oldFileNamePath, CFileMan::ERecurse );
			if( error != KErrNone )
				{
				LIVE_TV_TRACE2( _L("BaflUtils::DeleteFile( iFs, oldFileNamePath, ERecurse ); returned %d"), error );
				User::Leave( error );
				}
			
			LIVE_TV_TRACE3( _L("Moving all files from %S to %S"), &newFileNamePath, &oldFileNamePath );
			error = BaflUtils::RenameFile( iFs, newFileNamePath, oldFileNamePath, CFileMan::EOverWrite );
			
			if( error != KErrNone )
				{
				LIVE_TV_TRACE2( _L("BaflUtils::CopyFile( newFileNamePath, oldFileNamePath, EOverWrite ); returned %d"), error );
				User::Leave( error );
				}
			
			// Release file locks for the given files
			ReleaseFileLocks();
			// Tell observer that everything went more than fine
			iSwapObserver.FileSwapComplete( KErrNone );
			}
		else // At least one of the files is open -> waste some more time...
			{
			// We dont have time to wait all day for the file releases.
			// If things didn't happen in 2 seconds we are done here.
			if( iSwapTriesAmount > KMaxSwapTries )
				{
				LIVE_TV_TRACE3( _L("iSwapTriesAmount (%d) > KMaxSwapTries (%d) -> we are aborting the mission here"), iSwapTriesAmount, KMaxSwapTries );
				ReleaseFileLocks();
				iSwapObserver.FileSwapComplete( KErrAccessDenied );
				}
			else // Let's try again
				{
				LIVE_TV_TRACE4( _L("oldFileOpen = %d, newFileOpen = %d -> Wasting more time to make those close. Time already wasted %d times"), oldFileOpen, newFileOpen, iSwapTriesAmount );
				iSwapTriesAmount++;
				if ( !IsActive() )
					{
					iTimeWaster.After( iStatus, KMicrosecondsToWasteAtOnce );
					SetActive();	
					}
				}
			}	
		}
	LIVE_TV_TRACE1( _L("CIptvEpgFileSwapper::RunL OUT") );
	}

// -----------------------------------------------------------------------------
// CIptvEpgFileSwapper::DoCancel
// 
// -----------------------------------------------------------------------------
void CIptvEpgFileSwapper::DoCancel()
	{
	iTimeWaster.Cancel();
	ReleaseFileLocks();
	}
	
// End of file
