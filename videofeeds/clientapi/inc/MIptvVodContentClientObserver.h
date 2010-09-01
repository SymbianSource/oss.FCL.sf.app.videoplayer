/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Observer for VoD Content API*
*/




#ifndef MIPTVVODCONTENTOBSERVER_H
#define MIPTVVODCONTENTOBSERVER_H

#include "CIptvUtil.h"

/**
*  Observer for VoD Content API that needs to be implemented by application.
*
*  @lib
*  @since 
*/

class MIptvVodContentClientObserver
    {
    public:                
        /**
        * Messages from EPG manager are handled here. Messages are defined in CIptvUtil.h
        * @since            Engine 1.1
        * @param aMsg       Message 
        * @param aServiceId Service id.
        */
        virtual void HandleEpgManagerMsgL(TInt aMsg, TInt aInfo, TIptvServiceId aServiceId) = 0;       
    };

#endif      // MIPTVVODCONTENTOBSERVER_H
// End of File
