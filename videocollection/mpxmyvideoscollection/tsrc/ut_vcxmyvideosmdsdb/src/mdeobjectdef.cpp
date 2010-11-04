/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/

#include "mdeobjectdef.h"
#include "mdenamespacedef.h"
#include "mdepropertydef.h"



CMdEObjectDef::CMdEObjectDef( const TMdCObjectDef& aObjectDef, CMdENamespaceDef& aNamespaceDef )
    : iObjectDef( aObjectDef ), iNamespaceDef(aNamespaceDef)
    {
    }

CMdEObjectDef* CMdEObjectDef::NewL( const TMdCObjectDef& aObjectDef,
		CMdCSerializationBuffer& aSchemaBuffer, CMdENamespaceDef& aNamespaceDef )
    {
    }

CMdEObjectDef* CMdEObjectDef::NewLC( const TMdCObjectDef& aObjectDef,
		CMdCSerializationBuffer& aSchemaBuffer, CMdENamespaceDef& aNamespaceDef )
    {
    }

void CMdEObjectDef::ConstructL(CMdCSerializationBuffer& aSchemaBuffer)
    {
    }

CMdEObjectDef::~CMdEObjectDef()
    {
    }

EXPORT_C TDefId CMdEObjectDef::Id() const
    {
    }

EXPORT_C CMdENamespaceDef& CMdEObjectDef::NamespaceDef() const
    {
    }

EXPORT_C CMdEObjectDef* CMdEObjectDef::ParentL()
    {
    }

EXPORT_C const TDesC& CMdEObjectDef::Name() const
    {
    }

EXPORT_C TInt CMdEObjectDef::Compare( const CMdEObjectDef& aObjectDef ) const
    {
    }

EXPORT_C TInt CMdEObjectDef::PropertyDefCount() const
    {
    }

EXPORT_C CMdEPropertyDef& CMdEObjectDef::PropertyDefL(TInt aIndex)
	{
	}

EXPORT_C CMdEPropertyDef& CMdEObjectDef::GetPropertyDefL( const TDesC& aName )
	{
	}

CMdEPropertyDef* CMdEObjectDef::GetPropertyDefL( const TDesC& aName, CMdEObjectDef* aChild )
    {    
    }

CMdEPropertyDef* CMdEObjectDef::GetPropertyDefL(TDefId aId, CMdEObjectDef* aChild)
    {
    }

CMdEPropertyDef* CMdEObjectDef::PropertyDefL(TInt aIndex, CMdEObjectDef* /*aChild*/)
	{
	}

TInt CMdEObjectDef::CompareProperties(const CMdEPropertyDef& aFirst, const CMdEPropertyDef& aSecond)
    {
    }

