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
* Description: 
*
*/



#ifndef __MIPTVVODDLCLIENTOBSERVER_H__
#define __MIPTVVODDLCLIENTOBSERVER_H__

#include "CIptvMyVideosGlobalFileId.h"

class CIptvVodDlDownloadEvent;

// CLASS DECLARATION

/**
* MIptvVodDlClientObserver
* class specifies the interface for CIptvVodDlClient callback function.
* Client must inherit this and implement the virtual functions.
*/
class MIptvVodDlClientObserver
    {
        
    public: // New functions

    /**
    * NOTE: It is not allowed to delete CIptvVodDlClient object from this callback
    * function. Do it from other event (for example user initiated event or timer event).
    */
    virtual void HandleVodDlDownloadEvent(CIptvVodDlDownloadEvent& aEvent) = 0;  
    };

#endif
