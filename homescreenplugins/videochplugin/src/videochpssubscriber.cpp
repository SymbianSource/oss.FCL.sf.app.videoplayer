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
* Description:    Class to handle subscriptions from PS*
*/



#include "videochpssubscriber.h"


// -----------------------------------------------------------------------------
// CVcxNsCHPSSubscriber::CVcxConnUtilSubscriber()
// -----------------------------------------------------------------------------
//
CVcxNsCHPSSubscriber::CVcxNsCHPSSubscriber( const TUid aUid, 
                                            const TUint32 aKey, 
                                            RProperty::TType aType,
                                            MCHPSObserver* aObserver ) :
    CActive( EPriorityStandard ), 
    iUid( aUid ),
    iKey( aKey ), 
    iKeyType(aType),
    iObserver( aObserver )
    {
        // NOP
    }
 
// -----------------------------------------------------------------------------
// CVcxNsCHPSSubscriber::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsCHPSSubscriber* CVcxNsCHPSSubscriber::NewL( const TUid aUid, 
                                                  const TUint32 aKey,
                                                  RProperty::TType aType, 
                                                  MCHPSObserver* aObserver )
    {
    if( aType != RProperty::EInt )
        {
        User::Leave( KErrNotSupported );
        }
    CVcxNsCHPSSubscriber* self =
                      new( ELeave ) CVcxNsCHPSSubscriber( aUid, aKey, aType, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
 
// -----------------------------------------------------------------------------
// CVcxNsCHPSSubscriber::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxNsCHPSSubscriber::ConstructL()
    {
    iInitialized = EFalse;
    User::LeaveIfError( iProperty.Attach( iUid, iKey ) );
    CActiveScheduler::Add( this );    
    }
 
// -----------------------------------------------------------------------------
// CVcxNsCHPSSubscriber::~CVcxNsCHPSSubscriber()
// -----------------------------------------------------------------------------
//
CVcxNsCHPSSubscriber::~CVcxNsCHPSSubscriber()
    {
    if( IsActive() )
        {
        Cancel();
        }
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CVcxNsCHPSSubscriber::Get()
// -----------------------------------------------------------------------------
//
TInt CVcxNsCHPSSubscriber::Get( TInt& aValue )
    {
    return iProperty.Get( aValue );
    }


// -----------------------------------------------------------------------------
// CVcxNsCHPSSubscriber::Start()
// -----------------------------------------------------------------------------
//
void CVcxNsCHPSSubscriber::Start()
    {
    if( !IsActive() )
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        iInitialized = ETrue;
        }
    }


// -----------------------------------------------------------------------------
// CVcxNsCHPSSubscriber::DoCancel()
// -----------------------------------------------------------------------------
//
void CVcxNsCHPSSubscriber::DoCancel()
    {
    if( IsActive() )
        {
        iProperty.Cancel();
        }

    iInitialized = EFalse;
    }
 
// -----------------------------------------------------------------------------
// CVcxNsCHPSSubscriber::RunL()
// -----------------------------------------------------------------------------
//
void CVcxNsCHPSSubscriber::RunL()
    {
    // resubscribe before processing new 
    // value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();

    if( iInitialized )
        {
        TInt intValue;
    
        if(iKeyType == RProperty::EInt )
            {
            if( iProperty.Get( intValue ) == KErrNone && iObserver )
                {
                iObserver->ValueChanged( iKey, intValue );
                }
            }       
        }
    iInitialized = ETrue;
  }
// end of file
