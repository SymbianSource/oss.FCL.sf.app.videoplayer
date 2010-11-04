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

#include "mdepropertydef.h"

#include "mdeobjectdef.h"
#include "mdenamespacedef.h"


CMdEPropertyDef::CMdEPropertyDef( const TMdCPropertyDef& aPropertyDef, CMdEObjectDef& aObjectDef )
    : iPropertyDef(aPropertyDef), iObjectDef(aObjectDef)
    {
    }

CMdEPropertyDef* CMdEPropertyDef::NewL( const TMdCPropertyDef& aPropertyDef,
    CMdCSerializationBuffer& aSchemaBuffer, CMdEObjectDef& aObjectDef )
    {
    }

CMdEPropertyDef* CMdEPropertyDef::NewLC( const TMdCPropertyDef& aPropertyDef,
    CMdCSerializationBuffer& aSchemaBuffer, CMdEObjectDef& aObjectDef )
    {
    }

void CMdEPropertyDef::ConstructL( CMdCSerializationBuffer& aSchemaBuffer )
    {
    }

CMdEPropertyDef::~CMdEPropertyDef()
    {
    }

TDefId CMdEPropertyDef::Id() const
    {
    }

TDefId CMdEPropertyDef::ObjectDefId() const
	{
	}

EXPORT_C CMdEObjectDef& CMdEPropertyDef::ObjectDef() const
    {
    }
    
EXPORT_C const TDesC& CMdEPropertyDef::Name() const
    {
    }

EXPORT_C TInt CMdEPropertyDef::Compare( const CMdEPropertyDef& aPropertyDef ) const
    {
    }

EXPORT_C TPropertyType CMdEPropertyDef::PropertyType() const
    {
    }

EXPORT_C TBool CMdEPropertyDef::ReadOnly() const
    {
    }

EXPORT_C TBool CMdEPropertyDef::Mandatory() const
	{
	}

EXPORT_C TInt32 CMdEPropertyDef::MinInt32ValueL() const
    {
    }

EXPORT_C TInt32 CMdEPropertyDef::MaxInt32ValueL() const
    {
    }

EXPORT_C TUint32 CMdEPropertyDef::MinUint32ValueL() const
    {
    }

EXPORT_C TUint32 CMdEPropertyDef::MaxUint32ValueL() const
    {
    }

EXPORT_C TInt64 CMdEPropertyDef::MinInt64ValueL() const
    {
    }

EXPORT_C TInt64 CMdEPropertyDef::MaxInt64ValueL() const
    {
    }

EXPORT_C TReal CMdEPropertyDef::MinRealValueL() const
    {
    }

EXPORT_C TReal CMdEPropertyDef::MaxRealValueL() const
    {
    }

EXPORT_C TTime CMdEPropertyDef::MinTimeValueL() const
    {
    }

EXPORT_C TTime CMdEPropertyDef::MaxTimeValueL() const
    {
    }

EXPORT_C TInt CMdEPropertyDef::MinTextLengthL() const
    {
    }

EXPORT_C TInt CMdEPropertyDef::MaxTextLengthL() const
    {
    
    }

EXPORT_C CMdENamespaceDef& CMdEPropertyDef::NamespaceDef() const
    {
    }
