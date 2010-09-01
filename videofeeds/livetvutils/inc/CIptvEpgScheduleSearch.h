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
* Description:    Classes to provide carriers of search schedule data to UI*
*/




#ifndef __CIPTVEPGSCHEDULESEARCH_H__
#define __CIPTVEPGSCHEDULESEARCH_H__

// INCLUDES
#include <e32base.h>

/**
*	"Carrier" class to contain data for schedule data for search view.
*/
class CIptvEpgScheduleSearch : public CBase
	{
	public: // Constructors
	/**
	*	Default constructor
	*/
	IMPORT_C static CIptvEpgScheduleSearch* NewL();
	
	/**
	* Destructor.
	*/
	virtual ~CIptvEpgScheduleSearch();
	
	public: // Methods
	/**
	* Sort schedules by start time.
	*/
	static TInt LinearOrderOfSchedulesByTime( const CIptvEpgScheduleSearch &s1, 
									   const CIptvEpgScheduleSearch &s2 );
	/**
	* Sort schedules by program name.
	*/
	IMPORT_C static TInt LinearOrderOfSchedulesByName( const CIptvEpgScheduleSearch &s1, 
									   const CIptvEpgScheduleSearch &s2 );

	private: // Constructors

	/**
	* C++ constructor.
	*/
	CIptvEpgScheduleSearch();

	/**
	* Symbian 2nd phase constructor.
	*/	
	void ConstructL();
	
	public: // Public instance variables
	
	TInt64 iChannelId;
	
	TInt64 iProgramId;
	
	TTime iStartTime;

	TTime iEndTime;
	
	HBufC* iProgramName;
	
	};
	
#endif //__CIPTVEPGSCHEDULESEARCH_H__

// End of File
