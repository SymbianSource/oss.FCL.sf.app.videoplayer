/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    header file for CIptvEpgDatabase class*
*/




#ifndef __CIPTVEPGDATABASE_H__
#define __CIPTVEPGDATABASE_H__

//  INCLUDES
#include <e32std.h>
#include <badesca.h>    // CDesCArrayFlat (cannot be forward declarated)
#include <d32dbms.h>    // RDbStoreDatabase
#include <f32file.h>    // RFs
#include <babackup.h>	// For MBackupObserver && CBaBackupSessionWrapper

// CONSTANTS
const TInt KCustomSqlLength = 256;
const TInt KIptvEpgProgramMaxLength = 255;
const TInt KIptvEpgChannelNameMaxLength = 255;
const TInt KIptvEpgLogoPathMaxLength = 255;
const TInt KIptvEpgUriMaxLength = 255;
const TInt KIptvEpgDescrMaxLength = 255;
// Channel table
_LIT( KIptvEpgChannelTable, 				"Channel" );   
_LIT( KIptvEpgChannelDbKeyCol, 				"Key" );

_LIT( KIptvEpgChannelIdCol,					"ChannelId" );
_LIT( KIptvEpgChannelServProviderIdCol,		"ServiceProviderId" );
_LIT( KIptvEpgChannelNameCol,				"ChannelName" );
_LIT( KIptvEpgChannelLogoPathCol,			"ChannelLogoPath" );
_LIT( KIptvEpgChannelDescriptionCol,		"ChannelDescription" );
_LIT( KIptvEpgChannelURICol,				"ChannelURI" );
_LIT( KIptvEpgChannelSDPCol,				"ChannelSDP" );
_LIT( KIptvEpgChannelOrderCol,				"ChannelOrder" );

// Program table
_LIT( KIptvEpgProgramTable, 				"ProgramEvent" );
_LIT( KIptvEpgProgramDbKeyCol, 				"Key" );
_LIT( KIptvEpgProgramIdCol,					"ProgramId" );
_LIT( KIptvEpgProgramChannelId,				"ProgramChannelId" );
_LIT( KIptvEpgProgramServProviderIdCol, 	"ServiceProviderId" );
_LIT( KIptvEpgProgramStartTimeCol,			"ProgramStartTime" );
_LIT( KIptvEpgProgramEndTimeCol,			"ProgramEndTime" );
_LIT( KIptvEpgProgramURICol,				"ProgramURI" );
_LIT( KIptvEpgProgramGenreCol,				"ProgramGenre" );
_LIT( KIptvEpgProgramNameCol,				"ProgramName" );
_LIT( KIptvEpgProgramDescriptionCol,		"ProgramDescription" );
_LIT( KIptvEpgProgramLanguageCol,			"ProgramLanguage" );
_LIT( KIptvEpgProgramParentalRatingCol,		"ProgramParentalRating" );

// Schedule table
_LIT( KIptvEpgScheduleTable,				"Schedule" );
_LIT( KIptvEpgScheduleTableKeyCol,			"ScheduleKey" );
_LIT( KIptvEpgScheduleServiceProviderIdCol,	"ScheduleServiceProviderId" );
_LIT( KIptvEpgScheduleChannelIdCol,			"ScheduleChannelId" );
_LIT( KIptvEpgScheduleProgramIdCol,			"ScheduleProgramId" );
_LIT( KIptvEpgScheduleStartTimeCol,			"ScheduleStartTime" );
_LIT( KIptvEpgScheduleEndTimeCol,			"ScheduleEndTime" );

// LatestEpgAvailable table
_LIT( KIptvEpgLatestEpgAvailableTable,					"LatestEpgAvailable" );
_LIT( KIptvEpgLatestEpgAvailableTableKeyCol,			"LatestEpgAvailableKey" );
_LIT( KIptvEpgLatestEpgAvailableServiceProviderIdCol,	"LatestEpgAvailableServiceProviderId" );
_LIT( KIptvEpgLatestEpgAvailableStartTimeCol,			"LatestEpgAvailableStartTime" );

// Last modified table
_LIT( KIptvEpgLastModifiedTable,			"LastModifiedMetadata" );
_LIT( KIptvEpgLastModifiedTableKeyCol,		"LastModifiedMetadataKey" );
_LIT( KIptvEpgLastModifiedTableServiceIdCol,"LastModifiedMetadataServiceId" );
_LIT( KIptvEpgLastModifiedTableETagCol,		"LastModifiedMetadataETag" );
_LIT( KIptvEpgLastModifiedTableTimeCol,		"LastModifiedMetadataTime" );

// FORWARD DECLARATIONS
// CLASS DECLARATION

class CIptvEpgChannel;
class CIptvEpgProgram;
class CIptvEpgSchedule;
class CIptvEpgScheduleSearch;
class CIptvEpgLatestEpgAvailable;
class CIptvEpgProgramWithSchedule;

/**
 * Class:       CIptvEpgDatabase
 *
 * Description: An instance of class CIptvEpgDatabase provides Epg
 *              database access: creating & manipulating Epg database
 *              files and database entries (channels, and programs).
 *
 * Database definition:     
 *
 *              The Epg database contains one table as follows:
 *
 *              Table name: Channel
 *              	Column:				Type:			Length (bytes):   Desc:
 *                	------				-----          	-----------       -----
 *                 	Key                	EDbColUint32
 *					ChannelId			EDbColInt64
 *					ServiceProviderId	EDbColUint32
 *					ChannelName			EDbColText
 *					ChannelLogoPath		EDbColText
 *					ChannelDescription	EDbColText
 *					ChannelURI			EDbColText
 *					ChannelSDP			EDbColLongText
 *					ChannelOrder		EDbColUint32
 *
 *
 *              There is also index for Channel table with name "ChannelIndex".
 *				It consists of Key column.
 *
 *              Index provides quick find.
 *
 *
 * Database definition:
 *
 *              The Epg database contains one table as follows:
 *
 *              Table name: ProgramEvent
 *                 	Column:         		Type:         Length (bytes): Desc:
 *                 	------              	-----         -----------     -----
 *                 	Key                 	EDbColUint32
 *					ProgramId				EDbColInt64
 * 					ProgramChannelId		EDbColInt64
 *					ServiceProviderId		EDbColUint32
 *					ProgramStartTime		EDbColDateTime
 *					ProgramEndTime			EDbColDateTime
 *					ProgramURI				EDbColText
 *					ProgramSDP				EDbColLongText
 *					ProgramGenre			EDbColText
 *					ProgramName				EDbColText
 *					ProgramDescription		EDbColText
 *					ProgramLanguage			EDbColText
 *					ProgramParentalRating	EDbColText
 *
 *
 *              There is also index for Program table with name "ProgramIndex".
 *				It consists of Key column.
 *
 *              Index provides quick find.
 *
 * Database definition:
 *				The epg database contains one table as follows:
 *				Table name: Last modified
 *              Table name: ChannelProgram
 *                 	Column:             Type:          	Length (bytes): Desc:
 *                 	------              -----          	-----------     -----
 *                 	ServiceId           EDbColUint32   	
 *                 	LastModifedDateTime EDbColText
 *                 	ETag			 	EDbColText
 *
 * Database definition:
 *
 *              The Epg database contains one table as follows:
 *
 *              Table name: ChannelProgram
 *                 	Column:             Type:          	Length (bytes): Desc:
 *                 	------              -----          	-----------     -----
 *                 	ChannelKey          EDbColUint32   	4
 *                 	ProgramKey          EDbColUint32   	4
 *
 * 
 */
class CIptvEpgDatabase : public CBase, public MBackupObserver
    {
    public:
    /**
    *	Enumeration for the database class internal state.
    */
    enum TIptvEpgDatabaseInternalState
    	{
    	EReady = 0, 	// This class is ready to be used
    	EBackup			// Database backup is ongoing -> no interaction allowed
    	};
    	
    public:  // Constructors and destructor

         /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvEpgDatabase* NewL( const TFileName& aDbFile );
    
        /**
         * Destructor.
         */
        virtual ~CIptvEpgDatabase();
        
    public: // From MBackupObserver
    	/**
    	*	Method to get called when file that this class has registered
    	*	to observe for backup is to be affected with backup operation.
    	*	This method is also called when it's allowed to re-open the
    	*	file for read/write.
    	*	@param	aFileName	Name of the file that needs to be closed
    	*						or can be re-opened
    	*	@param	aFlags		Flags identifying the operation to be happen
    	*	@return	None
    	*/
    	void ChangeFileLockL( const TDesC& aFileName, TFileLockFlags aFlags );    
    
	public: // Public API for database operations
    
        /**
         *	Get channels.
         *
		 *  @param  aServiceProviderId 	Service provider id.
         *	@param	aResultArray		Array of channels.         
         */
        IMPORT_C void GetChannelsL( const TUint32 aServiceProviderId, 
									RPointerArray<CIptvEpgChannel>* 
									aResultArray );

        /**
         *	Get programs.
         *
		 *  @param aServiceProviderId Service provider id.
		 *  @param aChannelKey      Channel id.
         *							aResultArray.
         *	@param 	aResultArray    Array where found results are put.
         */
        IMPORT_C void GetProgramsByChannelIdL( 
											const TUint32 aServiceProviderId, 
											const TInt64 aChannelKey,
        				   					RPointerArray<CIptvEpgProgram>* 
											aResultArray );
		
        /**
         *	Get next program by channel.
         *
		 *  @param aServiceProviderId Service provider id.
		 *  @param aChannelKey Channel id.
         *	@param aRefTime reference time 
         *	@return CIptvEpgSchedule* the found schedule
         */									
		IMPORT_C CIptvEpgSchedule* GetNextProgramL( 
								const TUint32 aServiceProviderId,
								const TInt64 aChannelKey,
								const TTime& aRefTime );

		/**
         *	Get schedules (for search) by service id. 
         *
		 *  @param aServiceProviderId Service provider id.
		 *							aResultArray.
         *	@param 	aResultArray    Array where found results are put.
         *	@return                 None
         */
        IMPORT_C void GetSchedulesByServiceIdL( 
					const TUint32 aServiceProviderId, 
					RPointerArray<CIptvEpgScheduleSearch>* aResultArray );

        /**
         *	Get programs.
         *
		 *  @param aServiceProviderId Service provider id.
		 *  @param aChannelKey      Channel id.
         *							aResultArray.
         *	@param 	aResultArray    Array where found results are put.
         *	@return                 None
         */
        IMPORT_C void GetSchedulesByChannelIdL( 
											const TUint32 aServiceProviderId, 
											const TInt64 aChannelKey,
        				   					RPointerArray<CIptvEpgSchedule>* aResultArray );
        /**
         *	Get the programs for that channel and day
         *
		 *  @param aServiceProviderId Service provider id.
		 *  @param aChannelKey        Channel id.							
         *	@param aStartTime         start of the day
         *	@param 	aResultArray      Array where found results are put.
         *	@return 				  None 
         *	                  
         */					
		IMPORT_C void GetSchedulesByChannelAndDayL( 
								const TUint32 aServiceProviderId,
								const TInt64 aChannelKey,
								const TTime& aStartTime,
								RPointerArray<CIptvEpgSchedule>* aResultArray );
		
		
        /**
         *	Get the program that covers the given start time
         *  in its time duration
         *
		 *  @param aServiceProviderId Service provider id.
		 *  @param aChannelKey        Channel id.							
         *	@param aStartTime 
         *	@return CIptvEpgSchedule*		  the found schedule  
         *	                  
         */	
         
         												
		IMPORT_C CIptvEpgSchedule* GetProgramByTimeL(
												  const TUint32 aServiceProviderId, 
												  const TInt64 aChannelKey,
												  const TTime& aStartTime
												   );										
		/**
		*	Get channel specified by id
		*	@param	aService		Service provider id
		*	@param	aChannel	    Channel id
		*	@return	CIptvEpgChannel or NULL if not found
		*/
        IMPORT_C CIptvEpgChannel* GetChannelL( TInt aService,
        								       TInt64 aChannel );	
		/**
		*	Get latest schedule time in db
		*	@param	aService		Service provider id
		*   @param  aLatestScheduleTime, returns this
		*	@return	Latest schedule time in database.
		*/        								       
		IMPORT_C TTime GetLatestScheduleTimeL( const TUint32 aServiceId );        								       									  

		/**
		*	Inserts or updates given channel to the database.
		*	Each channel must have unique id inside one serivice providers
		*	list of channels. Therefore is aChannel.iChannelId is found from
		*	database and that database entry has aChannel.iServiceProviderId
		*	existing data is update. Otherwise aChannel is added to the
		*	database.
		*	@param	aChannel	Object containing data to be inserted or
		*						updated to the database
		*	@param	aChannelKey	On return contains unique database id of
		*						inserted or updated database entry
		*/        
		IMPORT_C void InsertOrUpdateChannelL( const CIptvEpgChannel& aChannel,
											  TUint32& aChannelKey );
		
        /**
        *	Delete channel from the database. This causes all programs which 
		*	has	aChannelId in the program table to be deleted as well.
		*   @param  aServiceProviderId Service provider id.
        *	@param	aChannelId	Channel id of the channel to be deleted
        *	@return	None
        */
        IMPORT_C void DeleteChannelL( const TUint32 aServiceProviderId,
									  const TInt64 aChannelId );
        
        /**
        *	Inserts or updates given program to the database.
        *	Each program must have unique program id inside a channel. This
        *	function tries to find an entry from database which has
        *	aProgram.iProgramId and aProgram.iChannelId. If one is found from
        *	the database, then database is updated, otherwise new entry is
        *	made.
        *	@param	aProgram	Class containing the program data to be
        *						added or updated
		*	@param	aProgramKey	On return contains unique database id of
		*						inserted or updated database entry
        */
        IMPORT_C void InsertOrUpdateProgramL( const CIptvEpgProgram& aProgram,
        									  TUint32& aProgramKey );

		IMPORT_C void InsertOrUpdateScheduleL( 
								const CIptvEpgSchedule& aSchedule );
        /**
        *	Inserts or updates given latestEpgAvailable info to the database.
        *	@param	aLatestEpgAvailable	
		*	@param	aInsert	boolean to decide insert or update
        */		
		IMPORT_C void InsertOrUpdateLatestEpgAvailableL
									( CIptvEpgLatestEpgAvailable& aLatestEpgAvailable );

        /**
        *	Delete program from the database.
		*   @param  aServiceProviderId Service provider id.
        *	@param	aProgramId	Id of the program to be deleted from 
		*			program table
        *	@return	None
        */
        IMPORT_C void DeleteProgramL( const TUint32 aServiceProviderId,
									  const TInt64 aProgramId );
        
		/**
         * Update channel order.
		 *
         * @param aServiceId		 	Service provider id.
         * @param aChannelId           	Channel id.
		 * @param aOrder				New order number.
		 */
        IMPORT_C void UpdateChannelOrderL( const TUint32 aServiceId,
										   const TInt64  aChannelId,
										   const TUint32 aOrder );
        
		/**
         * Update channel icon path.
		 *
         * @param aServiceId		 	Service provider id.
         * @param aChannelId           	Channel id.
		 * @param aIconPath				New icon path
		 */
        IMPORT_C void UpdateChannelIconPathL( const TUint32 aServiceId,
										   	  const TInt64  aChannelId,
										   	  const TDesC&  aIconPath );
		/**
         * Gets the schedules by program id
		 *
         * @param aServiceId		 	Service provider id.
         * @param aChannelKey           Channel id.
		 * @param aProgId				program id
		 * @param aResultArray
		 */										   	  
		IMPORT_C void GetSchedulesByProgIdL( 
								const TUint32 aServiceProviderId,
								const TInt64 aChannelKey,
								const TInt64 aProgId,
								RPointerArray<CIptvEpgSchedule>* aResultArray );

		/**
		* Clear service specific EPG data from database.
		* @param aServiceId Service id.
		*/
		IMPORT_C void ClearServiceEPGDataL( const TUint32 aServiceId );


        /**
         * Add new channel to database.
         *
         * @param aChannel             Reference to CIptvMulticastChannel 
		 *							   object. 
         * @param aChannelKey          New channel key value is inserted to 
		 *							   aChannelKey.
         */
        IMPORT_C void AddChannelL( const CIptvEpgChannel& aChannel, 
								   TUint32& aChannelKey );
								   
		IMPORT_C void AddChannelsL( const RPointerArray<CIptvEpgChannel>& aChannels,
									TUint32& aChannelKey );

		/**
         * Add new program to database.
         *
         * @param aProgram             Reference to CIptvMulticastProgram 
		 *							   object.
         * @param aProgramKey          New program key value
         */
        IMPORT_C void AddProgramL( const CIptvEpgProgram& aProgram, 
						  TUint32& aProgramKey );

		IMPORT_C void AddProgramsL( const RPointerArray<CIptvEpgProgram>& aPrograms );
		

		IMPORT_C void AddProgramWithScheduleL( CIptvEpgProgramWithSchedule& aProgramWithSchedule,
											   TUint32& aProgramKey );
		
		IMPORT_C void AddProgramsWithSchedulesL(
				const RPointerArray<CIptvEpgProgramWithSchedule>& aProgramsWithSchedules );

		/**
		*	Add schedule
		*	@param	aSchedule	Schedule information to be added to the database
		*	@return	None
		*/
		IMPORT_C void AddScheduleL( const CIptvEpgSchedule& aSchedule );
		
		IMPORT_C void AddSchedulesL( const RPointerArray<CIptvEpgSchedule>& aSchedules );

		/**
		* Get single program entry.
		* @param aServiceProviderId Service id.
		* @param aChannelId Channel id.
		* @param aProgramId Program id.
		* @return Found program entry or NULL if not found. NULL also returned
		* if iLocalState == EBackup. Ownership is
		* transferred to the caller.
		*/
		IMPORT_C CIptvEpgSchedule* GetProgramL( const TUint32 aServiceProviderId,
												const TInt64 aChannelId,
												const TInt64 aProgramId );
		/*
		*	Getter method for ETag and last modified date and time for the given service
		*	@param	aServiceId				Service that we are looking for
		*									last modified data
		*	@param	aETag					On return contains the retrieved
		*									etag value from the db
		*	@param	aLastModifiedDateTime	On return contains the retrieved
		*									last modified value from db
		*	@return	None
		*/
		IMPORT_C void GetLastModifiedDataL( const TUint32 aServiceId,
											TDes& aETag,
											TDes& aLastModifiedDateTime );

		/**
		*	Setter method for last modified data to the database
		*	@param	aServiceId				Id of the service we are assigning the
		*									last modified data values
		*	@param	aETag					ETag value to be stored to the db
		*	@param	aLastModifiedDateTime	Last modified data to be stored to the db.
		*									This value can be something like:
		*									"Sun, 23 Jan 2007 08:20:37 GMT"
		*	@return	None
		*/
		IMPORT_C void SetLastModifiedDataL( const TUint32 aServiceId,
											const TDesC& aETag,
											const TDesC& aLastModifiedDateTime );

    private:
        /**
        *	Constructor
        *	@param	aDbFile		Filename of the database to be used
        */
        CIptvEpgDatabase( const TFileName& aDbFile );

        /**
        *	Symbian second phase constructor. All code that might leave
        *	during construction is placed in here.
        *	@param	None
        *	@return	None
        */
    	void ConstructL();

        /**
        *	Removes all rows from channel table  in the epg database
		*   using given service id.
        *	@param	aServiceId Service id.
        *	@return	None
        */
        void ClearChannelTableL( const TUint32 aServiceId );


		/**
        *	Removes all service specific rows from program table in 
		*   the epg database.
        *	@param	aServiceId Service id.
        *	@return	None
        */
        void ClearProgramTableL( const TUint32 aServiceId );

		/**
        *	Removes all service specific rows from schedule table in 
		*   the epg database.
        *	@param	aServiceId Service id.
        *	@return	None
        */
        void ClearScheduleTableL( const TUint32 aServiceId );


		/**
        *	Removes all service specific rows from latest epg table in 
		*   the epg database.
        *	@param	aServiceId Service id.
        *	@return	None
        */
        void ClearLatestEpgTableL( const TUint32 aServiceId );

		/**
		*	Removes all service specific rows from last modified table.
		*	@param	aServiceId	Service id which rows are to be removed
		*	@return	None
		*/
		void ClearLastModifiedTableL( const TUint32 aServiceId );

		/**
		*	Add LatestEpgAvailable
		*	@param	aLatestEpgAvailable	LatestEpgAvailable for a service information 
		*   to be added to the database
		*	@return	None
		*/
		void AddLatestEpgAvailableL( CIptvEpgLatestEpgAvailable& aLatestEpgAvailable );
	
    	/**
    	* Create channel table
    	*/
    	void CreateChannelTableL( RDbNamedDatabase& aDatabase ) const;
    	
    	/**
    	* Create program table
    	*/
    	void CreateProgramTableL( RDbNamedDatabase& aDatabase) const;
    	    
    	/**
    	*	Create last modified table. This table consist of service id, ETag value
    	*	and last modified date (ETag and last modified are optional values)
    	*	@param	aDatabase	Reference to the database where the table is
    	*						created.
    	*	@return	None
    	*/
    	void CreateLastModifiedTableL( RDbNamedDatabase& aDatabase ) const;
    	    	
    	/**
    	* Create ChannelProgram table
    	*/
    	void CreateChannelProgramTableL( RDbNamedDatabase& aDatabase ) const;
    	
    	/**
    	* Create Schedule table
    	*/
    	void CreateScheduleTableL( RDbNamedDatabase& aDatabase ) const;
    	
    	/**
    	* Create LatestEpgAvailable table
    	*/    	
    	void CreateLatestEpgAvailableTableL( RDbNamedDatabase& aDatabase ) const;
    	
    	/**
    	*	Gets each and every row from view and parses the data to result 
		*	array. If aResultArray is NULL, this function leaves with 
		*	KErrArgument.
    	*	@param	aSqlStatement	SQL statement to be used for database.
    	*	@param	aResultArray	Array where parsed database stuff is 
		*   put into.
    	*	@return	None
    	*/
    	void FetchProgramsFromTableL( const TDesC& aSqlStatement,
    						RPointerArray<CIptvEpgProgram>* aResultArray );
    						
    	
		/**
    	*	Gets program names and program ids from program table.
		*	If aResultArray is NULL, this function leaves with 
		*	KErrArgument.
    	*	@param	aSqlStatement	SQL statement to be used for database.
    	*	@param	aResultArray	Array where parsed database stuff is 
		*   put into.
    	*	@return	None
    	*/
		void FetchProgramNamesFromTableL( const TDesC& aSqlStatement,
							RPointerArray<CIptvEpgProgram>* aResultArray );				
    	/**
    	*	Gets each and every row from view and parses the data to result 
		*	array. If aResultArray is NULL, this function leaves with 
		*	KErrArgument.
    	*	@param	aSqlStatement	SQL statement to be used for database.
    	*	@param	aResultArray	Array where parsed database stuff is 
		*   put into.
    	*	@return	None
    	*/
    	void FetchSchedulesFromTableL( const TDesC& aSqlStatement,
    						RPointerArray<CIptvEpgSchedule>* aResultArray );    	


		/**
    	*	Gets rows from schedule table using given sql statement and puts
		*   results to aResultArray. If aResultArray is NULL, this function 
		*   leaves with KErrArgument.
    	*	@param	aSqlStatement	SQL statement to be used for database.
    	*	@param	aResultArray	Array where parsed database stuff is 
		*   put into.
    	*	@return	None
    	*/	
		void FetchSearchSchedulesFromTableL( 
						const TDesC& aSqlStatement,
						RPointerArray<CIptvEpgScheduleSearch>* aResultArray );

        /**
         *	Get channels. The actual workhorse. Called from public
         *  channel-getting methods. 
         *
		 *  @param  aQuery              Contains SQL that is supposed to return the channels
         *	@param	aResultArray		Array of channels.         
         */
        void DoGetChannelsL( const TDesC& aQuery, 
						     RPointerArray<CIptvEpgChannel>* 
							 aResultArray );

        /**
         * 
         * Create new database if DB file is not found. 
         */
        void CreateDbL();

        /**        
         * Creates session to iptv database
         */
        void CreateMulticastDbSessionL();
    
        /**        
         * Closes the session to iptv database
         */
        void CloseMulticastDbSession();

		/**
		 * Update channel order numbers so that they are consistent.
		 * @param aServiceProviderId Service id.
		 */
		void UpdateChannelOrdersL( const TUint32 aServiceProviderId );

    	/**
		*   Method for deleting all schedule data for given channel. 
    	*	@param	aChannel	Specifies the channel whose schedules will be
    	*                       thrown away. 
    	*	@return	None
    	*/
    	void RemoveScheduledDataFromChannelL( const CIptvEpgChannel& aChannel );

		/**
		* Get program names and program ids using service id.		
		* @param aServiceProviderId Service ID.
		* @param aResultArray Array where results are put.
		*/
		void GetProgramNamesByServiceIdL( const TUint32 aServiceProviderId, 
									  RPointerArray<CIptvEpgProgram>* 
									    aResultArray );
        
	private: 

        /**               
         * Initialise database   
         */     
        void InitDbL();
        
        /**
        * Construct SQL-query strings
        */
        void MakeSqlStrings();
        
        /**
        *	Method to check iStatus state to see if we are having backup
        *	in progress. If that's the case then we'll put CActiveSchedulerWait
        *	on and wait until backup has been finished. Otherwise this method
        *	does nothing.
        *	@param	None
        *	@return	None
        */
        void CheckBackupStatus();
        
    private:

		/** File server session is connected in construction and closed in
			destruction */
    	RFs              iFsSession;
    	
    	/** File name of the database which to handle */
		TFileName        iDbFile;

        /** Multicast DB session */
        RDbs iMulticastDbSession;
        
        /** Multicast database */
        RDbNamedDatabase iMulticastDb;

        /** Channel SQL-query string */
        TBuf<KCustomSqlLength> iSqlChannel;
        
        /* Program SQL-query string */
        TBuf<KCustomSqlLength> iSqlProgram;

		/* Backup session to observer starting/ending backup operation */
        CBaBackupSessionWrapper* iBackupWrapper;
        
        /* Current state of this object */
        TIptvEpgDatabaseInternalState iLocalState;
        
        CActiveSchedulerWait* iActiveWait;
    };

#endif // __CIPTVEPGDATABASE_H__
            
// End of File
