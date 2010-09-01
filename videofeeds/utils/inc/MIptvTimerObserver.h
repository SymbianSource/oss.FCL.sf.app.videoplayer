/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __MIPTVTIMEROBSERVER_H__
#define __MIPTVTIMEROBSERVER_H__

class CIptvTimer;

// CLASS DECLARATION
/**
*  This class specifies the function to be called when a timeout occurs.
*  Used in conjunction with CIptvTimer class.
*/
class MIptvTimerObserver
    {
    public: // New functions

        /**
        * The function to be called when a timeout occurs.
        *
        * @param aTimer Pointer to timer which expired.
        */
        virtual void TimerExpired(CIptvTimer* aTimer) = 0;
    };

#endif // __MIPTVTIMEROBSERVER_H__

// End of File