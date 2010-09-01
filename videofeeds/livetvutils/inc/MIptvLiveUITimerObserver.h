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
* Description:    Live TV timer observer interface.*
*/




#ifndef __MIPTV_LIVE_UI_TIMEROBSERVER_H__
#define __MIPTV_LIVE_UI_TIMEROBSERVER_H__

// INCLUDES
// FORWARD DECLARATIONS
struct TTimerObject;

// CLASS DECLARATION
/**
*  Live TV timer observer interface class. Normal timer fires (KErrNone from 
*  timer) is notified through TimerFires. TimerError is called when
*  error occurs in timer. Error codes come from RTimer::At():
*  KErrCancel, the timer was cancelled; KErrAbort, the timer was aborted 
*  because the system time changed; KErrUnderflow, the requested completion 
*  time is in the past; KErrOverFlow, the requested completion time is too 
*  far in the future.
*/
class MIptvLiveUITimerObserver
    {        
    public:      // New functions
     
        /**
        * Timer fired successfully.
        */
        virtual void TimerFires() = 0;
        
        /**
        * Error occured in the timer.
        * @param aError KErrNone if timer fired ok, KErrCancel, the timer was
		* cancelled; KErrAbort, the timer was aborted because the system time
		* changed; KErrUnderflow, the requested completion time is in the past;
		* KErrOverFlow, the requested completion time is too far in the future.
        */
        virtual void TimerError( const TInt aError ) = 0;
        
    };

#endif       // __MIPTV_LIVE_UI_TIMEROBSERVER_H__

// End of File
