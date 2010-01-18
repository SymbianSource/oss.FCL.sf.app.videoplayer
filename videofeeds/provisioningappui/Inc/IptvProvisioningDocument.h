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
* Description: 
*
*/




#ifndef IPTVPROVISIONINGDOCUMENT_H
#define IPTVPROVISIONINGDOCUMENT_H

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;
class CIptvProvisioningAppUi;

// CLASS DECLARATION
/**
*  CIptvProvisioningDocument application class.
*/
class CIptvProvisioningDocument : public CAknDocument
    {
    public:
        /**
        * C++ constructor.
        * @param CEikApplication& aApp
        * @return void
        */
        CIptvProvisioningDocument( CEikApplication& aApp ) :
        							CAknDocument(aApp) { }

        /**
        * Destructor.
        * @param None
        * @return void
        */
        virtual ~CIptvProvisioningDocument();


	// From CEikDociment
    CFileStore* OpenFileL( TBool aDoOpen, const TDesC& aFilename, RFs& aFs );
	void OpenFileL( CFileStore*& aFileStore, RFile& aFile );
	

    private:
        /**
        * C++ default constructor.
        * @param None
        * @return void
        */
        CIptvProvisioningDocument();

        /**
        * This method makes an application hidden so that it is not visible
        * for example in Applications list and FastSwap window.
        * @param CApaWindowGroupName* aWgName
        * @return void        
        */
        void UpdateTaskNameL( CApaWindowGroupName* aWgName );
        
    private:// from CAknDocument
        /**
        * Create CIptvProvisioningDocument "App UI" object.
        * @param None
        * @return CEikAppUi*        
        */
        CEikAppUi* CreateAppUiL();
        
    private:
    };

#endif      // IPTVPROVISIONINGDOCUMENT_H
            
// End of File
