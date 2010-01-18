/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  STIF testclass declaration*
*/


#ifndef VCXTESTCONTROLEVENTQUEUE_H
#define VCXTESTCONTROLEVENTQUEUE_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h>
#include <e32keys.h>
#include <e32event.h>
#include "VcxTestTimer.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CStifLogger;

// CLASS DECLARATION

class CVcxTestControlEvent
    {
    public: // New functions
        
        /**
         * Two-phased constructor.
         * @param aDelay delay before next event
         * @param aType type of pointer event
         * @param aPos position of event
         */
        static CVcxTestControlEvent* NewL();
    
        /**
         * Destructor.
         */
        virtual ~CVcxTestControlEvent();
   
        /**
         * C++ default constructor.
         */
        CVcxTestControlEvent();
    
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
        
        /**
         * Sets this object as a pointer event. 
         */
        void SetPointerEvent( TInt aDelay, TPointerEvent::TType aType, TPoint aPos );
        
        /**
         * Sets this object as a key event. 
         */
        void SetKeyEvent( TInt aDelay, TStdScanCode aKeyCode, TRawEvent::TType aType );
        
        TPoint Position() { return iPosition; }
        
        TPointerEvent::TType PointerEventType() { return iPointerEventType; }
        
        TInt Delay() { return iDelay; }
        
        TStdScanCode KeyCode() { return iKeyCode; }
        
        TRawEvent::TType KeyEventType() { return iKeyEventType; }
        
    private: // Data
        
        // If keycode is EStdKeyNull then this is a pointer event.
        TStdScanCode iKeyCode;
        TRawEvent::TType iKeyEventType;
        
        TPointerEvent::TType iPointerEventType;
        TPoint iPosition;
        
        TInt iDelay;
    };

class CVcxTestControlEventQueue : public CBase, 
                                  public MVcxTestTimerObserver
    {
    public: // Constructors and destructor
    
        /**
         * Two-phased constructor.
         */
        static CVcxTestControlEventQueue* NewL( CStifLogger* aLog );
    
        /**
         * Destructor.
         */
        virtual ~CVcxTestControlEventQueue();
    
    private: // New functions
    
        /**
         * C++ default constructor.
         */
        CVcxTestControlEventQueue();
    
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( CStifLogger* aLog );

        void TapOnScreen( TPointerEvent::TType aType, TPoint aPosition );
        
        void SendKeyEvent( TStdScanCode aKeyCode, TRawEvent::TType aType );
    
    public: // New functions
        
        void EnableControlEvents( TInt aDelay );
        
        void DisableControlEvents();
        
        void AddControlEvent( CVcxTestControlEvent* aEvent );
        
        void ResetControlEvents();
        
    public: // Functions from base classes
        
        /**
         * From MVcxTestTimerObserver.
         */
        void TimerComplete( TInt aTimerId, TInt aError );
    
    private: // Data
        CVcxTestTimer* iTimer;
        RWsSession iWsSession;
        RPointerArray<CVcxTestControlEvent> iEvents;
        CStifLogger* iLog;
    };

#endif /* VCXTESTCONTROLEVENTQUEUE_H */
