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




#include "CIptvEpgSchedule.h"
#include "CIptvEpgProgramWithSchedule.h"

EXPORT_C CIptvEpgProgramWithSchedule* CIptvEpgProgramWithSchedule::NewL()
	{
	CIptvEpgProgramWithSchedule* self = new (ELeave) CIptvEpgProgramWithSchedule();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

CIptvEpgProgramWithSchedule::CIptvEpgProgramWithSchedule() :
    iEndTime( KMaxTInt64 )
	{
	}
	
CIptvEpgProgramWithSchedule::~CIptvEpgProgramWithSchedule()
	{
	delete iSchedule;
	}
	
void CIptvEpgProgramWithSchedule::ConstructL()
	{
	CIptvEpgProgram::ConstructL();
	iSchedule = CIptvEpgSchedule::NewL();
	}
	
EXPORT_C CIptvEpgSchedule& CIptvEpgProgramWithSchedule::Schedule()
	{
	iSchedule->SetChannelId( this->ChannelId() );
	iSchedule->SetProgramId( this->ProgramId() );
	iSchedule->SetServiceId( this->ServiceId() );
	iSchedule->SetStartTime( iStartTime );
	iSchedule->SetEndTime( iEndTime );

	return *iSchedule;	
	}
	
EXPORT_C void CIptvEpgProgramWithSchedule::SetStartTime( const TTime& aStartTime )
	{
	iStartTime = aStartTime;
	}
	
EXPORT_C TTime CIptvEpgProgramWithSchedule::StartTime() const
	{
	return iStartTime;
	}
	
EXPORT_C void CIptvEpgProgramWithSchedule::SetEndTime( const TTime& aEndTime )
	{
	iEndTime = aEndTime;
	}
	
EXPORT_C TTime CIptvEpgProgramWithSchedule::EndTime() const
	{
	return iEndTime;
	}

// End of file
