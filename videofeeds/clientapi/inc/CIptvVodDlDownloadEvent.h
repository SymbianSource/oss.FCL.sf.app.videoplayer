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
* Description:    Vod Dl event object*
*/





#ifndef CIPTVVODDLDOWNLOADEVENT_H
#define CIPTVVODDLDOWNLOADEVENT_H

//  INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"

#include "MIptvEvent.h"
#include "TIptvVodDlDownloadId.h"

// CONSTANTS


// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class RDesReadStream;
class RDesWriteStream;
class CIptvMyVideosGlobalFileId;

/**
*  Used for sending download events from VOD DL Manager to client.
*
*  @lib IptvClientApi.lib
*/
class CIptvVodDlDownloadEvent : public CBase, public MIptvEvent
    {
    public:
    
        /**
        * Identifies the type of the event. Type specifies which member fields contain
        * valid data.
        */
        enum TEvent
            {
            /**
            * VOD download state change event.
            * All members are valid.
            */ 
            EDownloadState = 0,
            
            /**
            * Response event to CIptvVodDlClient::DownloadReqL() method call.
            * iGlobalFileIsValid and iServiceIdAndContentIdAreValid members indicate which ids are usable.
            * iProgress and iState members do not contain valid data.
            * Download request failed, no download instance was created, iError and iGlobalError members
            * contain reason codes.
            */
            EDownloadReqFailed,

            /**
            * Response event to CIptvVodDlClient::DownloadReqL() method call.
            * iGlobalFileIsValid and iServiceIdAndContentIdAreValid members indicate which ids are usable.
            * iProgress and iState members do not contain valid data.
            * Download request succeeded, download instance was created, EDownloadState
            * messages will follow.
            */
            EDownloadReqSucceeded,
            
            /**
            * Response event to CIptvVodDlClient::CancelDownloadReqL() method call.
            * iGlobalFileIsValid and iServiceIdAndContentIdAreValid members indicate which ids are usable.
            * iProgress and iState members do not contain valid data.
            * Request failed, iError and iGlobalError members contain reason codes.
            */
            ECancelReqFailed,
            
            /**
            * Response event to CIptvVodDlClient::CancelDownloadReqL() method call.
            * iGlobalFileIsValid and iServiceIdAndContentIdAreValid members indicate which ids are usable.
            * iProgress and iState members do not contain valid data.
            * Request succeeded and cancel process started, EDownloadState messages
            * show when download is gone (iState = KIptvVodDlDownloadDeletedCompletely).
            * Notice that client might get ECancelReqSucceeded message after KIptvVodDlDownloadDeletedCompletely.
            */            
            ECancelReqSucceeded,
            
            /**
            * Event indicating that details of download item(s) has been modified. Occurs if EPG
            * data related to download has been modified during download. Forces client to rebuild
            * its information about on-going downloads.
            */
            EDownloadItemsModified,
            
            /**
            * Response event to CIptvVodDlClient::DownloadReqL() method call.
            * This is response for single file download.
            * iServiceId is only usable ident. iContentIndex will contain original request.
            * iState contain result. iProgress member do not contain valid data.
            * Download request is completed.
            */
            EDownloadSingleFileCompleted
            };
            
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvVodDlDownloadEvent* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvVodDlDownloadEvent();

    public:
        
        //from MIptvEvent
        IMPORT_C void InternalizeL(RReadStream& aStream);
        IMPORT_C void ExternalizeL(RWriteStream& aStream);
        IMPORT_C TUint32 CountExternalizeSize();
        IMPORT_C MIptvEvent* GetCopyL();
        IMPORT_C void Destruct();

        IMPORT_C void Set(CIptvVodDlDownloadEvent& aEvent);
        
    private:

        /**
        * C++ default constructor.
        */
        CIptvVodDlDownloadEvent();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
                
        /**
        * Identifies the type of the event. Type specifies which member fields contain
        * valid data.
        */
        TEvent iEvent;

        /**
        * ETrue if global file ID is valid and can be used.
        */
        TBool iGlobalFileIdIsValid;
        
        /**
        * File ID in My Videos.
        */
        CIptvMyVideosGlobalFileId* iGlobalFileId;

        /**
        * If ETrue, then iServiceId and iContentId contain valid data.
        */
        TBool iServiceIdAndContentIdAreValid;
                        
        /**
        * Service ID in Service Manager database.
        */
        TUint32 iServiceId;
        
        /**
        * Content ID in EPG database.
        */
        TUint32 iContentId;

        /**
        * Content index in EPG database.
        */
        TUint32 iContentIndex;
        
        /**
        * Download progress in percentages.
        */
        TUint8 iProgress;
        
        /**
        * Download state, see possible states from CIptvUtil.h
        */        
        TUint32 iState;
        
        /**
        * VOD DL specific error code is stored here.
        */        
        TIptvDlError iError;

        /**
        * Global error code is stored here. This variable is used
        * when functions leave or return error value and they are
        * not directly related to download, e.g memory runs out etc.
        * If iGlobalError != KErrNone then iError is EIptvDlGeneral. 
        */        
        TInt iGlobalError;
        
    };

#endif      // CIPTVVODDLEVENT_H   
            
// End of File
