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

#ifndef __CENTRALREPOSITORY_H__
#define __CENTRALREPOSITORY_H__

#include <e32base.h>
#include <qmap.h>

class CRepository 
{
public:
    
    /**
     * sets gCRNewLLeave
     */
    static void setNewLLeave(bool leave);
    
    /**
     * sets gCRSetFail
     */
    static void setSetFail(int amount);
    
    /**
     * sets gCRGetFail
     */
    static void setGetFail(int amount);
     
    /**
     * gets gSettedTDesValue;
     */
    static TDesC& getTDesValue();
     
    /**
     * sets gSettedTDesValue;
     */
    static void setTDesValue(const TDesC& value);
    
    
    static QMap<uint, int> mSortValues;

    /**
     * if gCRNewLLeave is true, will leave
     * if gCRNewLLeave is false returns new CRepository
     */
    static CRepository* NewL(TUid aRepositoryUid);

    /**
     * contructor
     */
    CRepository();
    
    /**
     * destructor
     */
    virtual ~CRepository();
    
    /**
     * if gCRGetFail is true, returns KErrGeneral
     * if gCRGetFail is false, returns gSettedValue and returns KErrNone
     */
    TInt Get(TUint32 aKey, TInt& aValue);

    /**
     * if gCRSetFail is true, returns KErrGeneral
     * if gCRSetFail is false, saves value to gSettedValue and returns KErrNone
     */
    TInt Set(TUint32 aKey, TInt aValue);
    
    /**
     * if gCRGetFail is true, returns KErrGeneral
     * if gCRGetFail is false, returns gSettedValue and returns KErrNone
     */
    TInt Get(TUint32 aKey, TDes& aValue);


};

#endif // __CENTRALREPOSITORY_H__
