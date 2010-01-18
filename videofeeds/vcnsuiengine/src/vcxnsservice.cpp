/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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




#include "IptvDebug.h"
#include "vcxnsservice.h"

// -----------------------------------------------------------------------------
// CVcxNsService::CVcxNsService()
// -----------------------------------------------------------------------------
//
CVcxNsService::CVcxNsService()
    : iHighlightIndex( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsService::~CVcxNsService()
// -----------------------------------------------------------------------------
//
CVcxNsService::~CVcxNsService( )
    {
    delete iIconPath;
    delete iName;
    delete iUserName;
    delete iPassword;
    delete iCustomUri;
    delete iDescription;
    delete iAccountMgmtUri;
    
    iServiceGroup.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVcxNsService::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsService* CVcxNsService::NewL( )
    {
    CVcxNsService* self = new ( ELeave ) CVcxNsService;
    
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetName()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsService::GetName()
    {
    if( iName && iName->Length() > 0 )
        {
        return *iName;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetDesc()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsService::GetDesc()
    {
    if( iDescription && iDescription->Length() > 0 )
        {
        return *iDescription;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetIconPath()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsService::GetIconPath()
    {
    if( iIconPath && iIconPath->Length() > 0 )
        {
        return *iIconPath;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetUserNameL()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsService::GetUserName()
    {
    if( iUserName && iUserName->Length() > 0 )
        {
        return *iUserName;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetPasswordL()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsService::GetPassword()
    {
    if( iPassword && iPassword->Length() > 0 )
        {
        return *iPassword;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetServiceId()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsService::GetServiceId()
    {
    return iServiceId;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetUpdateTime()
// -----------------------------------------------------------------------------
//
EXPORT_C TTime  CVcxNsService::GetUpdateTime()
    {
    return iUpdateTime;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetUpdateStatus()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsService::TServiceUpdateStatus CVcxNsService::GetUpdateStatus()
    {
    return iUpdateStatus;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetIap()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsService::GetIap()
    {
    return iIap;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::GetIsReadOnlyIap()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsService::GetIsReadOnlyIap()
    {
    return iIsReadOnlyIap;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::Type()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsService::TVcxNsVodServiceType CVcxNsService::Type() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::AccountMgmtUri
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsService::AccountMgmtUri()
    {
    if ( iAccountMgmtUri )
        {
        return *iAccountMgmtUri;
        }
    else
        { 
        return KNullDesC();
        }    
    }

// -----------------------------------------------------------------------------
// CVcxNsService::CustomUri
// Get URI that can be used in browser launch
// -----------------------------------------------------------------------------
//
const TDesC& CVcxNsService::CustomUri( )
    {
    if ( iCustomUri )
        {
        return *iCustomUri;
        }
    else
        { 
        return KNullDesC();
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetNameL()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetNameL( const TDesC& aName )
    {
    delete iName;
    iName = NULL;
    
    if( aName.Length() > 0 )
        {
        iName = aName.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetDescL()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetDescL( const TDesC& aDesc )
    {
    delete iDescription;
    iDescription = NULL;
    
    if( aDesc.Length() > 0 )
        {
        iDescription = aDesc.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetIconPathL()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetIconPathL(  const TDesC& aPath )
    {
    delete iIconPath;
    iIconPath = NULL;
    
    if( aPath.Length() > 0 )
        {
        iIconPath = aPath.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetUserNameL()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetUserNameL(  const TDesC& aPath )
    {
    delete iUserName;
    iUserName = NULL;
    
    if( aPath.Length() > 0 )
        {
        iUserName = aPath.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetPasswordL()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetPasswordL(  const TDesC& aPath )
    {
    delete iPassword;
    iPassword = NULL;
    
    if( aPath.Length() > 0 )
        {
        iPassword = aPath.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetServiceId()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetServiceId( const TUint32 aServiceId )
    {
    iServiceId = aServiceId;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetUpdateTime()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetUpdateTime( const TTime aUpdateTime )
    {
    iUpdateTime = aUpdateTime;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetUpdateStatus()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetUpdateStatus( const TServiceUpdateStatus aUpdateStatus )
    {
#ifdef _DEBUG    
    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsService::SetUpdateStatus for service: %d ", iServiceId );

    switch ( aUpdateStatus )
        {
        case ENotUpdating:
            IPTVLOGSTRING_LOW_LEVEL("ENotUpdating");
            break;
        case EUpdatePending:
            IPTVLOGSTRING_LOW_LEVEL("EUpdatePending");
            break;
        case EStartingPending:
            IPTVLOGSTRING_LOW_LEVEL("EStartingPending");
            break;
        case EUpdateOngoing:
            IPTVLOGSTRING_LOW_LEVEL("EUpdateOngoing");
            break;
        case EUpdateFailed:
            IPTVLOGSTRING_LOW_LEVEL("EUpdateFailed");
            break;
        default:
            break;
        }
#endif
    iUpdateStatus = aUpdateStatus;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetIap()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetIap( const TUint32 aIap )
    {
    iIap = aIap;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetIsReadOnlyIap()
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetIsReadOnlyIap( const TBool aIsReadOnlyIap )
    {
    iIsReadOnlyIap = aIsReadOnlyIap;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetUriL
// Set URI that can be used in browser launch.
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetCustomUriL( const TDesC& aCustomUri )
    {
    delete iCustomUri;
    iCustomUri = NULL;

    iCustomUri = aCustomUri.AllocL();
    }

// -----------------------------------------------------------------------------
// Set service type
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetType( const TVcxNsVodServiceType aType )
    {
    iType = aType;
    }

// -----------------------------------------------------------------------------
// CVcxNsService::SetAccountMgmtUriL
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetAccountMgmtUriL( const TDesC& aAccountMgmtUri )
    {
    delete iAccountMgmtUri;
    iAccountMgmtUri = NULL;
    
    if ( aAccountMgmtUri.Length() > 0 )
        {
        iAccountMgmtUri = aAccountMgmtUri.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// Set group id
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetGroupId( TUint32 aGroupId )
    {
    iGroupId = aGroupId;
    }

// -----------------------------------------------------------------------------
// Get group id
// -----------------------------------------------------------------------------
//
TUint32 CVcxNsService::GroupId()
    {
    return iGroupId;
    }

// -----------------------------------------------------------------------------
// Add service to service group
// -----------------------------------------------------------------------------
//
void CVcxNsService::AddSubServiceL( CVcxNsService* aSubService )
    {
    if ( iType != EVcxNsMainGroup || !aSubService )
        {
        User::Leave( KErrArgument );
        }

    iServiceGroup.AppendL( aSubService );
    }

// -----------------------------------------------------------------------------
// Get service group
// -----------------------------------------------------------------------------
//
RPointerArray<CVcxNsService>& CVcxNsService::ServiceGroup()
    {
    return iServiceGroup;
    }

// -----------------------------------------------------------------------------
// Set highlight index
// -----------------------------------------------------------------------------
//
void CVcxNsService::SetHighlightCategoryIndex( TInt aIndex )
    {
    iHighlightIndex = aIndex;
    }

// -----------------------------------------------------------------------------
// Get highlighted index
// -----------------------------------------------------------------------------
//
TInt CVcxNsService::HighlightCategoryIndex()
    {
    return iHighlightIndex;
    }
