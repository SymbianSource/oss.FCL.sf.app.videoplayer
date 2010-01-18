/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CIPTVEPGFILESWAPPER_H
#define __CIPTVEPGFILESWAPPER_H

// System includes
#include <e32base.h>		// For CActive
#include <f32file.h>		// For RFs
//#include <babackup.h>		// F

// Forward declarations
class CBaBackupSessionWrapper;
class MIptvEpgFileSwapObserver;

/**
*	Class to replace given old file with given new file.
*	This class takes care of requesting to close affected files by
*	using CBaBackupSessionWrapper and monitors the state of those two files.
*	When both files are not open by any other instance anymore this class
*	performs the file switching in it's RunL method and after that signalizes
*	the observers via CBaBackupSessionWrapper that they are allowed to re-open
*	the files for reading and writing.
*/
class CIptvEpgFileSwapper : public CActive
	{
	public:
		/**
		*	Symbian two phace constructor.
		*	@param	aOldFileName	Name of the file to be replaced
		*	@param	aNewFileName	Name of the file to be used to replace aOldFileName
		*	@return	Pointer to newly created instance of CIptvEpgFileSwapper object
		*/
		IMPORT_C static CIptvEpgFileSwapper* NewL( const TFileName& aOldFileName,
												   const TFileName& aNewFileName,
											       MIptvEpgFileSwapObserver& aSwapObserver );
		
		/**
		*	Destructor
		*/
		virtual ~CIptvEpgFileSwapper();
		
	public:
		/**
		*	Method to initiate file swapping process. This method signalizes the
		*	backupwrapper to ask all the backup observers for aOldFileName and aNewFilename
		*	to release their handles to the files and sets this object to active state.
		*	@param	None
		*	@return	None
		*/
		IMPORT_C void SwapFilesL();
	
	private:
		/**
		*	Default constructor
		*	@param	aOldFileName	Name of the file to be replaced
		*	@param	aNewFileName	Name of the file to be used to replace aOldFileName
		*	@param	aSwapObserver	Reference to a class that observes the file swapping
		*	@return	None
		*/
		CIptvEpgFileSwapper( const TFileName& aOldFileName,
							 const TFileName& aNewFileName,
							 MIptvEpgFileSwapObserver& aSwapObserver );
		
		/**
		*	Symbian second phase constructor containing the code that might leave
		*	during instantiation of this class.
		*	@param	None
		*	@return	None
		*/
		void ConstructL();
		
		/**
		*	Method that calls RestartFile method of CBaBackupSessionWrapper to allow
		*	observers to re-establish the connections to the files.
		*	@param	None
		*	@return	None
		*/
		void ReleaseFileLocks();
		
	private: // From CActive
		/**
		*	Overwritten error handling method to get called when active scheduler catches
		*	leave in this active object.
		*	@param	aError	Symbian error code for the occured error
		*	@return	KErrNone if error was handled by this method. If return value is
		*					something else but KErrNone, active scheduler will panic.
		*/
		TInt RunError( TInt aError );
		
		/**
		*	Method to get called when active object request has been completed.
		*	@param	None
		*	@return	None
		*/
		void RunL();
		
		/**
		*	Method to handle actual actions required after Cancel-method has been called.
		*/
		void DoCancel();
			
	private: // instance variables
		/**
		*	Handle to the file server session
		*/
		RFs iFs;
		/**
		*	Timer to use when monitoring open files
		*/
		RTimer iTimeWaster;
		
		/**
		*	Name of the old file that is to be replaced
		*/
		TFileName iOldFileName;
		
		/**
		*	Name of the file to be used to replace iOldFileName
		*/
		TFileName iNewFileName;
		
		/**
		*	Backup session to signalize all observers to release handle to
		*	iOldFileName and iNewFileName
		*/
		CBaBackupSessionWrapper* iBackupWrapper;
		
		/**
		*	Reference to the class that observes the file swapping
		*/
		MIptvEpgFileSwapObserver& iSwapObserver;
		
		TInt iSwapTriesAmount;
	};
	
#endif // __CIPTVEPGFILESWAPPER_H
