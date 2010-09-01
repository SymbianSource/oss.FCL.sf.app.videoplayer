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
 * Description:  This file contains testclass implementation.*
*/


// [INCLUDE FILES]

#include "VcxTestControlEventQueue.h"
#include <STIFLogger.h>
#include "VCXTestLog.h"
#include <EIKENV.H>

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CVcxTestControlEvent::NewL
// ----------------------------------------------------------------------------
//
CVcxTestControlEvent* CVcxTestControlEvent::NewL()
    {
    CVcxTestControlEvent* self = new ( ELeave ) CVcxTestControlEvent();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEvent::~CVcxTestControlEvent
// ----------------------------------------------------------------------------
//
CVcxTestControlEvent::~CVcxTestControlEvent()
    {
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEvent::CVcxTestControlEvent
// ----------------------------------------------------------------------------
//
CVcxTestControlEvent::CVcxTestControlEvent()
    {
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEvent::CVcxTestControlEvent
// ----------------------------------------------------------------------------
//
void CVcxTestControlEvent::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEvent::CVcxTestControlEvent
// ----------------------------------------------------------------------------
//
void CVcxTestControlEvent::SetPointerEvent( TInt aDelay, TPointerEvent::TType aType, TPoint aPos )
    {
    iKeyCode = EStdKeyNull;
    iPointerEventType = aType;
    iPosition = aPos;
    iDelay = aDelay;
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEvent::CVcxTestControlEvent
// ----------------------------------------------------------------------------
//
void CVcxTestControlEvent::SetKeyEvent( TInt aDelay, TStdScanCode aKeyCode, TRawEvent::TType aType )
    {
    iDelay = aDelay;
    iKeyCode = aKeyCode;
    iKeyEventType = aType;
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEventQueueQueue::NewL
// ----------------------------------------------------------------------------
//
CVcxTestControlEventQueue* CVcxTestControlEventQueue::NewL( CStifLogger* aLog )
    {
    VCXLOGLO1(">>>CVcxTestControlEventQueue::NewL");
    CVcxTestControlEventQueue* self = new ( ELeave ) CVcxTestControlEventQueue();
    CleanupStack::PushL(self);
    self->ConstructL( aLog );
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CVcxTestControlEventQueue::NewL");
    return self;
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEventQueue::~CVcxTestControlEventQueue
// ----------------------------------------------------------------------------
//
CVcxTestControlEventQueue::~CVcxTestControlEventQueue()
    {
    VCXLOGLO1(">>>CVcxTestControlEventQueue::~CVcxTestControlEventQueue");
    
    delete iTimer;
    iTimer = NULL;
    
    iWsSession.Close();
    
    iEvents.ResetAndDestroy();
    VCXLOGLO1("<<<CVcxTestControlEventQueue::~CVcxTestControlEventQueue");
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEventQueue::CVcxTestControlEventQueue
// ----------------------------------------------------------------------------
//
CVcxTestControlEventQueue::CVcxTestControlEventQueue()
    {

    }

// ----------------------------------------------------------------------------
// CVcxTestControlEventQueue::CVcxTestControlEventQueue
// ----------------------------------------------------------------------------
//
void CVcxTestControlEventQueue::ConstructL( CStifLogger* aLog )
    { 
    VCXLOGLO1(">>>CVcxTestControlEventQueue::ConstructL");
    iTimer = CVcxTestTimer::NewL( *this, 0 );
    User::LeaveIfError( iWsSession.Connect() );
    iLog = aLog;
    VCXLOGLO1("<<<CVcxTestControlEventQueue::ConstructL");
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEventQueue::EnableControlEvents
// ----------------------------------------------------------------------------
//
void CVcxTestControlEventQueue::EnableControlEvents( TInt aDelay )
    {
    VCXLOGLO1(">>>CVcxTestControlEventQueue::EnableControlEvents");
    iTimer->After( aDelay );
    VCXLOGLO1("<<<CVcxTestControlEventQueue::EnableControlEvents");
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEventQueue::DisableControlEvents
// ----------------------------------------------------------------------------
//
void CVcxTestControlEventQueue::DisableControlEvents()
    {
    VCXLOGLO1(">>>CVcxTestControlEventQueue::DisableControlEvents");
    iTimer->Cancel();
    VCXLOGLO1("<<<CVcxTestControlEventQueue::DisableControlEvents");
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEventQueue::AddControlEvent
// ----------------------------------------------------------------------------
//
void CVcxTestControlEventQueue::AddControlEvent( CVcxTestControlEvent* aEvent )
    {
    VCXLOGLO1(">>>CVcxTestControlEventQueue::AddControlEvent");
    iEvents.Append( aEvent );
    VCXLOGLO1("<<<CVcxTestControlEventQueue::AddControlEvent");
    }

// ----------------------------------------------------------------------------
// CVcxTestControlEventQueue::ResetControlEvents
// ----------------------------------------------------------------------------
//
void CVcxTestControlEventQueue::ResetControlEvents()
    {
    VCXLOGLO1(">>>CVcxTestControlEventQueue::TimerComplete");
    iEvents.ResetAndDestroy();
    VCXLOGLO1("<<<CVcxTestControlEventQueue::TimerComplete");
    }

// -----------------------------------------------------------------------------
//  CVcxTestControlEventQueue::TimerComplete
// -----------------------------------------------------------------------------
void CVcxTestControlEventQueue::TimerComplete( TInt aTimerId, TInt /* aError */ )
    {
    VCXLOGLO1(">>>CVcxTestControlEventQueue::TimerComplete");
    if( aTimerId == 0 )
        {
        if( iEvents.Count() > 0 )
            {
            CVcxTestControlEvent* event = iEvents[0];
            iEvents.Remove( 0 );

            if( event->KeyCode() == EStdKeyNull )
                {
                TapOnScreen( event->PointerEventType(), event->Position() );
                }
            else
                {
                SendKeyEvent( event->KeyCode(), event->KeyEventType() );
                }
            
            iTimer->After( event->Delay() );            
            delete event;
            }
        }
    VCXLOGLO1("<<<CVcxTestControlEventQueue::TimerComplete");
    }

// -------------------------------------------------------------------------------------------------
// CVcxTestControlEventQueue::TapOnScreen
// -------------------------------------------------------------------------------------------------
void CVcxTestControlEventQueue::TapOnScreen( TPointerEvent::TType aType,
        TPoint aPosition )
    {
    VCXLOGLO1(">>>CVcxTestControlEventQueue::TapOnScreen");
    
    TRawEvent pointer;
    
    VCXLOGLO4("CVcxTestControlEventQueue::X: %d Y: %d, type: %d", aPosition.iX, aPosition.iY, aType);
    
    if (aType == TPointerEvent::EButton1Down)
        {
        pointer.Set( TRawEvent::EButton1Down, aPosition.iX, aPosition.iY );

        iWsSession.SimulateRawEvent( pointer );
        iWsSession.Flush();
        }
    else
        {
        if (aType == TPointerEvent::EButton1Up)
            {
            pointer.Set( TRawEvent::EButton1Up, aPosition.iX, aPosition.iY );

            iWsSession.SimulateRawEvent( pointer );
            iWsSession.Flush();
            }
        }
    
    VCXLOGLO1("<<<CVcxTestControlEventQueue::TapOnScreen");
    }

// -------------------------------------------------------------------------------------------------
// CVcxTestControlEventQueue::SendKeyEvent
// -------------------------------------------------------------------------------------------------
void CVcxTestControlEventQueue::SendKeyEvent( TStdScanCode aKeyCode, TRawEvent::TType aType )
    {
    VCXLOGLO1(">>>CVcxTestControlEventQueue::SendKeyEvent");
    
    TRawEvent rawEvent;
    rawEvent.Set( aType, aKeyCode );
    iWsSession.SimulateRawEvent( rawEvent );
    iWsSession.Flush();
    
    VCXLOGLO1("<<<CVcxTestControlEventQueue::SendKeyEvent");
    }
