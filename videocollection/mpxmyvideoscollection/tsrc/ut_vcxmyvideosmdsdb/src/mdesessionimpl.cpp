/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Metadata engine client session implementation*
*/

#include <etel3rdparty.h>
#include <e32property.h> 

#include "mdesession.h"
#include "mdesessionimpl.h"


#include "mdeobjectdef.h"
#include "mdeobjectquery.h"
#include "mderelationdef.h"
#include "mdeeventdef.h"
#include "mdeobject.h"
#include "mderelation.h"
#include "mdeevent.h"

#include "mdeobjectdef.h"
#include "mdenamespacedef.h"
#include "mdedatabuffer.h"
#include "mdelogiccondition.h"
#include "mdeobjectcondition.h"

#include "VcxMyVideosMdsDbTest.h"


// for CleanupResetAndDestroyPushL
#include <mmf/common/mmfcontrollerpluginresolver.h>

CMdeSessionStartTimer* CMdeSessionStartTimer::NewL(MMdESessionObserver& aObserver)
    {
    CMdeSessionStartTimer* self = new CMdeSessionStartTimer( aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CMdeSessionStartTimer::ConstructL()
    {
    CTimer::ConstructL();
    }   

CMdeSessionStartTimer::~CMdeSessionStartTimer()
	{
	Cancel();
	}		

void CMdeSessionStartTimer::RunL()
	{
	iObserver.HandleSessionOpened(*(CMdESession*) 0  ,test->mdssessionasyncerror);
	}

CMdeSessionStartTimer::CMdeSessionStartTimer(MMdESessionObserver& aObserver)
	: CTimer( CActive::EPriorityStandard ), iObserver( aObserver )
	{
	CActiveScheduler::Add(this);
	}

CMdESessionImpl::CMdESessionImpl(MMdESessionObserver& aObserver)
	: iSessionStartupAO( NULL ), iSessionObserver(&aObserver), 
	  iSchemaBuffer( NULL ), iNextQueryId( 0 )
	{
    iTimer = CMdeSessionStartTimer::NewL( aObserver );
    
    iTimer->After( 1000 );
	}

CMdESessionImpl::~CMdESessionImpl()
	{
    delete iTimer;
    // No session errors should be sent during deconstruction to avoid possible double deletion
    iSessionObserver = NULL;

	delete iSchemaBuffer;

	iNotifiers.ResetAndDestroy();
	iNotifiers.Close();

	
	iSchemaChunk.Close();
	
	iNamespaceDefs.ResetAndDestroy();
	iNamespaceDefs.Close();
	
	delete iSessionStartupAO;
	}

void CMdESessionImpl::ConstructL()
	{
    }

void CMdESessionImpl::Close()
	{
	}

TInt CMdESessionImpl::NamespaceDefCount() const
	{
	return iNamespaceDefs.Count();
	}

CMdENamespaceDef& CMdESessionImpl::NamespaceDefL( TInt aIndex )
	{
	return *iNamespaceDefs[aIndex];
	}

CMdENamespaceDef& CMdESessionImpl::GetNamespaceDefL( const TDesC& aName )
	{
	const TInt KNamespaceCount = iNamespaceDefs.Count();
	for ( TInt i = 0; i < KNamespaceCount; ++i )
		{
		if ( !aName.Compare( iNamespaceDefs[i]->Name() ) )
			{
			return NamespaceDefL( i );
			}
		}

	User::Leave( KErrNotFound );
	return NamespaceDefL( -1 ); // never reached
	}

CMdENamespaceDef& CMdESessionImpl::GetNamespaceDefL(TDefId aId)
	{
	const TInt KNamespaceCount = iNamespaceDefs.Count();
	for ( TInt i = 0; i < KNamespaceCount; ++i )
		{
		if ( iNamespaceDefs[i]->Id() == aId )
			{
			return NamespaceDefL( i );
			}
		}

	User::Leave( KErrNotFound );
	return NamespaceDefL( -1 ); // never reached
	}

CMdENamespaceDef& CMdESessionImpl::GetDefaultNamespaceDefL()
	{
	}

CMdEObject* CMdESessionImpl::NewObjectL( CMdEObjectDef& aDef, const TDesC& aUri, TUint32 aMediaId )
	{
	CMdEObject* object = NewObjectLC( aDef, aUri, aMediaId );
	CleanupStack::Pop(object);
	return object;
	}

CMdEObject* CMdESessionImpl::NewObjectLC( CMdEObjectDef& aDef, const TDesC& aUri, TUint32 aMediaId )
	{
	CMdEObject* object = CMdEObject::NewLC( aDef, aUri, aMediaId );
	return object;
	}

void CMdESessionImpl::CommitObjectL(CMdEObject& aObject)
    {
    }

void CMdESessionImpl::CommitObjectsL(RPointerArray<CMdEObject>& aObjects)
    {
    }

TItemId CMdESessionImpl::CancelObjectL(CMdEObject& aObject)
    {
    }

CMdERelation* CMdESessionImpl::NewRelationLC( CMdERelationDef& aDef, TItemId aLeftObjectId, 
										      TItemId aRightObjectId, TInt32 aParameter )
	{
	}

CMdERelation* CMdESessionImpl::NewRelationL( CMdERelationDef& aDef, TItemId aLeftObjectId, 
										     TItemId aRightObjectId, TInt32 aParameter )
	{
	}


CMdEEvent* CMdESessionImpl::NewEventLC(CMdEEventDef& aDef, TItemId aObjectId, TTime aTime, const TDesC* aSource, const TDesC* aParticipant)
	{
	}

CMdEEvent* CMdESessionImpl::NewEventL(CMdEEventDef& aDef, TItemId aObjectId, TTime aTime, const TDesC* aSource, const TDesC* aParticipant)
	{
	}
	
void CMdESessionImpl::AddSchemaObserverL(MMdESchemaObserver& aObserver)
	{
	}


void CMdESessionImpl::RemoveSchemaObserverL(MMdESchemaObserver& aObserver)
	{
	}

void CMdESessionImpl::NotifySessionOpened(TInt aError)
	{
	}


void CMdESessionImpl::NotifyError(TInt aError)
	{
	if(iSessionObserver)
		{
		iSessionObserver->HandleSessionError(*this, aError);
		}
	iSessionState = EMdESessionClosed;
	}

void CMdESessionImpl::LoadSchemaL()
	{
	DoLoadSchemaL();
	}

void CMdESessionImpl::DoLoadSchemaL()
    {    }

void CMdESessionImpl::AddRelationDefL( const CMdENamespaceDef &aNamespaceDef, const TDesC &aName )
	{
	}

void CMdESessionImpl::AddEventDefL( const CMdENamespaceDef &aNamespaceDef, const TDesC &aName )
	{
	}


CMdEObject* CMdESessionImpl::GetObjectL( const TItemId aId, CMdENamespaceDef* aNamespaceDef )
	{}
	
CMdEObject* CMdESessionImpl::GetFullObjectL( const TItemId aId, CMdENamespaceDef* aNamespaceDef )
	{
	}

CMdEObject* CMdESessionImpl::GetObjectL( const TItemId aId, CMdEObjectDef& aObjectDef )
	{
    if( test->mdsoperationleaves )
        {
        User::Leave( test->mdsoperationleaves );
        }
    if ( test->mdsoperationreturns )
        {
        return NULL;
        }
    else
        {
        CMdEObject* object = CMdEObject::NewL( aObjectDef, KNullDesC(), aId );
        return object;
        }
	}

CMdEObject* CMdESessionImpl::GetObjectL( const TInt64 aGuidHigh, const TInt64 aGuidLow, CMdENamespaceDef* aNamespaceDef )
	{}

CMdEObject* CMdESessionImpl::GetFullObjectL( const TInt64 aGuidHigh, const TInt64 aGuidLow, CMdENamespaceDef* aNamespaceDef )
	{}

CMdEObject* CMdESessionImpl::GetObjectL( const TInt64 aGuidHigh, const TInt64 aGuidLow, CMdEObjectDef& aObjectDef )
	{}

CMdEObject* CMdESessionImpl::OpenObjectL( const TItemId aId, CMdENamespaceDef* aNamespaceDef )
	{}

CMdEObject* CMdESessionImpl::OpenFullObjectL( const TItemId aId, CMdENamespaceDef* aNamespaceDef )
	{}

CMdEObject* CMdESessionImpl::OpenObjectL( const TItemId aId, CMdEObjectDef& aObjectDef )
    {
    if( test->mdsoperationleaves )
        {
        User::Leave( test->mdsoperationleaves );
        }
    if ( test->mdsoperationreturns )
        {
        return NULL;
        }
    else
        {
        CMdEObject* object = CMdEObject::NewL( aObjectDef, KNullDesC(), aId );
        return object;
        }
    }

CMdEObject* CMdESessionImpl::OpenObjectL( const TInt64 aGuidHigh, const TInt64 aGuidLow, CMdENamespaceDef* aNamespaceDef )
	{}

CMdEObject* CMdESessionImpl::OpenFullObjectL( const TInt64 aGuidHigh, const TInt64 aGuidLow, CMdENamespaceDef* aNamespaceDef )
	{}

CMdEObject* CMdESessionImpl::OpenObjectL( const TInt64 aGuidHigh, const TInt64 aGuidLow, CMdEObjectDef& aObjectDef )
    { }

EXPORT_C CMdEObject* CMdESessionImpl::GetObjectL( const TDesC& aUri, CMdENamespaceDef* aNamespaceDef )
	{}
    
CMdEObject* CMdESessionImpl::GetFullObjectL( const TDesC& aUri, CMdENamespaceDef* aNamespaceDef )
	{}
    
CMdEObject* CMdESessionImpl::GetObjectL( const TDesC& aUri, CMdEObjectDef& aObjectDef )
    { }

    
CMdEObject* CMdESessionImpl::OpenObjectL( const TDesC& aUri, CMdENamespaceDef* aNamespaceDef )
	{}

CMdEObject* CMdESessionImpl::OpenFullObjectL( const TDesC& aUri, CMdENamespaceDef* aNamespaceDef )
	{}

CMdEObject* CMdESessionImpl::OpenObjectL( const TDesC& aUri, CMdEObjectDef& aObjectDef )
    { 
    CMdEObject* object = CMdEObject::NewLC( aObjectDef, aUri, 0 );
    return object;
    }

void CMdESessionImpl::CheckObjectL( TMdEObject& aObject, const TDesC& aUri, 
		CMdENamespaceDef* aNamespaceDef )
	{}

void CMdESessionImpl::CheckObjectL( TMdEObject& aObject, TItemId aId, 
		CMdENamespaceDef* aNamespaceDef )
	{}

void CMdESessionImpl::CheckObjectL( RArray<TMdEObject>& aObjects, 
		const RArray<TItemId>& aIds, CMdENamespaceDef* aNamespaceDef )
	{}

CMdERelation* CMdESessionImpl::GetRelationL(TItemId aId, CMdENamespaceDef* aNamespaceDef)
    { }


CMdEEvent* CMdESessionImpl::GetEventL(TItemId aId, 
		CMdENamespaceDef* aNamespaceDef)
    {}
CMdCSerializationBuffer* CMdESessionImpl::RemoveCommonL( 
		CMdENamespaceDef& aNamespaceDef, const RArray<TItemId>* aObjects,
		const RArray<TItemId>* aEvents, const RArray<TItemId>* aRelations )
	{}

CMdCSerializationBuffer* CMdESessionImpl::RemoveCommonL( 
		CMdENamespaceDef& aNamespaceDef, 
		const RPointerArray<TDesC16>* aObjects,
		const RArray<TItemId>* aEvents, const RArray<TItemId>* aRelations )
	{
	}

TInt CMdESessionImpl::DeserializeIdsL( RMdEDataBuffer& aSerializedItemIds, 
		RArray<TItemId>* aResultObjects, RArray<TItemId>* aResultEvents, 
		RArray<TItemId>* aResultRelations )
	{}

TItemId CMdESessionImpl::RemoveObjectL( TItemId aId, 
		CMdENamespaceDef* aNamespaceDef )
    {
    if ( test->mdsoperationleaves )
        {
        User::Leave( test->mdsoperationleaves );
        }
    else if ( test->mdsoperationreturns )
        {
        return 0;
        }
    else
        {
        return 1;
        }
    }

TItemId CMdESessionImpl::RemoveObjectL( const TDesC& aUri, 
		CMdENamespaceDef* aNamespaceDef )
    { }

TInt CMdESessionImpl::RemoveObjectsL( const RArray<TItemId>& aId, 
		RArray<TItemId>& aResult, CMdENamespaceDef* aNamespaceDef )
	{}

TInt CMdESessionImpl::RemoveObjectsL( const RPointerArray<TDesC>& aUri, 
		RArray<TItemId>& aResult, CMdENamespaceDef* aNamespaceDef )
	{}

void CMdESessionImpl::RemoveObjectsAsyncL( 
		const RArray<TItemId>& aId, TRequestStatus& aStatus, 
		RMdEDataBuffer& aSerializedObjectIds, 
		CMdENamespaceDef* aNamespaceDef )
	{}
	
void CMdESessionImpl::RemoveObjectsAsyncL( 
		const RPointerArray<TDesC>& aUri, TRequestStatus& aStatus, 
		RMdEDataBuffer& aSerializedObjectIds, 
		CMdENamespaceDef* aNamespaceDef )
	{}

TItemId CMdESessionImpl::RemoveRelationL(TItemId aId, 
		CMdENamespaceDef* aNamespaceDef)
    {}

TInt CMdESessionImpl::RemoveRelationsL(const RArray<TItemId>& aId, 
		RArray<TItemId>& aSuccessful, CMdENamespaceDef* aNamespaceDef)
	{}

void CMdESessionImpl::RemoveRelationsAsyncL(
		const RArray<TItemId>& aId, TRequestStatus& aStatus, 
		RMdEDataBuffer& aSerializedRelationIds, 
		CMdENamespaceDef* aNamespaceDef)
	{}

/**
* Add methods
*/
TItemId CMdESessionImpl::AddItemL( CMdEInstanceItem& aItem )
	{}

CMdCSerializationBuffer* CMdESessionImpl::SerializeItemsL( 
		RPointerArray<CMdEInstanceItem>& aItems )
    { }

void CMdESessionImpl::DeserializeQueryResultL( 
		CMdCSerializationBuffer& aBuffer, 
		RPointerArray<CMdEInstanceItem>& aItems )
	{}

TItemId CMdESessionImpl::AddObjectL( CMdEObject& aObject )
    {
    if( test->mdsoperationleaves )
        {
        User::Leave( test->mdsoperationleaves );
        }
    if ( test->mdsoperationreturns )
        {
        return 0;
        }
    else
        {
        return 1;
        }
    }

TInt CMdESessionImpl::AddObjectsL( RPointerArray<CMdEObject>& aObjects )
    {
    }

TItemId CMdESessionImpl::AddRelationL( CMdERelation& aRelation ) 
    {
    }

TItemId CMdESessionImpl::UpdateRelationL( CMdERelation& aRelation ) 
    {
    }

TInt CMdESessionImpl::AddItemsL( RPointerArray<CMdEInstanceItem>& aItems )
    {    
    }

void CMdESessionImpl::AddItemsAsyncL( 
		RPointerArray<CMdEInstanceItem>& aItems, TRequestStatus& aStatus, 
		RMdEDataBuffer& aSerializedItemIds )
    {    
    }

TInt CMdESessionImpl::UpdateItemsL( RPointerArray<CMdEInstanceItem>& aItems )
    {    }
void CMdESessionImpl::UpdateItemsAsyncL( 
		RPointerArray<CMdEInstanceItem>& aItems, TRequestStatus& aStatus,
		RMdEDataBuffer& aSerializedItemIds )
    {

    }

TInt CMdESessionImpl::DeserializeItemsL( RMdEDataBuffer& aSerializedItems, 
		RPointerArray<CMdEInstanceItem>& aItems )
	{
	}

TItemId CMdESessionImpl::AddEventL( CMdEEvent& aEvent )
    {
    }

TItemId CMdESessionImpl::RemoveEventL( TItemId aId, 
		CMdENamespaceDef* aNamespaceDef )
    {
    }

TInt CMdESessionImpl::RemoveEventsL( const RArray<TItemId>& aId, 
		RArray<TItemId>& aSuccessful, CMdENamespaceDef* aNamespaceDef )
	{
	}    
	
void CMdESessionImpl::RemoveEventsAsyncL( 
		const RArray<TItemId>& aId, TRequestStatus& aStatus, 
		RMdEDataBuffer& aSerializedEventIds, 
		CMdENamespaceDef* aNamespaceDef )
	{
	}
	

// Query 

CMdEObjectQuery* CMdESessionImpl::NewObjectQueryL(
		CMdENamespaceDef& aNamespaceDef, CMdEObjectDef& aObjectDef, 
		MMdEQueryObserver* aObserver)
    {    
    return new CMdEObjectQuery();
    }

CMdEObjectQuery* CMdESessionImpl::NewObjectQueryL(
		CMdEObjectDef& aObjectDef, RPointerArray<CMdEObjectDef>* aObjectDefs, 
		MMdEQueryObserver* aObserver)
	{
	}

CMdERelationQuery* CMdESessionImpl::NewRelationQueryL(
		CMdENamespaceDef& aNamespaceDef, MMdEQueryObserver* aObserver)
    {}
CMdEEventQuery* CMdESessionImpl::NewEventQueryL(
		CMdENamespaceDef& aNamespaceDef, MMdEQueryObserver* aObserver)
    {}
void CMdESessionImpl::AddObjectObserverL( MMdEObjectObserver& aObserver,
    									  CMdELogicCondition* aCondition, 
     									  TUint32 aNotificationType,
    									  CMdENamespaceDef* aNamespaceDef )
    {}
void CMdESessionImpl::AddObjectObserverWithUriL( MMdEObjectObserverWithUri& aObserver,
                                          CMdELogicCondition* aCondition, 
                                          TUint32 aNotificationType,
                                          CMdENamespaceDef* aNamespaceDef )
    {}
void CMdESessionImpl::AddObjectPresentObserverL(
		MMdEObjectPresentObserver& aObserver)
	{}
void CMdESessionImpl::RemoveObjectObserverWithUriL( 
        MMdEObjectObserverWithUri& aObserver, CMdENamespaceDef* aNamespaceDef )
    {}
void CMdESessionImpl::AddRelationObserverL( MMdERelationObserver& aObserver,
    										CMdECondition* aCondition, 
    										TUint32 aNotificationType,
    										CMdENamespaceDef* aNamespaceDef )
    {}
void CMdESessionImpl::AddRelationItemObserverL( 
		MMdERelationItemObserver& aObserver, CMdECondition* aCondition, 
		TUint32 aNotificationType, CMdENamespaceDef* aNamespaceDef )
	{}
void CMdESessionImpl::AddRelationPresentObserverL(
		MMdERelationPresentObserver& aObserver)
	{}
void CMdESessionImpl::AddEventObserverL( MMdEEventObserver& aObserver,
    									 CMdECondition* aCondition, 
    									 TUint32 aNotificationType,
    									 CMdENamespaceDef* aNamespaceDef )
    {}
void CMdESessionImpl::RemoveObjectObserverL( 
		MMdEObjectObserver& aObserver, CMdENamespaceDef* aNamespaceDef )
    {}
void CMdESessionImpl::RemoveObjectPresentObserverL(
		MMdEObjectPresentObserver& aObserver)
	{}
void CMdESessionImpl::RemoveRelationObserverL( 
		MMdERelationObserver& aObserver, CMdENamespaceDef* aNamespaceDef )
    {}
void CMdESessionImpl::RemoveRelationItemObserverL( 
		MMdERelationItemObserver& aObserver, CMdENamespaceDef* aNamespaceDef )
    {}
void CMdESessionImpl::RemoveRelationPresentObserverL(
		MMdERelationPresentObserver& aObserver)
	{}
void CMdESessionImpl::RemoveEventObserverL( 
		MMdEEventObserver& aObserver, CMdENamespaceDef* aNamespaceDef )
    {}
TInt CMdESessionImpl::FindNotifier( TUint32 aNotifyType, TAny* aObserver, 
									CMdENamespaceDef& aNamespaceDef )
    {}
void CMdESessionImpl::NotifierInError( CMdENotifierAO* aNotifier )
    {}
void CMdESessionImpl::ImportSchemaL( const TDesC& aFileName )
    {}
TInt CMdESessionImpl::ImportMetadataL( const TDesC& aFileName )
    {}
void CMdESessionImpl::ImportMetadata( const TDesC& aFileName, 
		TPckgBuf<TInt>& aResult, TRequestStatus& aStatus )
	{}
CMdCSerializationBuffer* CMdESessionImpl::ExportCommonL( 
		const CMdENamespaceDef* aNamespaceDef,
		const RPointerArray<CMdEObjectDef>* aObjectDefs,
        const RPointerArray<CMdERelationDef>* aRelationDefs,
        const RPointerArray<CMdEEventDef>* aEventDefs )
	{}
void CMdESessionImpl::ExportMetadataL( const TDesC& aFileName,
    	const CMdENamespaceDef* aNamespaceDef,
	    const RPointerArray<CMdEObjectDef>* aObjectDefs,
        const RPointerArray<CMdERelationDef>* aRelationDefs,
        const RPointerArray<CMdEEventDef>* aEventDefs )
    {}

void CMdESessionImpl::ExportMetadataL( const TDesC& aFileName, 
		TRequestStatus& aStatus, RMdEDataBuffer& aBuffer, 
        const CMdENamespaceDef* aNamespaceDef, 
	    const RPointerArray<CMdEObjectDef>* aObjectDefs, 
        const RPointerArray<CMdERelationDef>* aRelationDefs, 
        const RPointerArray<CMdEEventDef>* aEventDefs )
    {}
void CMdESessionImpl::GetSchemaVersionL(
		TInt& aMajorVersion, TInt& aMinorVersion)
	{}
void CMdESessionImpl::SetObjectToPresentByGuidL( 
		const TInt64& aGuidHigh, const TInt64& aGuidLow )
	{}
void CMdESessionImpl::CheckOpened() const {}
void CMdESessionImpl::GetCountL( CMdCSerializationBuffer* aBuffer, 
		TUint32& aResult )
	{}
void CMdESessionImpl::GetItemIdL( CMdCSerializationBuffer* aBuffer, 
		RArray<TItemId>& aIdArray )
	{}
void CMdESessionImpl::GetDistinctValuesL( CMdCSerializationBuffer& aBuffer, 
		CDesCArray& aResults )
	{}
CMdENamespaceDef* CMdESessionImpl::GetNamespaceDefL( 
		CMdENamespaceDef* aNamespaceDef )
	{}
