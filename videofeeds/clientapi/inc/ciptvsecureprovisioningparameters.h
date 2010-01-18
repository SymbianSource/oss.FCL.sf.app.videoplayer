/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Collects various parameters needed in secure provisioning service url.*
*/





#ifndef CIPTVSECUREPROVISIONINGPARAMETERS_H
#define CIPTVSECUREPROVISIONINGPARAMETERS_H

//  INCLUDES
#include <e32base.h>
#include <etelmm.h> 

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CRepository;
class C3DESEncryptor;
class C3DESDecryptor;


// CLASS DECLARATION

/**
 * Secure provisioning service url parameters getter.
 * @lib IptvClientApi.dll
 */
class CIptvSecureProvisioningParameters : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return pointer to new CIptvSecureProvisioningParameters instance.
        */
        static CIptvSecureProvisioningParameters* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CIptvSecureProvisioningParameters();
        
    public:
        /**
        * CIptvService::ESecureProvisioningServer type service needs additional dynamic
        * parameters after the address url (CIptvService::GetAddress()), this function
        * generates the dynamic part.
        *
        * @return Pointer to dynamic part of the url, caller gets ownership of the pointer.
        */
        HBufC* SecureProvisioningUrlParametersL();
         
    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Gets device model (n80, n95,...). 
        * @param aModel Descriptor where model is written.
        */
        void FetchProductModelL( TDes8& aModel );

        /**
        * Writes Home network id and home country code to cenrep.
        */
        void FetchHomeNetworkInfoL();

        /**
        * Creates aFormattedMnc.
        *
        * @param aMcc            Country code
        * @param aUnformattedMnc Network id
        * @param aFormattedMnc   Formatted network code
        */        
        void FormatMncCodeL(
            const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc,
            const RMobilePhone::TMobilePhoneNetworkIdentity aUnformattedMnc,
            RMobilePhone::TMobilePhoneNetworkIdentity& aFormattedMnc );

        /**
        * Returns home country code.
        */
        const RMobilePhone::TMobilePhoneNetworkCountryCode HomeMccL();

        /**
        * Returns home network id.
        */
        const RMobilePhone::TMobilePhoneNetworkIdentity HomeMncL();

        /**
        * Sets home country code to central repository.
        * @param aMcc home country code.
        */
        void SetHomeMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode aMcc );
        
        /**
        * Sets home network id to central repository.
        * @param aMnc home network ID.
        */
        void SetHomeMncL( const RMobilePhone::TMobilePhoneNetworkIdentity aMnc );

        /**
        * Gets key info from cenrep.
        * @param aKeyInfo Descriptor where key info is written.
        */
        void GetKeyInfoL( TDes8& aKeyInfo );

        /**
        * Creates nonce and saves it to central repository.
        * @param aTimeOut not used.
        * @param aNonce new nonce is written here.
        */
        void CreateNewNonceL( const TUint /*aTimeOut*/, TDes8& aNonce );

        /**
        * Returns random charachter.
        * @param aSeed a number which is used to initialize random generator.
        */ 
        TUint RandomCharacter( TInt64& aSeed );
        
    private:

         /**
         * Pointer to repository, used for storing dynamic url parameter fields.
         * Own.
         */
         CRepository* iRepository;
         
#ifndef __WINS__ // encryptor is not used in emulator
         
         /**
         * Used for encrypting nounce to central repository.
         * Own.
         */
         C3DESEncryptor* iEncryptor;
#endif

         RTelServer   iServer;
         RMobilePhone iPhone;

    };

#endif      // CIPTVSECUREPROVISIONINGPARAMETERS_H   
            
// End of File
