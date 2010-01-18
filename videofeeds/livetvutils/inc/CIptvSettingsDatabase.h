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
* Description:    header file for CIptvSettingsDatabase class*
*/




#ifndef __CIPTVSETTINGSDATABASE_H__
#define __CIPTVSETTINGSDATABASE_H__

//  INCLUDES
#include <e32base.h>
#include <d32dbms.h>
#include <f32file.h>

// CONSTANTS
// Settings table
_LIT( KIptvSettingsTable, 					"Settings" );   
_LIT( KIptvSettingsDbKeyCol, 				"Key" );
_LIT( KIptvParentalControlCol,				"ParentalControl" );
_LIT( KIptvRecordingTimeCol,				"RecordingTime" );
_LIT( KIptvReminderTimeCol,					"ReminderTime" );
_LIT( KIptvLastUsedServiceCol,				"LastService" );
_LIT( KIptvLastUsedChannelCol,				"LastChannel" );
_LIT( KIptvIsLastUsedCol,			        "IsLast" );
_LIT( KIptvQuerySettingsSql, "SELECT ParentalControl, RecordingTime, ReminderTime, LastService, LastChannel, IsLast FROM Settings" );

// Default setting values
const TInt KIptvDefaultParentalControl = 0;
const TInt KIptvDefaultRecordingTimeReserve = 0;
const TInt KIptvDefaultReminderTime = 0;
const TUint32 KIptvDefaultLastUsedServiceId = 0;
const TInt64 KIptvDefaultLastUsedChannelId = 0;
const TInt KIptvDefaultIsLastUsed = 0;

// FORWARD DECLARATIONS
// CLASS DECLARATIONS
/**
* Live TV specific settings class.
*/
class TIptvLiveUISettings
	{
	public: 

		TInt    iParentalControl; 			// 0 = Off, or "%N Years"
		TInt    iRecordingTimeReserve;   	// 0 = Off, or "%N min"
		TInt    iReminderTime;				// 0 = Off, or "%N min"
  		TUint32 iLastUsedServiceId;         // 0 = Not used
  		TInt64  iLastUsedChannelId;         // 0 = Not used
  		TInt    iIsLastUsed;                // 0 = No (default), 1 = Yes
	};

/**
 * Class:       CIptvSettingsDatabase
 *
 * Description: An instance of class CIptvSettingsDatabase provides settings
 *              database access: creating & manipulating settings database
 *              files and database entries.
 * 
 */
class CIptvSettingsDatabase : public CBase
    {
    public:  // Constructors and destructor

         /**
         * Two-phased constructor.
         * @param aDbFile Database file name.
         */
        IMPORT_C static CIptvSettingsDatabase* NewL( const TFileName& aDbFile );
    
        /**
         * Destructor.
         */
        IMPORT_C virtual ~CIptvSettingsDatabase();

	    
	public: // Public API for database operations
    
		/**
		* Save settings to the database.
		* @param aSettings Settings object.
		*/
		IMPORT_C void SaveSettingsL( const TIptvLiveUISettings& aSettings );

		/**
		* Read settings from database.
		* @param aSettings On return, contains current settings.
		*/
		IMPORT_C void ReadSettingsL( TIptvLiveUISettings& aSettings );

		/**
		* Save last used service and channel ids to database.
		* @param aSettings Settings object.
		*/
		IMPORT_C void SaveLastUsageL( const TIptvLiveUISettings& aSettings );

		/**
		* Read last used service and channel ids from database.
		* @param aServiceId current service id		
		* @param aSettings On return, contains current settings.
		*/
		IMPORT_C void ReadLastUsageByServiceL( const TUint32 aServiceId, 
		                                TIptvLiveUISettings& aSettings );
		
		/**
		* Reads the actual last watched channel and service from the database.		
		* @param aSettings On return, contains current settings (=last usage)
		*/		                                
        IMPORT_C void ReadLastUsageL( TIptvLiveUISettings& aSettings );		                                

    private: // Constructors
    
        /**
        *	Constructor
        *	@param	aDbFile		Filename of the database to be used
        */
        CIptvSettingsDatabase( const TFileName& aDbFile );

        /**
        *	Symbian second phase constructor. All code that might leave
        *	during construction is placed in here.
        *	@param	None
        *	@return	None
        */
    	void ConstructL();
    	
	private: // Methods
	
        /**
    	* Create table for Settings and last used service and channel.
		* @param aDatabase Database.
    	*/
    	void CreateTableL( RDbNamedDatabase& aDatabase ) const;
    	
        /**
         * 
         * Create new database if DB file is not found. 
         */
        void CreateDbL();

        /**        
         * Creates session to iptv database
         */
        void CreateDbSessionL();
    
		/**
		* Put in default settings.
		* @param aDatabase Database.
		*/
		void SetDefaultValuesL( RDbNamedDatabase& aDatabase ) const;

        /**               
         * Initialise database   
         */     
        void InitDbL();
        
        
    private: // Data

		/** File server session is connected in construction and closed in
			destruction */
    	RFs              iFsSession;
    	
    	/** File name of the database which to handle */
		TFileName        iDbFile;

        /** DB session */
        RDbs iDbSession;
        
        /** database */
        RDbNamedDatabase iDb;
        
    };

#endif // __CIPTVSETTINGSDATABASE_H__
            
// End of File
