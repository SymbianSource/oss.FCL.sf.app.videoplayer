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
* Description:  ?Description
*
*/


#include "mdenamespacedef.h"

#include "mdeobjectdef.h"
#include "mderelationdef.h"
#include "mdeeventdef.h"

CMdENamespaceDef::CMdENamespaceDef( const CMdESession& aMdeSession, 
		const TMdCNamespaceDef& aNamespaceDef )
	: iMdeSession( aMdeSession ), iNamespaceDef( aNamespaceDef )
	{
	}

CMdENamespaceDef* CMdENamespaceDef::NewL( const CMdESession& aMdeSession, 
		const TMdCNamespaceDef& aNamespaceDef, 
		CMdCSerializationBuffer& aSchemaBuffer )
	{
    }

CMdENamespaceDef* CMdENamespaceDef::NewLC( const CMdESession& aMdeSession, 
		const TMdCNamespaceDef& aNamespaceDef, 
		CMdCSerializationBuffer& aSchemaBuffer )
	{
	}

void CMdENamespaceDef::ConstructL(CMdCSerializationBuffer& aSchemaBuffer)
	{
	}

CMdENamespaceDef::~CMdENamespaceDef()
	{
	}

TDefId CMdENamespaceDef::Id() const
	{
	}

EXPORT_C TBool CMdENamespaceDef::ReadOnly() const
	{
	}

EXPORT_C const TDesC& CMdENamespaceDef::Name() const
	{
	}

EXPORT_C TInt CMdENamespaceDef::Compare( const CMdENamespaceDef& aNamespaceDef ) const
	{
	}

EXPORT_C TInt CMdENamespaceDef::ObjectDefCount() const
	{
	}

EXPORT_C CMdEObjectDef& CMdENamespaceDef::ObjectDefL(TInt aIndex)
	{
	}

EXPORT_C CMdEObjectDef& CMdENamespaceDef::GetObjectDefL(const TDesC& aName)
	{
	}

CMdEObjectDef* CMdENamespaceDef::GetObjectDefL(TDefId aId)
	{
	}

EXPORT_C TInt CMdENamespaceDef::RelationDefCount() const
	{
	}

EXPORT_C CMdERelationDef& CMdENamespaceDef::RelationDefL(TInt aIndex)
	{
	}

EXPORT_C CMdERelationDef& CMdENamespaceDef::GetRelationDefL(const TDesC& aName)
	{
	}

CMdERelationDef* CMdENamespaceDef::GetRelationDefL(TDefId aId)
	{
	}

EXPORT_C TInt CMdENamespaceDef::EventDefCount() const
	{
	}

EXPORT_C CMdEEventDef& CMdENamespaceDef::EventDefL(TInt aIndex)
	{
	}

EXPORT_C CMdEEventDef& CMdENamespaceDef::GetEventDefL(const TDesC& aName)
	{
	}

CMdEEventDef* CMdENamespaceDef::GetEventDefL(TDefId aId)
	{
	}

const CMdESession& CMdENamespaceDef::Session() const
	{
	}
