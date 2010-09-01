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




#ifndef __CIPTVPROVISIONINGRECOGNIZER_H
#define __CIPTVPROVISIONINGRECOGNIZER_H

// INCLUDES
#include <apmrec.h>

// DEFINITIONS

// Uid of this dll (defined in .mmp file)
const TUid KUidMimeIptvProvisioningRecognizer = { 0x10275126 };

// Implementation uid for this recognizer
const TInt KIptvProvRecImplUIDValue = 0x10281F19;
const TUid KUidIptvProvRecImpl = { KIptvProvRecImplUIDValue };

// Amount of mime types this recognizer can handle
const TInt KIptvSupportedMimeTypes( 2 );

// Exported mime types
_LIT8( KIptvConfMimetypeSimple, "vnd.nokia.iptv.config+xml" );
_LIT8( KIptvConfMimetypeApplication, "application/vnd.nokia.iptv.config+xml" );

// Strings to be searched to recognize content
_LIT8( KXmlFileDefinitionXml, "?xml" );
_LIT8( KIptvNameSpace, "xmlns:nv=\"www.nokia.com/multimedia/video-service\"");

/**
*	Class to make recognisation for Ipvt provisioning xml file
*/
class CIptvProvisioningRecognizer : public CApaDataRecognizerType
	{
	public:

    	/**
    	*	Default constructor. Calls CApaDataRecognizerType in it's initializer
    	*	list to complete construction
    	*/
    	CIptvProvisioningRecognizer();
    	
    	/**
    	*	Static method to create instance of this recognizer. This method is called
    	*	by the framework. Method pointer is delivered to the framework by
    	*	ImplementationTable table returned by ImplementationGroupProxy.
    	*	@param	None
    	*	@return	Pointer to newly created instance of the recognizer
    	*/
    	static CApaDataRecognizerType* CreateRecognizerL();
    	
    	/**
    	*	Method called by the framework. This method returns the amount of data
    	*	to be wanted for recognisation.
    	*	@param	None
    	*	@return	Preferred buffer size for recognisation
    	*/
    	virtual TUint PreferredBufSize();
    	
    	/**
    	*	Method to deliver supported data types by this recognizer.
    	*	@param	aIndex	Index pointing out which data type of the supported ones
    	*					is returned
    	*	@return	Corresponding data type of the requested index
    	*/
    	virtual TDataType SupportedDataTypeL( TInt aIndex ) const;

    private: // From CApaDataRecognizerType

    	/**
    	*	Overwritten method to handle recognisation.
    	*	@param	aName	The name of the data; typically this is a file name
    	*					containing the data to be recognized.
    	*	@param	aBuffer	Buffer containing PreferredBufSize() from the beginning
    	*					of the file to be recognized.
    	*	@return	None
    	*/
    	virtual void DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer );
	};
	
#endif // __CIPTVPROVISIONINGRECOGNIZER_H
