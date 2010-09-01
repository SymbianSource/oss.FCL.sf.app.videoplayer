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




#ifndef CIPTVPROVISIONINGPROCESSOR_H
#define CIPTVPROVISIONINGPROCESSOR_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include <f32file.h>
#include <xml/parser.h>

// FORWARD DECLARATIONS
class CIptvPPContentHandler;
class CIptvServices;
using namespace Xml;
class Xml::CParser;

// CLASS DECLARATION
class CIptvProvisioningProcessor : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvProvisioningProcessor* NewL();

        /**
         * Destructor.
         */
        virtual ~CIptvProvisioningProcessor();

        /**
         * Stop parsing (too many services).
         */
        void StopParsing();

    public:
        
        /**
         * Local provisioning.
         *
         * @param aFileName Full path to service xml-file.
         * @param aError    Provisioning error code. If aFileName is incorrect value is KErrNotFound.
         *                  If xml-file uses wrong schema, error code is KIptvSchemaMismatch. Otherwise
         *                  aError is error code from XML-parser, see XmlParserErrors.h.
         *  @return         Pointer to CIptvServices which contains array of provisioned services.
         */
        IMPORT_C CIptvServices* LocalProvisionL(const TDesC& aFileName, TInt& aError); 
        
        /**
         * External provisioning.
         *
         * @param aFile     Open RFile reference to service xml-file.
         * @param aError    Provisioning error code. 
         *                  If xml-file uses wrong schema, error code is KIptvSchemaMismatch. Otherwise
         *                  aError is error code from XML-parser, see XmlParserErrors.h.
         *  @return         Pointer to CIptvServices which contains array of provisioned services.
         */
        IMPORT_C CIptvServices* ExternalProvisionL(RFile& aFile, TInt& aError);
         
    private:    // New functions
        
        /**
        * C++ default constructor.
        */
        CIptvProvisioningProcessor();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();                       
          
    private:

        /**
        * Content handler.
        */
        CIptvPPContentHandler* iHandler;

        /**
        * Parser.
        */
        CParser* iParser;                    
        
        /**
	    * File server session
	    */
	    RFs iFs;

        /**
	    * File content.
	    */
        HBufC8* iXmlFileContent;
	        
        /**
	    * Local provisioning.
	    */
        TBool iLocal;
    };

#endif  // CIPTVPROVISIONINGPROCESSOR_H

// End of File
