/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  stub central repository -class for testing CVideoCollectionClient
* 
*/


#include "centralrepository.h"

const int KVideoCollectionViewCenrepSortingRoleKey(0x5);
const int KVideoCollectionViewCenrepSortingOrderKey(0x6);

/**
 * if true, Newl will leave
 */
bool gCRNewLLeave = false;

/**
 * if zero, Set will fail. Every Set call decreases this value by one.
 */
int gCRSetFail = 255;

/**
 * if zero, Get will fail. Every Get call decreases this value by one.
 */
int gCRGetFail = 255;

/**
 * order value will be saved here in correct cases
 */
TBuf<255> gSettedTDesValue = TBuf<255>();


QMap<uint, int> CRepository::mSortValues = QMap<uint, int>();


// -----------------------------------------------------------------------------
// setNewLLeave
// -----------------------------------------------------------------------------
//
void CRepository::setNewLLeave(bool leave)
{
    gCRNewLLeave = leave;
}

// -----------------------------------------------------------------------------
// setSetFail
// -----------------------------------------------------------------------------
//
void CRepository::setSetFail(int amount)
{
    gCRSetFail = amount;
}
    
// -----------------------------------------------------------------------------
// setGetFail
// -----------------------------------------------------------------------------
//
void CRepository::setGetFail(int amount)
{
    gCRGetFail = amount;
}

// -----------------------------------------------------------------------------
// getTDesValue
// -----------------------------------------------------------------------------
//
TDesC& CRepository::getTDesValue()
{
    return gSettedTDesValue;
}

// -----------------------------------------------------------------------------
// setOrderValue
// -----------------------------------------------------------------------------
//
void CRepository::setTDesValue(const TDesC& value)
{
    gSettedTDesValue = value;
}

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CRepository* CRepository::NewL(TUid /*aRepositoryUid*/)
{
    if(gCRNewLLeave)
    {
        User::Leave(KErrGeneral);  
    }
    return new CRepository();
}

// -----------------------------------------------------------------------------
// CRepository
// -----------------------------------------------------------------------------
//
CRepository::CRepository()
{
    
}

// -----------------------------------------------------------------------------
// ~CRepository
// -----------------------------------------------------------------------------
//
CRepository::~CRepository()
{
    
}

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------
//
TInt CRepository::Get(TUint32 aKey, TInt& aValue)
{
    if(gCRGetFail == 0)
    {
        return KErrGeneral;
    } else {
        gCRGetFail--;
    }
    
    aValue = mSortValues.value(aKey);
   
    return KErrNone;
}

// -----------------------------------------------------------------------------
// Set
// -----------------------------------------------------------------------------
//
TInt CRepository::Set(TUint32 aKey, TInt aValue)
{
    if(gCRSetFail == 0)
    {
        return KErrGeneral;
    } else {
        gCRSetFail--;
    }
    
    mSortValues[aKey] = aValue;
    
    return KErrNone;
}

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------
//
TInt CRepository::Get(TUint32 /*aKey*/, TDes& aValue)
{
    if(gCRGetFail == 0)
    {
        return KErrGeneral;
    } else {
        gCRGetFail--;
    }
    
    aValue = gSettedTDesValue;
    
    return KErrNone;
}
