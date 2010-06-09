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


#include <mdeobject.h>

#include <mdeproperty.h>
#include <mdeobjectdef.h>
#include <mdenamespacedef.h>
#include <mdesession.h>

#include "VcxMyVideosMdsDbTest.h"

//////////////////////////////
//                          //
//        TMdEObject        //
//                          //
//////////////////////////////

EXPORT_C TMdEObject::TMdEObject() : 
	iDef( NULL )
	{
	}

EXPORT_C TItemId TMdEObject::Id() const
	{
	}

EXPORT_C const CMdEObjectDef& TMdEObject::DefL() const
	{
	}

EXPORT_C TBool TMdEObject::Confidential() const
	{
	}

EXPORT_C TBool TMdEObject::Placeholder() const
	{
	}

EXPORT_C TBool TMdEObject::Removed() const
	{
	}

EXPORT_C TBool TMdEObject::NotPresent() const
	{
	}

TUint32 TMdEObject::RequiredBufferSize()
	{
	}

void TMdEObject::DeSerializeL(CMdCSerializationBuffer& aBuffer, 
		CMdENamespaceDef& aNamespaceDef)
	{
	}

//////////////////////////////
//                          //
//        CMdEObject        //
//                          //
//////////////////////////////

CMdEObject* CMdEObject::NewL( CMdEObjectDef& aDef, const TDesC& aUri, TUint32 aMediaId )
	{
    CMdEObject* nev = new CMdEObject(NULL,aMediaId,aDef);
    return nev;
	}

CMdEObject* CMdEObject::NewLC( CMdEObjectDef& aDef, const TDesC& aUri, TUint32 aMediaId )
	{
    CMdEObject* nev = new CMdEObject(NULL,aMediaId,aDef);
    CleanupStack::PushL( nev );
    return nev;
	}

CMdEObject::CMdEObject(CMdESession* aSession, TItemId aId, CMdEObjectDef& aDef)
		: CMdEInstanceItem( aSession, aId), iDef( &aDef )
	{
	}

void CMdEObject::ConstructL( const TDesC& aUri, TUint32 aMediaId )
	{
   	}

CMdEObject::~CMdEObject()
	{
    delete iProperty;
	}

EXPORT_C CMdEObjectDef& CMdEObject::Def() const
	{
	}

EXPORT_C void CMdEObject::SetDefL(CMdEObjectDef& aDef)
	{
	}

EXPORT_C const TDesC& CMdEObject::Uri() const
	{
    return KNullDesC();
	}

EXPORT_C void CMdEObject::SetUriL(const TDesC& aUri)
	{
	}

EXPORT_C TUint32 CMdEObject::MediaId() const
	{
	}

EXPORT_C void CMdEObject::SetMediaId(TUint32 aMediaId)
	{
	}

EXPORT_C TUint32 CMdEObject::UsageCount() const
	{
	}

EXPORT_C void CMdEObject::Guid( TInt64& aGuidHigh, TInt64& aGuidLow ) const
	{
	}
	
EXPORT_C void CMdEObject::SetGuid( const TInt64& aGuidHigh, const TInt64& aGuidLow )
	{
	}


EXPORT_C TBool CMdEObject::Confidential() const
	{
	}

EXPORT_C void CMdEObject::SetConfidential( TBool aValue )
	{
    
	}

EXPORT_C TBool CMdEObject::Placeholder() const
	{
	}

EXPORT_C void CMdEObject::SetPlaceholder( TBool aValue )
	{
	}

void CMdEObject::DoSetUriL(const TDesC& aUri)
	{
	}

EXPORT_C TInt CMdEObject::PropertyCount() const
	{
	}

EXPORT_C CMdEProperty& CMdEObject::PropertyL(TInt aIndex) const
	{
	}

EXPORT_C TInt CMdEObject::PropertyCount(const CMdEPropertyDef& aDef) const
	{
	}

EXPORT_C TInt CMdEObject::Property(const CMdEPropertyDef& aDef, CMdEProperty*& aProperty, TInt aStartAt) const
    {
    if ( test->propertynotdefined )
        {
        return KErrNotFound;
        }
    if( !iProperty )
        {
        iProperty = new CMdEProperty();
        }
    
    if ( test->propertyisnull )
        {
        aProperty = NULL;
        }
    }
    
EXPORT_C CMdEProperty& CMdEObject::AddBoolPropertyL(CMdEPropertyDef& aDef, TBool aValue)
	{

	}

EXPORT_C CMdEProperty& CMdEObject::AddInt8PropertyL(CMdEPropertyDef& aDef, TInt8 aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddUint8PropertyL(CMdEPropertyDef& aDef, TUint8 aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddInt16PropertyL(CMdEPropertyDef& aDef, TInt16 aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddUint16PropertyL(CMdEPropertyDef& aDef, TUint16 aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddInt32PropertyL(CMdEPropertyDef& aDef, TInt32 aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddUint32PropertyL(CMdEPropertyDef& aDef, TUint32 aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddInt64PropertyL(CMdEPropertyDef& aDef, TInt64 aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddReal32PropertyL(CMdEPropertyDef& aDef, TReal32 aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddReal64PropertyL(CMdEPropertyDef& aDef, TReal64 aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddTextPropertyL(CMdEPropertyDef& aDef, const TDesC& aValue)
	{
	}

EXPORT_C CMdEProperty& CMdEObject::AddTimePropertyL(CMdEPropertyDef& aDef, TTime aValue)
	{

	}

void CMdEObject::DoAddPropertyL(CMdEProperty& aProperty)
	{
	}

EXPORT_C void CMdEObject::RemoveProperty(TInt aIndex)
	{
	}

TInt CMdEObject::CompareProperties(const CMdEProperty& aFirst, const CMdEProperty& aSecond)
	{
	}

EXPORT_C TInt CMdEObject::FreeTextCount() const
	{
	return iFreeTextArray.Count();
	}

EXPORT_C TPtrC CMdEObject::FreeText(TInt aIndex) const
	{
	return *iFreeTextArray[aIndex];
	}

EXPORT_C TInt CMdEObject::FreeTextIndex(const TDesC& aFreeText) const
	{
	}

EXPORT_C void CMdEObject::AddFreeTextL(const TDesC& aFreeText)
	{
	}

TInt CMdEObject::DoAddFreeTextL(const HBufC& aFreeText)
	{
	}

EXPORT_C void CMdEObject::RemoveFreeText(TInt aIndex)
	{
	}

TInt CMdEObject::CompareFreeTexts(const HBufC& aFirst, const HBufC& aSecond)
	{
	}

EXPORT_C void CMdEObject::MovePropertiesL(CMdEObject& aObject)
	{
	}


TBool CMdEObject::ObjectModified() const
    {
    }

TBool CMdEObject::FreeTextModified() const
    {
    }

TBool CMdEObject::PropertyModified() const
    {
    }

void CMdEObject::AutoLockL() const
	{
	}

EXPORT_C TBool CMdEObject::OpenForModifications() const
	{
    if ( test->mdeobjectopenresult )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
	}

void CMdEObject::ClearObject( TBool aClearFlags )
	{
	}

void CMdEObject::SetNotOpenForModifications()
	{
	}

TUint32 CMdEObject::ModifiedPropertiesCount() const
	{
	}

TUint32 CMdEObject::RequiredBufferSize() const
	{
	}

TMdCOffset CMdEObject::SerializeL(CMdCSerializationBuffer& aBuffer, TMdCOffset aFreespaceOffset) const
	{
	}

CMdEObject* CMdEObject::NewL( CMdESession* aSession, CMdCSerializationBuffer& aBuffer, CMdENamespaceDef& aNamespaceDef )
	{
	}


CMdEObject* CMdEObject::NewLC( CMdESession* aSession, CMdCSerializationBuffer& aBuffer, CMdENamespaceDef& aNamespaceDef )
	{
	}

TMdEInstanceType CMdEObject::InstanceType() const
	{
	}

