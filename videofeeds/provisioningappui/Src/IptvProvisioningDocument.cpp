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




// INCLUDE FILES
#include <apgwgnam.h>
#include "IptvProvisioningDocument.h"
#include "IptvProvisioningAppUi.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CIptvProvisioningDocument::~CIptvProvisioningDocument()
// ----------------------------------------------------------------------------

CIptvProvisioningDocument::~CIptvProvisioningDocument()
    {
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningDocument::CreateAppUiL()
// Creates CIptvProvisioningAppUi
// ----------------------------------------------------------------------------
CEikAppUi* CIptvProvisioningDocument::CreateAppUiL()
    {
    // Create new isntance of appui class
    return new(ELeave) CIptvProvisioningAppUi();
    }

// ----------------------------------------------------------------------------
// CIptvProvisioningDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
// ----------------------------------------------------------------------------
void CIptvProvisioningDocument::UpdateTaskNameL( CApaWindowGroupName* aWgName )
	{
	// Set task name to be hidden from the tasklist
	CAknDocument::UpdateTaskNameL( aWgName );
	aWgName->SetHidden( ETrue );
	}

// ----------------------------------------------------------------------------
// CIptvProvisioningDocument::OpenFileL
// ----------------------------------------------------------------------------
CFileStore* CIptvProvisioningDocument::OpenFileL(
    TBool aDoOpen,
    const TDesC& aFilename,
    RFs& aFs )
	{
	if ( aDoOpen )
	    {
        RFile file;
        TInt error( file.Open( aFs, aFilename, EFileShareAny | EFileRead ) );
        if ( error == KErrNone )
            {
            static_cast<CIptvProvisioningAppUi*>( iAppUi )->HandleFileL( file );
            file.Close();
            }
	    }
	
	return NULL;
	}


// ----------------------------------------------------------------------------
// CIptvProvisioningDocument::OpenFileL
// ----------------------------------------------------------------------------
void CIptvProvisioningDocument::OpenFileL(
    CFileStore*& /*aFileStore*/,
    RFile& aFile )
	{
	// Request appui to handle this file
	static_cast<CIptvProvisioningAppUi*>( iAppUi )->HandleFileL( aFile );
	}


// End of File
