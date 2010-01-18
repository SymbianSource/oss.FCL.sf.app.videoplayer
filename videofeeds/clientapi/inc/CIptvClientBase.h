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
* Description:    Base class of iptv client objects*
*/




#ifndef CIPTVCLIENTBASE_H
#define CIPTVCLIENTBASE_H

#include <e32base.h>

#include "RIptvClientSession.h"

class MIptvStreamObject;
class MIptvEvent;

/**
 * Iptv client objects base class.
 *
 * @lib IptvEngineClient.dll
 */
class CIptvClientBase : public CActive
    {

public:

    /**
     * Destructor.
     */
    virtual ~CIptvClientBase();

    /**
     * Sends message to IPTV Engine server synchronously.
     * @param aMsgId ID of the message to send.
     * @param aReqStreamObject Object to send.
     * @param aRespStreamObject Response object from server.
     * @param aIpcMsgSize if 0, aReqStreamObject.CountExtrenalizeSize() is used
     *                   to allocate memory for ipc msg.
     * @return TInt Set by server.
     */
    TInt SendRequestL(
        TUint16 aMsgId,
        MIptvStreamObject& aReqStreamObject,
        MIptvStreamObject& aRespStreamObject,
        TInt aIpcMsgSize );

    /**
     * Sends message to IPTV Engine server synchronously.
     * @param aMsgId ID of the message to send.
     * @param aRespStreamObject Response object from server.
     * @param aIpcMsgSize ipc message size to allocated.
     * @return TInt Set by server.
     */
    TInt SendRequestL(
        TUint16 aMsgId,
        MIptvStreamObject& aRespStreamObject,
        TInt aIpcMsgSize );

    /**
     * Sends message to IPTV Engine server synchronously.
     * @param aMsgId ID of the message to send.
     * @param aReqStreamObject Object to send.
     * @return TInt Set by server.
     */
    TInt SendRequestL(
        TUint16 aMsgId,
        MIptvStreamObject& aReqStreamObject );

    /**
     * Send request to server.
     *
     * @param aMessage Message to send.
     * @param aUintParam   Message parameter, TUint32.
     * @return KErrNone or one of system wide error codes.
     */
    TInt SendRequestL(
        TInt aMessage,
        const TDesC& aMsg );

    /**
     * From MIptvEventListenerObserver.
     * @param aEvent
     */
    virtual void HandleEvent( MIptvEvent& aEvent ) = 0;

protected: //called from derived classes constructor

    /**
     * C++ default constructor.
     */
    CIptvClientBase();

private:

    /**
     * From CActive, handles an active object’s request completion event
     */
    void RunL();

    /**
     * From CActive, implements cancellation of an outstanding request.
     * This function is called as part of the active object’s Cancel().
     */
    void DoCancel();

public:     // Data

    RIptvClientSession        iSession;

    };

#endif // CIPTVCLIENTBASE_H
