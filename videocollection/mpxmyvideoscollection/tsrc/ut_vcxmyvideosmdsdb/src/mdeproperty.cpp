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


#include "mdeproperty.h"
#include "mdepropertydef.h"
#include "mdeobject.h"
#include "mdeobjectdef.h"
#include "mdeerror.h"
#include "mdesession.h"

CMdEProperty::CMdEProperty(const CMdEPropertyDef& aDef, const CMdEObject& aObject)
		: CMdEInstanceItem( CONST_CAST( CMdESession*, &aDef.NamespaceDef().Session() ), KNoId), 
		iModFlags(0), iDef(aDef), iObject(aObject)
	{
	}

CMdEProperty* CMdEProperty::NewL(const CMdEPropertyDef& aDef, const CMdEObject& aObject)
    {
    }

CMdEProperty::CMdEProperty() :  
        CMdEInstanceItem(  NULL, 0  ), 
        iDef(const_cast<const CMdEPropertyDef&>( *(CMdEPropertyDef*)0) ), 
        iObject(*(CMdEObject*)0)
    {
    }

CMdEProperty* CMdEProperty::NewLC(const CMdEPropertyDef& aDef, const CMdEObject& aObject)
    {
    }


CMdEProperty* CMdEProperty::NewL( const CMdEObject& aObject, CMdCSerializationBuffer& aBuffer )
	{
	}

CMdEProperty* CMdEProperty::NewLC( const CMdEObject& aObject, CMdCSerializationBuffer& aBuffer )
	{
	}

void CMdEProperty::BaseConstructL()
	{
	}

CMdEProperty::~CMdEProperty()
	{
	}

EXPORT_C const CMdEPropertyDef& CMdEProperty::Def() const
	{
	}

EXPORT_C const CMdEObject& CMdEProperty::Object() const
	{
	}

EXPORT_C TBool CMdEProperty::Modified() const
	{
	}

TUint8 CMdEProperty::ModFlags() const
	{
	}

void CMdEProperty::SetRemoved()
	{
	}

TBool CMdEProperty::Removed()
	{
	}

void CMdEProperty::ClearFlags()
	{
	}

void CMdEProperty::SetModified()
	{
	}

TMdEInstanceType CMdEProperty::InstanceType() const
	{
	}

EXPORT_C TBool CMdEProperty::BoolValueL() const
	{
	}

EXPORT_C TInt8 CMdEProperty::Int8ValueL() const
	{
	}

EXPORT_C TUint8 CMdEProperty::Uint8ValueL() const
	{
	}

EXPORT_C TInt16 CMdEProperty::Int16ValueL() const
	{
	}

EXPORT_C TUint16 CMdEProperty::Uint16ValueL() const
	{
	}

EXPORT_C TInt32 CMdEProperty::Int32ValueL() const
	{
	}

EXPORT_C TUint32 CMdEProperty::Uint32ValueL() const
	{
	}

EXPORT_C TInt64 CMdEProperty::Int64ValueL() const
	{
	}

EXPORT_C TReal32 CMdEProperty::Real32ValueL() const
	{
	}

EXPORT_C TReal64 CMdEProperty::Real64ValueL() const
	{
	}

EXPORT_C TTime CMdEProperty::TimeValueL() const
	{
	}

EXPORT_C const TDesC& CMdEProperty::TextValueL() const
	{
    return KNullDesC();
	}


EXPORT_C void CMdEProperty::SetBoolValueL(TBool aValue)
	{
	}

EXPORT_C void CMdEProperty::SetInt8ValueL(TInt8 aValue)
	{
	}

EXPORT_C void CMdEProperty::SetUint8ValueL(TUint8 aValue)
	{
	}


EXPORT_C void CMdEProperty::SetInt16ValueL(TInt16 aValue)
	{
	}

EXPORT_C void CMdEProperty::SetUint16ValueL(TUint16 aValue)
	{
	}

EXPORT_C void CMdEProperty::SetInt32ValueL(TInt32 aValue)
	{
	}

EXPORT_C void CMdEProperty::SetUint32ValueL(TUint32 aValue)
	{
	}

EXPORT_C void CMdEProperty::SetInt64ValueL(TInt64 aValue)
	{
	}

EXPORT_C void CMdEProperty::SetReal32ValueL(TReal32 aValue)
	{
	}

EXPORT_C void CMdEProperty::SetReal64ValueL(TReal64 aValue)
	{
	}

EXPORT_C void CMdEProperty::SetTimeValueL(const TTime& aValue)
	{
	}

EXPORT_C void CMdEProperty::SetTextValueL(const TDesC& aValue)
    {
    }

void CMdEProperty::DoSetTextValueL(const TDesC& aValue)
    {
	}

TMdCOffset CMdEProperty::SerializeL(CMdCSerializationBuffer& aBuffer, TMdCOffset aFreespaceOffset) const
	{
	}

