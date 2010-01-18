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
* Description:    Observer for completion of copy or move requests.*
*/




#ifndef CIPTVMYVIDEOSCOPYORMOVEOBSERVER_H
#define CIPTVMYVIDEOSCOPYORMOVEOBSERVER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MIptvMyVideosClientObserver;
class RIptvClientSession;
class CIptvMyVideosGlobalFileId;
class CIptvMyVideosGlobalFolderId;

// CLASS DECLARATION

/**
 * Observer for completion of copy or move requests.
 *
 * @lib IptvClientApi.dll
 */
class CIptvMyVideosCopyOrMoveObserver : public CActive
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @param aClientObserver Observer.
         * @param aSession        Server client session.         
         */
        static CIptvMyVideosCopyOrMoveObserver* NewL(MIptvMyVideosClientObserver& aClientObserver, RIptvClientSession& aSession);
        
        /**
         * Destructor.
         */
        virtual ~CIptvMyVideosCopyOrMoveObserver();

        /**
         * Sends copy/move request to server and sets this object active.
         * Request is completed when server has handled the copy/move operation.
         *
         * @param aCommand        Message to send.
         * @param aSourceFileId   Source file's global file Id.
         * @param aTargetFolderId Target folder's global folder Id.
         */
        void SetActiveL(TUint16 aCommand, CIptvMyVideosGlobalFileId& aSourceFileId, CIptvMyVideosGlobalFolderId& aTargetFolderId);
        
        /**
         * Sends copy/move request to server and sets this object active.
         * Request is completed when server has handled the copy/move operation.         
         *
         * @param aCommand        Message to send.
         * @param aSourceFolderId Source folder's global folder Id.
         * @param aTargetFolderId Target folder's global folder Id.         
         */
        void SetActiveL(TUint16 aCommand, CIptvMyVideosGlobalFolderId& aSourceFolderId, CIptvMyVideosGlobalFolderId& aTargetFolderId);
        
    public: // From CActive
        
        /**
         * Called when request is completed.
         */
        void RunL();
        
        /**
         * Called when request needs to be cancelled.
         */
        void DoCancel();

    protected:

        /**
         * Constructor.
         *
         * @param aClientObserver Observer.
         * @param aSession        Server client session.         
         */
        CIptvMyVideosCopyOrMoveObserver(MIptvMyVideosClientObserver& aClientObserver, RIptvClientSession& aSession);
           
        /**
         * EPOC constructor.
         */
        void ConstructL();

    protected: // Data.

        MIptvMyVideosClientObserver& iClientObserver;
        RIptvClientSession&          iSession;
        HBufC8*                      iMsg;
        TPtr8                        iMsgPtr;        
    };

#endif // CIPTVMYVIDEOSCOPYORMOVEOBSERVER_H

// End of File
