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



#ifndef __MIPTVEVENTLISTENEROBSERVER_H__
#define __MIPTVEVENTLISTENEROBSERVER_H__

class MIptvEvent;

// CLASS DECLARATION

/**
* MIptvEventListenerObserver
* class specifies the interface for CIptvEventListener callback function.
* Client must inherit this and implement the virtual functions.
*/
class MIptvEventListenerObserver
    {
        
    public: // New functions

    /**
    * Event receiving function, CIptvEventListener is the sender.
    * NOTE: It is not allowed to delete CIptvEventListener object from this callback
    * function. Do it from other event (for example user initiated event or timer event).
    *
    * @param aEvent Event received.
    */
    virtual void HandleEvent( MIptvEvent& aEvent ) = 0;  
    };

#endif
