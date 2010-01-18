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




#ifndef CIPTVTESTLOGGER_H
#define CIPTVTESTLOGGER_H

//  INCLUDES
#include <e32base.h>
#include <flogger.h>
// CONSTANTS
                                            
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  
*  
*  
*
*  @since
*/
class CIptvTestLogger : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvTestLogger* NewL(const TDesC& aDir, const TDesC& aName);
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvTestLogger();


    public: // New functions
			IMPORT_C void  Log(TRefByValue<const TDesC> aFmt,... );
		
					
    protected: //from base classes
	    
   
    private:

        /**
        * C++ default constructor.
        */
        CIptvTestLogger();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC& aDir, const TDesC& aName);
        

    
    private:    // Data

    TBuf<255> iFileDir;
    TBuf<255> iFileName;

    };


#endif      // CIPTVTESTLOGGER_H   
            
// End of File
