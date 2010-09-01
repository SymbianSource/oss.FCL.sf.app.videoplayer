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




#ifndef __CIPTVCONTENTHANDLERBASE_H
#define __CIPTVCONTENTHANDLERBASE_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <xml/contenthandler.h>

using namespace Xml;

/**
*	Base class for content handling of xml parsing.
*	This class implemenets MContentHandler functions as
*	an empty implementations and derived classes needs to take
*	care of implementing those which are needed to fullfill
*	their requirements.
*	Most likely functions that needs to be overwritten are
*	OnStartElementL, OnEndElementL and OnContentL
*/
class CIptvContentHandlerBase : public CBase,
								public MContentHandler
	{
	public:
	/**
	*	Destructor
	*/
	IMPORT_C virtual ~CIptvContentHandlerBase();
	
	public: // From MContentHandler
	/**
	This method is a callback to indicate the start of the document.
	@param				aDocParam Specifies the various parameters of the document.
	@param				aDocParam.iCharacterSetName The character encoding of the document.
	@param				aErrorCode is the error code. 
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnStartDocumentL( const RDocumentParameters& aDocParam, TInt aErrorCode );

	/**
	This method is a callback to indicate the end of the document.
	@param				aErrorCode is the error code. 
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnEndDocumentL( TInt aErrorCode );

	/**
	This method is a callback to indicate an element has been parsed.
	@param				aElement is a handle to the element's details.
	@param				aAttributes contains the attributes for the element.
	@param				aErrorCode is the error code.
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnStartElementL( const RTagInfo& aElement,
						  const RAttributeArray& aAttributes, 
						  TInt aErrorCode );

	/**
	This method is a callback to indicate the end of the element has been reached.
	@param				aElement is a handle to the element's details.
	@param				aErrorCode is the error code.
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );

	/**
	This method is a callback that sends the content of the element.
	Not all the content may be returned in one go. The data may be sent in chunks.
	When an OnEndElementL is received this means there is no more content to be sent.
	@param				aBytes is the raw content data for the element. 
						The client is responsible for converting the data to the 
						required character set if necessary.
						In some instances the content may be binary and must not be converted.
	@param				aErrorCode is the error code.
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnContentL( const TDesC8& aBytes, TInt aErrorCode );

	/**
	This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
	This method is always called before the corresponding OnStartElementL method.
	@param				aPrefix is the Namespace prefix being declared.
	@param				aUri is the Namespace URI the prefix is mapped to.
	@param				aErrorCode is the error code.
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnStartPrefixMappingL( const RString& aPrefix,
										const RString& aUri, 
										TInt aErrorCode);

	/**
	This method is a notification of the end of the scope of a prefix-URI mapping.
	This method is called after the corresponding DoEndElementL method.
	@param				aPrefix is the Namespace prefix that was mapped.
	@param				aErrorCode is the error code.
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );

	/**
	This method is a notification of ignorable whitespace in element content.
	@param				aBytes are the ignored bytes from the document being parsed.
	@param				aErrorCode is the error code.
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );

	/**
	This method is a notification of a skipped entity. If the parser encounters an 
	external entity it does not need to expand it - it can return the entity as aName 
	for the client to deal with.
	@param				aName is the name of the skipped entity.
	@param				aErrorCode is the error code.
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnSkippedEntityL( const RString& aName, TInt aErrorCode );

	/**
	This method is a receive notification of a processing instruction.
	@param				aTarget is the processing instruction target.
	@param				aData is the processing instruction data. If empty none was supplied.
	@param				aErrorCode is the error code.
						If this is not KErrNone then special action may be required.
	*/
	IMPORT_C void OnProcessingInstructionL( const TDesC8& aTarget,
								   		   const TDesC8& aData, 
								   		   TInt aErrorCode);

	/**
	This method indicates an error has occurred.
	@param				aError is the error code
	*/
	IMPORT_C void OnError( TInt aErrorCode );

	/**
	This method obtains the interface matching the specified uid.
	@return				0 if no interface matching the uid is found.
						Otherwise, the this pointer cast to that interface.
	@param				aUid the uid identifying the required interface.
	*/
	IMPORT_C TAny* GetExtendedInterface( const TInt32 aUid );

 		
	protected:
	/**
	*	Default consturtor
	*/
	IMPORT_C CIptvContentHandlerBase();
	
	/**
	*	Symbian second phase constructor can contain code that might leave.
	*	This function completes CIptvThomsonContentHandlerBase construction.
	*	@param	None
	*	@return	None
	*/
	IMPORT_C void BaseConstructL();
	
	};

#endif // __CIptvThomsonContentHandlerBase_H
