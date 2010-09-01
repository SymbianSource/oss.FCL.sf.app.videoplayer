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




#ifndef CIPTVTESTDLINFO_H
#define CIPTVTESTDLINFO_H

//  INCLUDES
#include <e32base.h>
#include "CIptvTestLogger.h"
#include "CIptvTestUtilities.h"
#include "CIptvTestDlFileInfo.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CIptvTestUtilities;
//class CIptvMyVideosGlobalFileId;


/**
*  
*  
*  
*
*  @since
*/
class CIptvTestDlInfo : public CBase
    {
    public:  // Constructors and destructor
        
         /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvTestDlInfo* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvTestDlInfo();
					
		IMPORT_C void DlStart(TUint32 aIapId,
					        TIptvServiceId aServiceId,
					        TIptvContentId aContentId,
					        TUint32 aDlContentNum, // a content can have multiple urls (CIptvMediaContent)
					        TIptvDownloadType aDlType);
        
		IMPORT_C void DlStartFailed(TIptvServiceId aServiceId,
	        						TIptvContentId aContentId,
	        						TIptvDlError aError);                                        
        
		IMPORT_C void DlGoing(TIptvServiceId aServiceId, 
							TIptvContentId aContentId, 
							CIptvMyVideosGlobalFileId& aFileId, 
							TUint32 aState,
							TIptvDlError aError );

		IMPORT_C void DlUpdateState(TIptvServiceId aServiceId, 
									TIptvContentId aContentId,
									TUint32 aState, 
									TIptvDlError aError );

		IMPORT_C void DlStop(TIptvServiceId aServiceId, 
							TIptvContentId aContentId, 
							TUint32 aState,
							TIptvDlError aError );
							
		IMPORT_C void DlTimeOut(TIptvServiceId aServiceId, 
							TIptvContentId aContentId 
							 );
		
    protected: 
	    
   
    private:
				/**
        * C++ default constructor.
        */
        CIptvTestDlInfo();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
        
     	TBool RemoveDlFileInfoItem(TUint32 index);
     		
        TBool FindDlFileInfo(TInt& aIndex,TIptvServiceId aServiceId,TIptvContentId aContentId);

    
    private:    // Data


        CIptvTestUtilities* iIptvTestUtilities;
        RPointerArray<CIptvTestDlFileInfo> iDlFileInfoArray;
    

   
    
    };

#endif      // CIPTVTESTDLINFO_H   
            
// End of File
