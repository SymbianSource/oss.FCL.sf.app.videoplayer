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




#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <eiknotapi.h>
#include "IptvDebug.h"
#include "vcxnotifier.h"

const TInt KnbrOfSubjects = 1;

// -----------------------------------------------------------------------------
// CleanupArray
// 
// -----------------------------------------------------------------------------
//  
void CleanupArray(TAny* aArray)
    {
    IPTVLOGSTRING_LOW_LEVEL("VCxNotifierPlugin::CleanupArray");

    CArrayPtrFlat<MEikSrvNotifierBase2>* subjects = 
            static_cast<CArrayPtrFlat<MEikSrvNotifierBase2>*>(aArray);
    if (subjects)
        {
        TInt lastInd = subjects->Count()-1;
        for (TInt i = lastInd; i >= 0; i--)
            {        
            subjects->At(i)->Release();	    	    
            }
        }
    delete subjects;
    }

// -----------------------------------------------------------------------------
// DoCreateNotifierArrayL
// 
// -----------------------------------------------------------------------------
//  
CArrayPtr<MEikSrvNotifierBase2>* DoCreateNotifierArrayL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodNotifierPlugin::DoCreateNotifierArrayL");

    CArrayPtrFlat<MEikSrvNotifierBase2>* subjects = 
                new (ELeave)CArrayPtrFlat<MEikSrvNotifierBase2>( KnbrOfSubjects );    
                
    CleanupStack::PushL( TCleanupItem( CleanupArray, subjects ) );
    
    MEikSrvNotifierBase2* master = CVcXNotifier::NewL();	   
    CleanupStack::PushL( master );
    subjects->AppendL( master );
    CleanupStack::Pop( master );

    CleanupStack::Pop( subjects );       
    return (subjects);
    }

// -----------------------------------------------------------------------------
// NotifierArray
// 
// -----------------------------------------------------------------------------
//  
CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodNotifierPlugin::NotifierArray");

    CArrayPtr<MEikSrvNotifierBase2>* array = 0;
    TRAPD(error, array = DoCreateNotifierArrayL() );
    if (error != KErrNone)
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvVodNotifierPlugin::NotifierArray --- DoCreateNotifierArrayL error %d", error);
        }            
    
    return array;
    }

const TImplementationProxy ImplementationTable[] =
	{    
#ifdef __EABI__    
    {{0x20016BA5},(TFuncPtr)NotifierArray}
#else
    {{0x20016BA5},NotifierArray}
#endif
	};

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// 
// -----------------------------------------------------------------------------
// 
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodNotifierPlugin::ImplementationGroupProxy");

	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
	return ImplementationTable;
	}
