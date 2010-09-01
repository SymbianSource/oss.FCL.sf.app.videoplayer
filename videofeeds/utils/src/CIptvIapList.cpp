/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for handling group of IAPs.*
*/




// INCLUDE FILES
#include "CIptvIapList.h"
#include "CIptvUtil.h"
#include "IptvDebug.h"

// ============================ MEMBER FUNCTIONS ===============================

//defined to move default constructor to private
CIptvIapList::CIptvIapList()
    {
    }

// -----------------------------------------------------------------------------
// CIptvIapList::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvIapList* CIptvIapList::NewL()
    {
    CIptvIapList* self = new (ELeave) CIptvIapList;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvIapList::ConstructL
// -----------------------------------------------------------------------------
//
void CIptvIapList::ConstructL()
    {
    iIapList.Reset();
    }

// -----------------------------------------------------------------------------
// CIptvIapList::~CIptvIapList
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvIapList::~CIptvIapList()
    {
    iIapList.Close();
    }

// -----------------------------------------------------------------------------
// CIptvIapList::AddIap
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvIapList::AddIap(TIptvIap& aIap)
    {
    if ( iIapList.Count() >= KIptvSmServicesDbMaxIaps )
        {
        return KErrOverflow;
        }
    return iIapList.Append( aIap );
    }

// -----------------------------------------------------------------------------
// CIptvIapList::AddIap
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvIapList::AddIap( TIptvIap& aIap, TBool aIgnoreMax )
    {
    if ( iIapList.Count() >= KIptvSmServicesDbMaxIaps && !aIgnoreMax )
        {
        return KErrOverflow;
        }
    return iIapList.Append( aIap );
    }

// -----------------------------------------------------------------------------
// CIptvIapList::DeleteIap
// -----------------------------------------------------------------------------
//    
EXPORT_C TInt CIptvIapList::DeleteIap( TUint8 aIndex )
    {
    if ( aIndex >= iIapList.Count() )
        {
        return KErrOverflow;
        }
        
    iIapList.Remove( aIndex );
    iIapList.Compress();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvIapList::DeleteIap
// -----------------------------------------------------------------------------
//    
EXPORT_C TInt CIptvIapList::DeleteIap( TIptvIap& aIap )
    {
    TInt i;
    for ( i = 0; i < iIapList.Count(); i++ )
        {
        if ( aIap.iId == iIapList[i].iId )
            {
            iIapList.Remove( i );
            iIapList.Compress();
            return KErrNone;
            }
        }
             
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CIptvIapList::ModifyIap
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvIapList::ModifyIap(TUint8 aIndex, TIptvIap& aIap)
    {
    if(aIndex > (iIapList.Count()-1))
        {
        return KErrOverflow;
        }
    iIapList.Remove(aIndex);
    iIapList.Insert(aIap, aIndex);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvIapList::IapL
// -----------------------------------------------------------------------------
//
EXPORT_C TIptvIap& CIptvIapList::IapL( TUint8 aIndex )
    {
    if ( aIndex > (iIapList.Count()-1) )
        {
        User::Leave( KErrOverflow );
        }
    return iIapList[aIndex];
    }

// -----------------------------------------------------------------------------
// CIptvIapList::GetIap
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvIapList::GetIap( TUint8 aIndex, TIptvIap& aIap )
    {
    if ( aIndex > (iIapList.Count()-1) )
        {
        return KErrOverflow;
        }

    aIap.iId       = iIapList[aIndex].iId;
    aIap.iPriority = iIapList[aIndex].iPriority;
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CIptvIapList::Count
// -----------------------------------------------------------------------------
//    
EXPORT_C TUint8 CIptvIapList::Count()
    {
    return static_cast<TUint8>(iIapList.Count());
    }
    
// -----------------------------------------------------------------------------
// CIptvIapList::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvIapList::ExternalizeL(RDesWriteStream& aStream) const
    {
    TUint8 count;
    count = static_cast<TUint8>(iIapList.Count());    
    aStream.WriteUint8L(count); 

    TUint8 i;
    for(i = 0; i < count; i++)
        {
        iIapList[i].ExternalizeL(aStream);
        }    
    }

// -----------------------------------------------------------------------------
// CIptvIapList::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvIapList::InternalizeL(RDesReadStream& aStream)
    {
    TUint8 count;    
    count = aStream.ReadUint8L();

    //delete previous list data
    iIapList.Reset();
    
    //add uninitialized iaps
    TUint8 i;
    TIptvIap iap;
    for(i = 0; i < count; i++)
        {
        iIapList.Append(iap);
        }
    
    //init iaps with stream data
    for(i = 0; i < count; i++)
        {
        iIapList[i].InternalizeL(aStream);
        }
    }

// -----------------------------------------------------------------------------
// CIptvIapList::CountExternalizeSize
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvIapList::CountExternalizeSize()
    {
    TUint8 i;
    TUint32 externalizeSize = 1; //first item is count (1 byte)
    
    for(i = 0; i < iIapList.Count(); i++)
        {
        externalizeSize += iIapList[i].CountExternalizeSize();
        }
    return externalizeSize;
    }
    
// -----------------------------------------------------------------------------
// CIptvIapList::SetL
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvIapList::SetL(CIptvIapList& aIapList)
    {
    HBufC8* iapDes = HBufC8::NewL(aIapList.CountExternalizeSize() +1);
    CleanupStack::PushL(iapDes);
    RDesWriteStream writeStream;
    TPtr8 iapPtr8(iapDes->Des());
    writeStream.Open(iapPtr8);
    aIapList.ExternalizeL(writeStream);
    writeStream.Close();
    RDesReadStream readStream;
    readStream.Open(iapDes->Des());
    InternalizeL(readStream);
    readStream.Close();
    CleanupStack::PopAndDestroy(iapDes);
    }

// -----------------------------------------------------------------------------
// CIptvIapList::SortByPriorityL
// Iaplist can have max of 10 items. This sorting is done as simply as possible
// to avoid errors. Don't copy paste this code to any larger lists.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvIapList::SortByPriorityL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvIapList::SortByPriority()");

    CIptvIapList* iapListSorted;
    iapListSorted = CIptvIapList::NewL();
    CleanupStack::PushL( iapListSorted );
    
    TUint8 highestPriorityIndex;
    TUint8 i;
    TUint8 count;
    
    count = Count();
    
    for ( i = 0; i < count; i++ )
        {
        highestPriorityIndex = GetHighestPriorityIapIndexL();
        iapListSorted->AddIap( iIapList[highestPriorityIndex] );
        DeleteIap( highestPriorityIndex );
        }
    
    SetL(*iapListSorted);
    CleanupStack::PopAndDestroy(iapListSorted);

    IPTVLOGSTRING_LOW_LEVEL("CIptvIapList::SortByPriority() exit");
    }
    
// -----------------------------------------------------------------------------
// CIptvIapList::GetHighestPriorityIapIndexL
// -----------------------------------------------------------------------------
//
TUint8 CIptvIapList::GetHighestPriorityIapIndexL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvIapList::GetHighestPriorityIapIndexL()");

    if ( Count() < 1 )
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvIapList:: Error: No IAPs set");
        User::Leave( KErrNotFound );
        }
    
    //small number = high priority
    TUint8 highestPriorityFound = 255;
    TUint8 highestPriorityIndex = 0;
    TUint16 i;

    for ( i = 0; i < Count(); i++ )
        {
        
        if ( highestPriorityFound > iIapList[i].iPriority )
            {
            highestPriorityFound = iIapList[i].iPriority;  //store current best priority
            highestPriorityIndex = i; //store current best priority location
            }
        }
    
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvIapList::GetHighestPriorityIapIndex() exit");

    //return the index which had the highest priority (= smallest value)
    return highestPriorityIndex;
    }

// -----------------------------------------------------------------------------
// CIptvIapList::FindIap
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvIapList::FindIap( TUint32 aIapId, TUint8& aIndex )
    {
    TInt i;
    for ( i = 0; i < Count(); i++ )
        {
        if( iIapList[i].iId == aIapId )
            {
            aIndex = i;
            return KErrNone;
            } 
        }
    return KErrNotFound;
    }

