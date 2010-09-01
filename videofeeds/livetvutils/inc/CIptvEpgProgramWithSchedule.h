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
* Description:    Class to provide program data to*
*/




#ifndef __CIPTVEPGPROGRAMWITHSCHEDULE_H__
#define __CIPTVEPGPROGRAMWITHSCHEDULE_H__

#include "CIptvEpgProgram.h"

// FORWARD DECLARATIONS
class CIptvEpgSchedule;

/**
*	"Carrier" class to contain data for single program and it's start and end
*	time occurence in EPG xml file. This class is used to communicate data
*	from xml content handler to EPG manager to be stored to the database.
*/
class CIptvEpgProgramWithSchedule : public CIptvEpgProgram
	{
	public:
	/**
	*	Symbian two phase constructor
	*	@param	None
	*	@return	Created instance of CLiveTvEpgProgramWithSchedule
	*/
	IMPORT_C static CIptvEpgProgramWithSchedule* NewL();
	
	/**
	*	Destructor
	*/
	virtual ~CIptvEpgProgramWithSchedule();

	private: // Constructors
	/**
	*	Default constructor
	*/
	CIptvEpgProgramWithSchedule();
	
	/**
	*	Symbian second phase constructor which might contain leaving code
	*/	
	void ConstructL();
	
	public: // Functions

	IMPORT_C void SetStartTime( const TTime& aStartTime );
	
	IMPORT_C TTime StartTime() const;
	
	IMPORT_C void SetEndTime( const TTime& aEndTime );
	
	IMPORT_C TTime EndTime() const;
	
	IMPORT_C CIptvEpgSchedule& Schedule();
	
	// Instance variables
	private:

	TTime iStartTime;
	TTime iEndTime;
	CIptvEpgSchedule* iSchedule;
	};
	
#endif //__CIPTVEPGPROGRAM_H__

// End of file
