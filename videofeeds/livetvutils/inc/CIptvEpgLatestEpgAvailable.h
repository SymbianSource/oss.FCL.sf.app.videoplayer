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
* Description:    Classes to provide carriers of LatestEpgAvailable data to*
*/




#ifndef __CIPTVEPGLATESTEPGAVAILABLE_H__
#define __CIPTVEPGLATESTEPGAVAILABLE_H__

#include <e32std.h>
#include <e32base.h>

#include "IptvLiveEpgConstants.h"

/**
*	"Carrier" class to contain data for single program occurence in EPG xml
*	file. This class is used to communicate data from xml content handler
*	to EPG manager to be stored to the database.
*/
class CIptvEpgLatestEpgAvailable : public CBase
	{
	public:
		/**
		*	Default constructor
		*/
		IMPORT_C static CIptvEpgLatestEpgAvailable* NewL();
		
		virtual ~CIptvEpgLatestEpgAvailable();
		
	public: // Getter methods

		/**
		* @return program's service provider id.
		*/
		IMPORT_C TUint32 GetServiceId() const;
		
		/**
		* @return program start time.
		*/
		IMPORT_C TTime GetStartTime() const;
	
		
	public: // Setter methods


		/**
		* @param aServiceId Program's service provider id.
		*/
		IMPORT_C void SetServiceId( const TUint32& aServiceId );

		/**
		* @param aStartTime Program start time.
		*/
		IMPORT_C void SetStartTime( const TTime& aStartTime );

	private:
	
		CIptvEpgLatestEpgAvailable();
		
		void ConstructL();
	
	public: // Public instance variables
	
		TUint32 iServiceId;
			
		TTime iStartTime;

	
	};
	
#endif //__CIPTVEPGLATESTEPGAVAILABLE_H__

// End of file
