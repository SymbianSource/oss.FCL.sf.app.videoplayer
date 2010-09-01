/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/




#ifndef CIPTVEPGSERVICE_H
#define CIPTVEPGSERVICE_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"

#include "CIptvService.h"

// CLASS DECLARATION
class CIptvEpgService : public CBase
    {
    public:
        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvEpgService* NewL();

        /**
         * Destructor.
         */
        virtual ~CIptvEpgService();
        
    public: // New methods
    	/**
    	*	Setter for search url
    	*	@param	aSearchUrl	Url to be stored to this class's
    	*						iSearchUrl field
    	*	@return	None
    	*/
    	IMPORT_C void SetSearchUrlL( const TDesC& aSearchUrl );
    	
    	/**
    	*	Getter for search url
    	*	@param	None
    	*	@return	On return contains the reference to the search
    	*			url
    	*/
    	IMPORT_C TPtrC SearchUrl();

    private: // New functions           
        /**
        * C++ default constructor.
        */
        CIptvEpgService();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();
                  

	private: // Private instance variables

        /**
        * Search address
        */
        HBufC* iSearchUrl;

    public:     // Data members

        /**
        * Service address.       
        */
        TBuf<KIptvEpgServiceAddressMaxLength> iAddress;      

        /**
        * IAP
        */
        TUint32 iIap;

        /**
        * Username.
        */
        TBuf<KIptvEpgServiceUserNameMaxLength> iUserName;

        /**
        * Password.
        */
        TBuf<KIptvEpgServicePasswordMaxLength> iPassword;

        /**
        * UID.
        */
        TUid iUid;

        /**
        * Update set.
        */
        TUint32 iUpdateSet;

        /**
        * Service type;
        */
        CIptvService::TServiceType iServiceType;

        /**
        * Service id.
        */
        TUint32 iServiceId;

        /**
        * Last updated.
        */
        TTime iLastUpdated;
    };

#endif  // CIPTVEPGSERVICE_H

// End of File
