/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   implements logging functionality*
*/




// INCLUDE FILES
#include "CIptvTestLogger.h"
//#include "MIptvTestTimerObserver.h"
#include "VCXTestLog.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvTestLogger::
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTestLogger* CIptvTestLogger::NewL(const TDesC& aDir, const TDesC& aName)
    {
    VCXLOGLO1(">>>CIptvTestLogger::NewL");
    CIptvTestLogger* self = new (ELeave) CIptvTestLogger( );
    CleanupStack::PushL(self);
    self->ConstructL(aDir, aName);
    CleanupStack::Pop();
    VCXLOGLO1("<<<CIptvTestLogger::NewL");
    return self;
    }
// -----------------------------------------------------------------------------
// CIptvTestLogger::~CIptvTestLogger
// destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTestLogger::~CIptvTestLogger()
    {
    VCXLOGLO1(">>>CIptvTestLogger::~CIptvTestLogger");

    VCXLOGLO1("<<<CIptvTestLogger::~CIptvTestLogger");
    }
// -----------------------------------------------------------------------------
// CIptvTestLogger::~CIptvTestLogger
// destructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTestLogger::Log(TRefByValue<const TDesC> aFmt,... )
    {
    VA_LIST argptr; //pointer to argument list
    VA_START( argptr, aFmt );
    RFileLogger::WriteFormat(iFileDir,iFileName,EFileLoggingModeAppend,aFmt,argptr);
    }
// -----------------------------------------------------------------------------
// CIptvTestLogger::CIptvTestLogger
//
// -----------------------------------------------------------------------------
//
CIptvTestLogger::CIptvTestLogger()
    {
    VCXLOGLO1(">>>CIptvTestLogger::CIptvTestLogger");

    VCXLOGLO1("<<<CIptvTestLogger::CIptvTestLogger");
    }
// -----------------------------------------------------------------------------
// CIptvTestLogger::ConstructL()
//
// -----------------------------------------------------------------------------
//
void CIptvTestLogger::ConstructL(const TDesC& aDir, const TDesC& aName)
    {
    VCXLOGLO1(">>>CIptvTestLogger::ConstructL");
    iFileDir.Append(aDir);
    iFileName.Append(aName);
    VCXLOGLO1("<<<CIptvTestLogger::ConstructL");
    }
// ---------------------------------

//  End of File
