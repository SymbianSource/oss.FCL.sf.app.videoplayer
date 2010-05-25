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
* Description:  Query base class
*
*/


#include "mdequery.h"

#include "mdeorderrule.h"
#include "mdeinstanceitem.h"
#include "mdelogiccondition.h"
#include "mdenamespacedef.h"


CMdEQuery::CMdEQuery(TQueryType aType, CMdESession& aSession, CMdENamespaceDef& aNamespaceDef)
        : iQueryId( 0 ), iSession(aSession), iNamespaceDef(aNamespaceDef), iType(aType), 
          iResultMode(EQueryResultModeItem), iState(EStateNew), iError(KErrNone), 
          iObjectResult(NULL), iCountResult(0), iDistinctResults(NULL), iDestroyed(EFalse)
    {
    }


CMdEQuery::CMdEQuery()
        : iQueryId( 0 ), iSession(*(CMdESession*)0), iNamespaceDef(*(CMdENamespaceDef*)0), iType((TQueryType)0), 
          iResultMode(EQueryResultModeItem), iState(EStateNew), iError(KErrNone), 
          iObjectResult(NULL), iCountResult(0), iDistinctResults(NULL), iDestroyed(EFalse)
    {
    }

void CMdEQuery::QueryConstructL()
    {
    }


CMdEQuery::~CMdEQuery()
	{
	}


EXPORT_C TQueryType CMdEQuery::Type() const
    {
    }

EXPORT_C CMdENamespaceDef& CMdEQuery::NamespaceDef() const
	{
	}
    
EXPORT_C CMdESession& CMdEQuery::Session() const
    {
    }


EXPORT_C CMdELogicCondition& CMdEQuery::Conditions() const
    {
    }
    
    
EXPORT_C void CMdEQuery::AppendOrderRuleL(const TMdEOrderRule& aRule)
    {
    }


EXPORT_C void CMdEQuery::InsertOrderRuleL(const TMdEOrderRule& aRule, TInt aPos)
    {
    }


EXPORT_C TInt CMdEQuery::OrderRuleCount() const
    {
    }


EXPORT_C void CMdEQuery::RemoveOrderRule(TInt aIndex)
    {
    }


EXPORT_C TMdEOrderRule CMdEQuery::OrderRule(TInt aIndex) const
    {   
    }
    

EXPORT_C void CMdEQuery::FindL(TUint aMaxCount, TUint aNotifyCount)
    {
    }


EXPORT_C void CMdEQuery::Cancel()
    {
    }


EXPORT_C TBool CMdEQuery::IsComplete() const
    {
    }


EXPORT_C TInt CMdEQuery::Error() const
    {
    }


EXPORT_C TInt CMdEQuery::Count() const
    {
    }

EXPORT_C CMdEItem& CMdEQuery::ResultItem(TInt aIndex) const
    {
    }


EXPORT_C TItemId CMdEQuery::ResultId(TInt aIndex) const
    {
    }

EXPORT_C const RArray<TItemId>& CMdEQuery::ResultIds() const
	{
	}

EXPORT_C CMdEItem* CMdEQuery::TakeOwnershipOfResult(TInt aIndex)
    {
    }


EXPORT_C TBool CMdEQuery::OwnsResult(TInt aIndex)
    {
    }


EXPORT_C CMdEQuery::TState CMdEQuery::State() const
    {
    }

void CMdEQuery::DoCancel()
    {
    }

void CMdEQuery::SetState(TState aState)
    {
    }


void CMdEQuery::AssertInState(TState aState)
    {
    }
    

void CMdEQuery::AssertNotInState(TState aState)
    {
    }
    

void CMdEQuery::NotifyNewResultsL(const RPointerArray<CMdEInstanceItem>& aNewResults)
    {
    }


void CMdEQuery::NotifyNewResultsL(const RArray<TItemId>& aNewResults)
    {
    }

void CMdEQuery::NotifyNewResultsL( const CDesCArray& aNewResults )
    {
    }

void CMdEQuery::NotifyNewResults(TUint32 aResult)
	{

    }

void CMdEQuery::AppendResultsL(const RPointerArray<CMdEInstanceItem>& aNewResults)
    {
    }

void CMdEQuery::AppendResultsL(const RArray<TItemId>& aNewResults)
    {
    }

void CMdEQuery::AppendResultsL( const CDesCArray& aNewResults )
    {
    }
    
void CMdEQuery::NotifyCompleted(TInt aError)
    {
    }

EXPORT_C void CMdEQuery::AddObserverL(MMdEQueryObserver& aObserver)
	{
	}

EXPORT_C void CMdEQuery::RemoveObserver(MMdEQueryObserver& aObserver)
	{
	}

EXPORT_C void CMdEQuery::SetResultMode( TQueryResultMode aMode )
    {    
    }

EXPORT_C TQueryResultMode CMdEQuery::ResultMode() const
    {
    }
EXPORT_C CMdEItem& CMdEQuery::ResultObjectItem() const
    {
    }

EXPORT_C TPtrC16 CMdEQuery::ResultDistinctValue(TInt aIndex) const
	{
	}

RArray<TMdEOrderRule>& CMdEQuery::OrderRules()
	{
	}
