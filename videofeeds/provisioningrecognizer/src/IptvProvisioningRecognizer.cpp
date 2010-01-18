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




#include "IptvProvisioningRecognizer.h"
#include "ecom/implementationproxy.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvProvisioningRecognizer::CIptvProvisioningRecognizer
// Default constructor
// ---------------------------------------------------------
//
CIptvProvisioningRecognizer::CIptvProvisioningRecognizer() :
				CApaDataRecognizerType( KUidMimeIptvProvisioningRecognizer,
										CApaDataRecognizerType::EHigh )
	{
	iCountDataTypes = KIptvSupportedMimeTypes;
	}

// ---------------------------------------------------------
// CIptvProvisioningRecognizer::PreferredBufSize
// Overwritten method from CApaDataRecognizerType
// ---------------------------------------------------------
//
TUint CIptvProvisioningRecognizer::PreferredBufSize()
	{
	return 0x200;
	}

// ---------------------------------------------------------
// CIptvProvisioningRecognizer::SupportedDataTypeL
// Overwritten method from CApaDataRecognizerType
// ---------------------------------------------------------
//
TDataType CIptvProvisioningRecognizer::SupportedDataTypeL( TInt aIndex ) const
	{
	__ASSERT_DEBUG( aIndex >= 0 &&
					aIndex < KIptvSupportedMimeTypes, User::Invariant() );

	switch( aIndex )
		{
		case 0:
			{
			TDataType type( KIptvConfMimetypeSimple );
			return type;
			}
		case 1:
			{
			TDataType type( KIptvConfMimetypeApplication );
			return type;
			}			
		default:
		    {
		    }
			break;
		}

	return TDataType();
	}

// ---------------------------------------------------------
// CIptvProvisioningRecognizer::DoRecognizeL
// Overwritten method from CApaDataRecognizerType
// ---------------------------------------------------------
//
void CIptvProvisioningRecognizer::DoRecognizeL( const TDesC& /*aName*/,
												const TDesC8& aBuffer )
	{
    iConfidence = ENotRecognized;
    iDataType = TDataType();
	if ( aBuffer.FindF( KIptvNameSpace ) != KErrNotFound &&
         aBuffer.FindF( KXmlFileDefinitionXml ) != KErrNotFound )
		{
		iConfidence = ECertain;
		iDataType = TDataType( KIptvConfMimetypeApplication );
		}
	}

// ---------------------------------------------------------
// CIptvProvisioningRecognizer::CreateRecognizerL
// Static method to create instance of CIptvProvisioningRecognizer
// ---------------------------------------------------------
//
CApaDataRecognizerType* CIptvProvisioningRecognizer::CreateRecognizerL()
	{
	return new( ELeave ) CIptvProvisioningRecognizer();
	}

// ---------------------------------------------------------
// ImplementationTable
// Table containing the data concerning CIptvProvisioningRecognizer 
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(
				KIptvProvRecImplUIDValue,
				CIptvProvisioningRecognizer::CreateRecognizerL )
	};

// ---------------------------------------------------------
// ImplementationGroupProxy
// Function called by framework to return data about this recognizer
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
												TInt& aTableCount )
	{
	aTableCount =
			sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}
