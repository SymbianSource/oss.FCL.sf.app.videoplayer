/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Classes to provide carriers of schedule data to*
*/




#ifndef __CIPTVEPGSCHEDULE_H__
#define __CIPTVEPGSCHEDULE_H__

#include <e32std.h>
#include <e32base.h>

#include "IptvLiveEpgConstants.h"

class CIptvEpgProgram;

/**
*	"Carrier" class to contain data for single program occurence in EPG xml
*	file. This class is used to communicate data from xml content handler
*	to EPG manager to be stored to the database.
*/
class CIptvEpgSchedule : public CBase
	{
	public:
	/**
	*	Default constructor
	*/
	IMPORT_C static CIptvEpgSchedule* NewL();
	
	virtual ~CIptvEpgSchedule();
	
	static TInt LinearOrderOfSchedulesByTime(const CIptvEpgSchedule &s1, const CIptvEpgSchedule &s2);
	
	public: // Getter methods
	/**
	* @return program id.
	*/
	IMPORT_C TInt64 GetProgramId();
	
	/**
	* @return program's channel id.
	*/
	IMPORT_C TInt64 GetChannelId();

	/**
	* @return program's service provider id.
	*/
	IMPORT_C TUint32 GetServiceId();
	
	/**
	* @return program start time.
	*/
	IMPORT_C TTime GetStartTime();
	
	/**
	* @return program end time.
	*/
	IMPORT_C TTime GetEndTime();

	/**
	* @return program URI address.
	*/
	IMPORT_C TPtrC GetURI();

	/**
	* @return program SDP data.
	*/
	IMPORT_C TPtrC GetSDP();

	/**
	* @return program genre.
	*/
	IMPORT_C TPtrC GetGenre();

	/**
	* @return program name.
	*/
	IMPORT_C TPtrC GetName();

	/**
	* @return program description.
	*/
	IMPORT_C TPtrC GetDescription();

	/**
	* @return program language.
	*/
	IMPORT_C TPtrC GetLanguage();

	/**
	* @return program parental rating.
	*/
	IMPORT_C TPtrC GetParentalRating();

	
	public: // Setter methods

	/**
	* @param aProgramId Program id.
	*/
	IMPORT_C void SetProgramId( const TInt64& aProgramId );

	/**
	* @param aChannelId Program's channel id.
	*/
	IMPORT_C void SetChannelId( const TInt64& aChannelId );
	
	/**
	* @param aServiceId Program's service provider id.
	*/
	IMPORT_C void SetServiceId( const TUint32& aServiceId );

	/**
	* @param aStartTime Program start time.
	*/
	IMPORT_C void SetStartTime( const TTime& aStartTime );

	/**
	* @param aEndTime Program end time.
	*/
	IMPORT_C void SetEndTime( const TTime& aEndTime );

	/**
	* @param aURI Program URI.
	*/
	IMPORT_C void SetURIL( const TDesC& aURI );

	/**
	* @param aSDP Program SDP.
	*/
	IMPORT_C void SetSDPL( const TDesC& aSDP );

	/**
	* @param aGenre Program genre.
	*/
	IMPORT_C void SetGenreL( const TDesC& aGenre );

	/**
	* @param aName Program name.
	*/
	IMPORT_C void SetNameL( const TDesC& aName );

	/**
	* @param aDescription Program description.
	*/
	IMPORT_C void SetDescriptionL( const TDesC& aDescription );

	/**
	* @param aLanguage Program language.
	*/
	IMPORT_C void SetLanguageL( const TDesC& aLanguage );

	/**
	* @param aParentalRating Program parental rating.
	*/
	IMPORT_C void SetParentalRatingL( const TDesC& aParentalRating );
	
	private:
	CIptvEpgSchedule();
	
	void ConstructL();
	
	public: // Public instance variables
	
	TUint32 iServiceId;
	
	TInt64 iChannelId;
	
	TInt64 iProgramId;
	
	TTime iStartTime;
	
	TTime iEndTime;
	
	CIptvEpgProgram* iProgram;
	
	};
	
#endif //__CIPTVEPGSCHEDULE_H__

// End of file
