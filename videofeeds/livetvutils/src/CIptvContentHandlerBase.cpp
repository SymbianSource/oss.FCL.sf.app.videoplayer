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
* Description:    Content handler for xml reader callbacks*
*/




// SYSTEM INCLUDES
#include <f32file.h>					// For RFs
#include <xml/wbxmlextensionhandler.h>
#include "IptvLiveLogger.h"
// USER INCLUDES
#include "CIptvContentHandlerBase.h"


// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::CIptvContentHandlerBase()
//
// Constructor
// ----------------------------------------------------------------------------
EXPORT_C CIptvContentHandlerBase::CIptvContentHandlerBase()
	{
	}
	
// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::~CIptvContentHandlerBase()
//
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CIptvContentHandlerBase::~CIptvContentHandlerBase()
	{
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::ConstructL()
//
// Second phase constructor
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::BaseConstructL()
	{
	}
	
// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnStartDocumentL()
//
// Setter for service provider id
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnStartDocumentL(
										const RDocumentParameters& /*aDocParam*/,
										TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnEndDocumentL()
//
// Setter for service provider id
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnEndDocumentL( TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnStartElementL()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnStartElementL( const RTagInfo& /*aElement*/,
						  						      const RAttributeArray& /*aAttributes*/, 
						  						  	  TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}


// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnEndElementL()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnEndElementL( const RTagInfo& /*aElement*/,
													TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnContentL()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnContentL( const TDesC8& /*aBytes*/,
										  TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnStartPrefixMappingL()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnStartPrefixMappingL( const RString& /*aPrefix*/,
														    const RString& /*aUri*/, 
															TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnEndPrefixMappingL()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnEndPrefixMappingL( const RString& /*aPrefix*/,
													  TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnIgnorableWhiteSpaceL()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/,
														 	 TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnSkippedEntityL()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnSkippedEntityL( const RString& /*aName*/,
												   	   TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnProcessingInstructionL()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnProcessingInstructionL( const TDesC8& /*aTarget*/,
								   						   	   const TDesC8& /*aData*/, 
								   						   	   TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::OnError()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C void CIptvContentHandlerBase::OnError( TInt /*aErrorCode*/ )
	{
	// If needed implement in derived class
	}

// ----------------------------------------------------------------------------
// CIptvContentHandlerBase::GetExtendedInterface()
//
// From MContentHandler
// ----------------------------------------------------------------------------
EXPORT_C TAny* CIptvContentHandlerBase::GetExtendedInterface( const TInt32 /*aUid*/ )
	{
	// If needed implement in derived class
	return NULL;
	}

// end of file
