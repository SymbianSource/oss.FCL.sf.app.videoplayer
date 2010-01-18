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
* Description:   ?Description*
*/


// INCLUDE FILES
#include "VCXMyVideosTestTransactions.h"
#include "VCXTestLog.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::NewL
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestTransactions* CVCXMyVideosTestTransactions::NewL()
    {
    VCXLOGLO1(">>>CVCXMyVideosTestTransactions::NewL");
    CVCXMyVideosTestTransactions* self = new (ELeave) CVCXMyVideosTestTransactions();
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CVCXMyVideosTestTransactions::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestTransactions::CVCXMyVideosTestTransactions
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestTransactions::CVCXMyVideosTestTransactions()
    {
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestTransactions::~CVCXMyVideosTestTransactions
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestTransactions::~CVCXMyVideosTestTransactions( )
    {
        
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestTransactions::ConstructL( )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestTransactions::ConstructL");
    
    VCXLOGLO1("<<<CVCXMyVideosTestTransactions::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::TransactionId
// -----------------------------------------------------------------------------
//
TUint32 CVCXMyVideosTestTransactions::TransactionId( )
    {
    return iTransactionId;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::NextTransactionId
// -----------------------------------------------------------------------------
//
TUint32 CVCXMyVideosTestTransactions::NextTransactionId()
    {
    return ++iTransactionId;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::AddTransactionId
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestTransactions::AddTransactionId( TUint32 aCommandId )
    {
    VCXLOGLO3("CVCXMyVideosTestTransactions::AddTransactionId: add transactionId: %d, commandId: %d", iTransactionId, aCommandId);
    iTransactionIds.Append( iTransactionId );
    iCommandIds.Append( aCommandId );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::TransactionResponse
// -----------------------------------------------------------------------------
//
TBool CVCXMyVideosTestTransactions::TransactionResponse( TUint32 aTransactionId )
    {
    VCXLOGLO2("CVCXMyVideosTestTransactions::TransactionResponse: %d", aTransactionId );
    
    for( TInt i = iTransactionIds.Count() - 1; i >= 0; i-- )
        {
        if( iTransactionIds[i] == aTransactionId )
            {
            iTransactionIds.Remove( i );
            iCommandIds.Remove( i );
            VCXLOGLO1("CVCXMyVideosTestTransactions::TransactionResponse: found transaction.");
            return ETrue;
            }
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::CheckTransactions
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosTestTransactions::CheckTransactions( )
    {
    for( TInt i = 0; i < iTransactionIds.Count(); i++ )
        {
        VCXLOGLO3("CVCXMyVideosTestTransactions::CheckTransactions: transactionId: %d, commandId: %d", iTransactionIds[i], iCommandIds[i]);
        }
    
    if( iTransactionIds.Count() > 0 )
        {
        return KErrCorrupt;
        }
    return KErrNone;
    }

//  End of File
