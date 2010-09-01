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
* Description:   Object to offer some common utility functions to Iptv engine testing*
*/




#ifndef CIPTVTESTECGUPDATEINFO_H
#define CIPTVTESTECGUPDATEINFO_H

//  INCLUDES
#include <e32base.h>
#include "CIptvTestLogger.h"
#include "CIptvTestUtilities.h"
// CONSTANTS
                                      
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CIptvTestUtilities;

/**
*  @since
*/
class CIptvTestEcgUpdateInfo : public CBase
    {
    public:  // Constructors and destructor
        
         /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvTestEcgUpdateInfo* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvTestEcgUpdateInfo();
							
		IMPORT_C void EcgUpdateStart(TUint32 aServiceId, TUint32 aIapId);
		IMPORT_C void EcgUpdateStop(TUint32 aServiceId, TInt aError, TInt aInfo);
		IMPORT_C void WriteLogL(TRefByValue<const TDesC> aFmt, ... );
		IMPORT_C void WriteLogL(TDesC& aStr);        
		
    protected: 
	    
   
    private:
				/**
        * C++ default constructor.
        */
        CIptvTestEcgUpdateInfo();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
        
        //TUint32 GetSystemTime();

    
    private:    // Data

    CIptvTestLogger* iIptvTestLogger;
    CIptvTestUtilities* iIptvTestUtilities;
    
   
    
    };

#endif      // CIPTVTESTECGUPDATEINFO_H   
            
// End of File
