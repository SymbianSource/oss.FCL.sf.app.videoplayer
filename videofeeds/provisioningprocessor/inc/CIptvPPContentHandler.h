/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef CIPTVPPCONTENTHANDLER_H
#define CIPTVPPCONTENTHANDLER_H

#include <e32std.h>
#include <xml/contenthandler.h>
#include "IptvEngineUids.h"

const TInt KMaxLanguageCodeStringLength = 5;
const TInt KIptvProtocolLength = 15;

class CIptvProvisioningProcessor;
class CIptvService;
class CIptvServices;

using namespace Xml;

class CIptvPPContentHandler : public CBase,
                              public MContentHandler
    {

public:

    /**
     * Two-phased constructor.
     * @param
     */
    static CIptvPPContentHandler* NewL(
        CIptvProvisioningProcessor& aProcessor,
        const RFs& aFs );

    /**
     * Destructor.
     */
    ~CIptvPPContentHandler();

    /**
     * Reset content handler.
     */
    void Reset();

    /**
     * Set mode.
     */
    void SetMode( TBool aLocal );

    /**
     * Set services array.
     */
    void SetServicesArray( CIptvServices* aServices );

    /**
     * Get error code.
     */
    TInt GetError() const { return iError; };

// From MContentHandler

    /**
     * This method is a callback to indicate the start of the document.
     * @param              aDocParam Specifies the various parameters of the document.
     * @param              aDocParam.iCharacterSetName The character encoding of the document.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnStartDocumentL( const RDocumentParameters& aDocParam, TInt aErrorCode );

    /**
     * This method is a callback to indicate the end of the document.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnEndDocumentL( TInt aErrorCode );

    /**
     * This method is a callback to indicate an element has been parsed.
     * @param              aElement is a handle to the element's details.
     * @param              aAttributes contains the attributes for the element.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnStartElementL( const RTagInfo& aElement,
                          const RAttributeArray& aAttributes,
                          TInt aErrorCode );

    /**
     * This method is a callback to indicate the end of the element has been reached.
     * @param              aElement is a handle to the element's details.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );

    /**
     * This method is a callback that sends the content of the element.
     * Not all the content may be returned in one go. The data may be sent in chunks.
     * When an OnEndElementL is received this means there is no more content to be sent.
     * @param              aBytes is the raw content data for the element.
     *                     The client is responsible for converting the data to the
     *                     required character set if necessary.
     *                     In some instances the content may be binary and must not be converted.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnContentL( const TDesC8& aBytes, TInt aErrorCode );

    /**
     * This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
     * This method is always called before the corresponding OnStartElementL method.
     * @param              aPrefix is the Namespace prefix being declared.
     * @param              aUri is the Namespace URI the prefix is mapped to.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnStartPrefixMappingL( const RString& aPrefix,
                                const RString& aUri,
                                TInt aErrorCode );

    /**
     * This method is a notification of the end of the scope of a prefix-URI mapping.
     * This method is called after the corresponding DoEndElementL method.
     * @param              aPrefix is the Namespace prefix that was mapped.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );

    /**
     * This method is a notification of ignorable whitespace in element content.
     * @param              aBytes are the ignored bytes from the document being parsed.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );

    /**
     * This method is a notification of a skipped entity. If the parser encounters an
     * external entity it does not need to expand it - it can return the entity as aName
     * for the client to deal with.
     * @param              aName is the name of the skipped entity.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnSkippedEntityL( const RString& aName, TInt aErrorCode );

    /**
     * This method is a receive notification of a processing instruction.
     * @param              aTarget is the processing instruction target.
     * @param              aData is the processing instruction data. If empty none was supplied.
     * @param              aErrorCode is the error code.
     *                     If this is not KErrNone then special action may be required.
     */
    void OnProcessingInstructionL( const TDesC8& aTarget,
                                   const TDesC8& aData,
                                   TInt aErrorCode );

    /**
     * This method indicates an error has occurred.
     * @param              aError is the error code
     */
    void OnError( TInt aErrorCode );

    /**
     * This method obtains the interface matching the specified uid.
     * @return             0 if no interface matching the uid is found.
     *                     Otherwise, the this pointer cast to that interface.
     * @param              aUid the uid identifying the required interface.
     */
    TAny* GetExtendedInterface( const TInt32 aUid );

private: // New functions

    /**
     * C++ default constructor.
     */
    CIptvPPContentHandler(
        CIptvProvisioningProcessor& aProcessor,
        const RFs& aFs );

    /**
     * Add service.
     */
    void AddServiceL();

    /**
     * Checks if input string includes only number characters.
     *
     * @param aString Input string.
     * @return ETrue if string includes only number characters.
     */
    TBool HasOnlyNumbers( const TDesC& aString ) const;

    /**
     * Get service type.
     */
    void GetServiceTypeL( const RAttributeArray& aAttributes );

    /**
     * Get language code
     */
    void GetLanguageCode( const RAttributeArray& aAttributes );

    /**
     * Set service type.
     */
    void SetServiceTypeL( const TDesC& aServiceType );

    /**
     * Complete title.
     */
    void CompleteTitleL();

    /**
     * Complete description.
     */
    void CompleteDescriptionL();

    /**
     * Complete service group Id.
     */
    void CompleteGroupIdL();

    /**
     * Complete use category.
     */
    void CompleteUseCategoryL();

    /**
     * Complete Uri.
     */
    void CompleteUriL();

    /**
     * Complete Iap.
     */
    void CompleteIapL();

    /**
     * Complete Username.
     */
    void CompleteUsernameL();

    /**
     * Complete password.
     */
    void CompletePasswordL();

    /**
     * Complete account mgmt uri.
     */
    void CompleteAccountMgmtUri();

    /**
     * Complete icon Uri.
     */
    void CompleteIconUrlL();

    /**
     * Complete icon path.
     */
    void CompleteIconPathL();

    /**
     * Complete upload provider.
     */
    void CompleteUploadProvider();

    /**
     * Complete application UID.
     */
    void CompleteAppUIDL();

    /**
     * Complete search uri
     */
    void CompleteSearchUriL();

    /**
     * Complete record uri.
     */
    void CompleteRecordUriL();
    
    /**
     * Set protocol.
     */
    void SetProtocolL( const TDesC& aProtocol );

    /**
     * Is valid service.
     * @param aError Cause for invalid service.
     */
    TBool IsValidServiceL( TInt& aError );

    /**
     * Set given icon path. Whether it is drive independent filepath,
     * searches correct drive and fix it
     *
     * @param aIconPath Icon path.
     */
    void SetIconPathL( const TDesC& aIconPath );

private:

    /**
     * Service Title state.
     */
    enum TServiceTitleState
        {
        EIptvServiceTitleNone,
        EIptvServiceTitleNormal,
        EIptvServiceTitleStore
        };

    /**
     * State.
     */
    TInt iState;

    /**
     * Service count.
     */
    TInt iServiceCount;

    /**
     * Reference to CIptvProvisioningProcessor.
     */
    CIptvProvisioningProcessor& iProcessor;

    /**
     * Pointer to service object.
     * Own.
     */
    CIptvService* iService;

    /**
     * Local provisioning.
     */
    TBool iLocal;

    /**
     * Buffer.
     * Own.
     */
    HBufC* iBuffer;

    /**
     * Servies array.
     * Not own,
     */
    CIptvServices* iServices;

    /**
     * Error code.
     */
    TInt iError;

    /**
     * Language code currently in use for the device
     */
    TLanguage iCurrentDeviceLanguage;

    /**
     * Current language code from the xml file
     */
    TBuf<KMaxLanguageCodeStringLength> iCurrentLanguageCode;

    /**
     * Symbian enumeration for iCurrentLanguageCode
     */
    TLanguage iCurrentlyParsedDetailLanguage;

    /**
     * Flag to identify if a single language data for title is set
     */
    TBool iTitleLanguageDataSet;

    /**
     * Flag to identify if a single language data for description is set
     */
    TBool iDescriptionLanguageDataSet;

    /**
     * Flag to identify if a single language data for icon URI is set
     */
    TBool iIconUriLanguageDataSet;

    /**
     * Service group feed indication.
     */
    TBool iServiceGroupFeed;

    /**
     * Grouped service indication.
     */
    TUint iServiceGroupId;

    /**
     * Protocol.
     */
    TBuf<KIptvProtocolLength> iProtocol;

    /**
     * Service Title state.
     */
    TServiceTitleState iServiceTitleState;

    /**
     * Icon url waiting to be saved or ignored according to title.
     * Own.
     */
    HBufC* iPendingIcon;

    /**
     * File server session
     */
    RFs iFs;

    };

#endif      // CIPTVPPCONTENTHANDLER_H
