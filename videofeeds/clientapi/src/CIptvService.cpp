/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Holds information of the service.*
*/





// INCLUDE FILES
#include <e32base.h>
#include "IptvDebug.h"
#include <sysutil.h>

#include "ciptvsecureprovisioningparameters.h"
#include "CIptvService.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvService::ConstructL()
    {
    iIapList = CIptvIapList::NewL();
    // Create right away since we want SearchUrl() and UploadUrl() to always
    // return something meaningfull
    iSearchUrl = HBufC::NewL( 1 );  
    iRecordUrl = HBufC::NewL( 1 );
    }

// -----------------------------------------------------------------------------
// CIptvService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvService* CIptvService::NewL()
    {
    CIptvService* self = new( ELeave ) CIptvService;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::~CIptvService()
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvService::~CIptvService()
    {
    delete iIapList;
    delete iSearchUrl;
    delete iRecordUrl;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::SetId()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetId( TUint32 aId )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvService::SetId() aId = %d", aId);   
    iId = aId;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetId()
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C TUint32 CIptvService::GetId()
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvService::GetId() iId = %d", iId);
    return iId;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::SetName()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetName( const TDesC16& aName )
    {
    if ( aName.Length() > KIptvSmServicesDbNameMaxLength )
        {
        return KErrNoMemory;
        }
        
    iName = aName;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetName()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TDes16& CIptvService::GetName()
    {
    return iName;    
    }

// -----------------------------------------------------------------------------
// CIptvService::SetAddress()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetAddress( const TDesC16& aAddress )
    {
    if( aAddress.Length() > KIptvSmServicesDbAddressMaxLength )
        {
        return KErrNoMemory;
        }
        
    iAddress = aAddress;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetAddress()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TDes16& CIptvService::GetAddress()
    {
    return iAddress;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::SetIconPath()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetIconPath( const TDesC16& aIconPath )
    {
    if ( aIconPath.Length() > KIptvSmServicesDbIconPathMaxLength )
        {
        return KErrNoMemory;
        }
        
    iIconPath = aIconPath;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetIconPath()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TDes16& CIptvService::GetIconPath()
    {
    return iIconPath;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::SetEpgPluginUid()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetEpgPluginUid( TUid aEpgPluginUid )
    {
    iEpgPluginUid = aEpgPluginUid.iUid;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::GetEpgPluginUid()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CIptvService::GetEpgPluginUid()
    {
    TUid uid;
    uid.iUid = iEpgPluginUid;
    
    return uid;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetVodPluginUid()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetVodPluginUid( TUid aVodPluginUid )
    {
    iVodPluginUid = aVodPluginUid.iUid;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::GetVodPluginUid()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CIptvService::GetVodPluginUid()
    {
    TUid uid;
    uid.iUid = iVodPluginUid;

    return uid;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetIptvPluginUid()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetIptvPluginUid( TUid aIptvPluginUid )
    {
    iIptvPluginUid = aIptvPluginUid.iUid;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::GetIptvPluginUid()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CIptvService::GetIptvPluginUid()
    {
    TUid uid;
    uid.iUid = iIptvPluginUid;

    return uid;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetType()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetType( TServiceType aType )
    {
    iType = aType;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetType()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvService::TServiceType CIptvService::GetType()
    {
    return ( CIptvService::TServiceType )iType;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetIapList()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetIapListL( CIptvIapList& aIapList )
    {
    if ( aIapList.Count() > KIptvSmServicesDbMaxIaps )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvService:: SetIapListL() Max IAP count exceeded, aborting");
        return KErrArgument;
        }
        
    iIapList->SetL( aIapList );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetIapList()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvIapList& CIptvService::GetIapList()
    {
    return *iIapList;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetUserName()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetUserName( const TDesC16& aUserName )
    {
    if ( aUserName.Length() > KIptvSmPasswordsDbUserNameMaxLength )
        {
        return KErrNoMemory;
        }
    
    iUserName = aUserName;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetUserName()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TDes16& CIptvService::GetUserName()
    {
    return iUserName;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetPassword()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetPassword( const TDesC16& aPassword )
    {
    if ( aPassword.Length() > KIptvSmPasswordsDbPasswordMaxLength )
        {
        return KErrNoMemory;
        }

    iPassword = aPassword;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetPassword()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TDes16& CIptvService::GetPassword()
    {
    return iPassword;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetDesc()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetDesc( const TDesC16& aDesc )
    {
    if ( aDesc.Length() > KIptvSmServicesDbDescMaxLength )
        {
        return KErrNoMemory;
        }
    
    iDesc = aDesc;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetDesc()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TDes16& CIptvService::GetDesc()
    {
    return iDesc;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetFlags()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetFlags( TUint32 aFlags )
    {
    iFlags = aFlags;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::GetFlags()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvService::GetFlags()
    {
    return iFlags;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::SetApplicationUid()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetApplicationUid( TInt32 aUid )
    {
    iApplicationUid = aUid;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::GetApplicationUid()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CIptvService::GetApplicationUid()
    {
    return iApplicationUid;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::SetDisplayOrder()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetDisplayOrder( TUint32 aOrder )
    {
    iDisplayOrder = aOrder;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::GetDisplayOrder()
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C TUint32 CIptvService::GetDisplayOrder()
    {
    return iDisplayOrder;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetProviderId()
// 
// -----------------------------------------------------------------------------
//    

EXPORT_C TInt CIptvService::SetProviderId( const TDesC& aProviderId )
    {
    if ( aProviderId.Length() > KIptvSmServicesDbProviderIdMaxLength )
        {
        return KErrOverflow;
        }
    
    iProviderId = aProviderId;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetProviderId()
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C TDesC& CIptvService::GetProviderId()
    {
    return iProviderId;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetAccountManagementUrl()
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C TInt CIptvService::SetAccountManagementUrl(
    const TDesC& aAccountManagementUrl )
    {
    if ( aAccountManagementUrl.Length() >
         KIptvSmServicesDbAccountManagementUrlMaxLength )
        {
        return KErrOverflow;
        }
    
    iAccountManagementUrl = aAccountManagementUrl;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetAccountManagementUrl()
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C TDesC& CIptvService::GetAccountManagementUrl()
    {
    return iAccountManagementUrl;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetEpgUpdateTimeL()
//
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvService::SetEpgUpdateTimeL( const TTime& aTime )
    {
    iEpgUpdateTime = aTime;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetEpgUpdateTimeL()
//
// -----------------------------------------------------------------------------
//    
EXPORT_C TTime& CIptvService::GetEpgUpdateTimeL()
    {
    return iEpgUpdateTime;
    }
        
// -----------------------------------------------------------------------------
// CIptvService::SetIapNameL()
//
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvService::SetIapNameL( const TDesC& aIapName )
    {
    if ( aIapName.Length() > KIptvSmServicesDbIapNameMaxLength )
        {
        User::Leave( KErrOverflow );
        }
    
    iIapName = aIapName;    
    }

// -----------------------------------------------------------------------------
// CIptvService::GetIapNameL()
//
// -----------------------------------------------------------------------------
//    
EXPORT_C const TDesC& CIptvService::GetIapNameL()
    {
    return iIapName;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetSearchUrl()
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetSearchUrlL( const TDesC& aSearchUrl )
    {
    if ( aSearchUrl.Length() > KIptvSmServicesDbSearchUrlMaxLength )
        {
        User::Leave( KErrOverflow );
        }
    
    delete iSearchUrl;
    iSearchUrl = NULL;
    iSearchUrl = aSearchUrl.AllocL();
    }

// -----------------------------------------------------------------------------
// CIptvService::SearchUrl()
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIptvService::SearchUrl() const
    {
    return *iSearchUrl;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetRecordUrlL()
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetRecordUrlL( const TDesC& aRecordUrl )
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvService::SetRecordUrlL(): aRecordUrl = %S", &aRecordUrl);
        
    if ( aRecordUrl.Length() > KIptvSmServicesDbRecordUrlMaxLength )
        {
        IPTVLOGSTRING_LOW_LEVEL ("Record URL is too long -> User::Leave( KErrOverflow");        
        User::Leave( KErrOverflow );
        }
    
    delete iRecordUrl;
    iRecordUrl = NULL;
    iRecordUrl = HBufC::NewL( aRecordUrl.Length() );
    iRecordUrl->Des() = aRecordUrl;     
    }

// -----------------------------------------------------------------------------
// CIptvService::RecordUrl()
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIptvService::RecordUrl() const
    {
    return *iRecordUrl;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetScheduleDlNetwork()
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetScheduleDlNetwork( TUint aScheduleDlNetwork )
    {
    iScheduleDownloadNetwork = aScheduleDlNetwork;
    }

// -----------------------------------------------------------------------------
// CIptvService::ScheduleDlNetwork()
//
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CIptvService::ScheduleDlNetwork() const
    {
    return iScheduleDownloadNetwork;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetScheduleDlTime()
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetScheduleDlTime( TUint aScheduleDlTime )
    {
    iScheduleDownloadTime = aScheduleDlTime;
    }

// -----------------------------------------------------------------------------
// CIptvService::ScheduleDlTime()
//
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CIptvService::ScheduleDlTime() const
    {
    return iScheduleDownloadTime;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetScheduleDlType()
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetScheduleDlType( TUint aScheduleDlType )
    {
    iScheduleDownloadType = aScheduleDlType;
    }

// -----------------------------------------------------------------------------
// CIptvService::ScheduleDlType()
//
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CIptvService::ScheduleDlType() const
    {
    return iScheduleDownloadType;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetLastScheduledDownloadTimeL
//
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvService::SetScheduledLastDownloadTime( const TTime& aTime )
    {
    iLastScheduledDownloadTime = aTime;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetLastScheduledDownloadTimeL
//
// -----------------------------------------------------------------------------
//    
EXPORT_C const TTime& CIptvService::GetScheduledLastDownloadTime() const
    {
    return iLastScheduledDownloadTime;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetGroupId()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvService::SetGroupId( TUint32 aGroupId )
    {
    iGroupId = aGroupId;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetGroupId()
// 
// -----------------------------------------------------------------------------
//    
EXPORT_C TUint32 CIptvService::GetGroupId()
    {
    return iGroupId;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::SetUseCategory()
//
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvService::SetUseCategory( const TBool& aUseCategory )
    {
    iUseCategory = ( TUint8 )aUseCategory;    
    }

// -----------------------------------------------------------------------------
// CIptvService::GetUseCategory()
//
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool CIptvService::GetUseCategory()
    {
    return ( iUseCategory != 0 );
    }

// -----------------------------------------------------------------------------
// CIptvService::SetUploadProvider()
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetUploadProvider( const TDesC& aProvider )
    {
    if ( aProvider.Length() > KIptvSmServicesDbNameMaxLength )
        {
        return KErrNoMemory;
        }
        
    iUploadProvider = aProvider;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetUploadProvider()
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CIptvService::GetUploadProvider() const
    {
    return iUploadProvider;
    }

// -----------------------------------------------------------------------------
// CIptvService::SetIconUrl()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetIconUrl( const TDesC16& aAddress )
    {
    if ( aAddress.Length() > KIptvSmServicesDbAddressMaxLength )
        {
        return KErrNoMemory;
        }
        
    iIconUrl = aAddress;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvService::GetIconUrl()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TDes16& CIptvService::GetIconUrl()
    {
    return iIconUrl;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::Set()
// This format is used when sending service data over IPC of when handling
// arrays of services.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvService::SetL( const TDesC8& aService )
    {
    RDesReadStream readStream;
    readStream.Open(aService);
    readStream.PushL(); // 1->
    InternalizeL(readStream);
    readStream.Pop(); // <-1
    readStream.Close();
        
    return KErrNone; //successful
    }

// -----------------------------------------------------------------------------
// CIptvService::GetL()
// This format is used when sending service data over IPC and handling
// arrays of services.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CIptvService::GetL()
    {
    HBufC8* serviceDes;
    serviceDes = HBufC8::NewL( CountSize() + 1 );
    CleanupStack::PushL( serviceDes ); // 1->
    
    RDesWriteStream writeStream;
    TPtr8 servicePtr8(serviceDes->Des());
    writeStream.Open(servicePtr8);
    writeStream.PushL(); // 2->
    ExternalizeL( writeStream );
    writeStream.Pop(); // <-2
    writeStream.Close();
    
    CleanupStack::Pop(serviceDes); // <-1 //ownership moves to caller
    return serviceDes;
    }
    
// -----------------------------------------------------------------------------
// CIptvService::CountSize()
// Returns binary descriptor size with current member values.
// Size is in 8-bit bytes.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvService::CountSize()
    {
    return CountExternalizeSize();
    }

// -----------------------------------------------------------------------------
// CIptvService::InternalizeL()
// -----------------------------------------------------------------------------
//    
EXPORT_C void CIptvService::InternalizeL( RDesReadStream& aStream )
    {
    iId = aStream.ReadInt32L();

    CIptvUtil::ReadDesFromStreamL( iName, aStream );    
    CIptvUtil::ReadDesFromStreamL( iAddress, aStream );
    CIptvUtil::ReadDesFromStreamL( iIconPath, aStream );

    iEpgPluginUid  = aStream.ReadInt32L();
    iVodPluginUid  = aStream.ReadInt32L();
    iIptvPluginUid = aStream.ReadInt32L();

    iType = ( TServiceType )aStream.ReadInt16L();

    iIapList->InternalizeL( aStream );    

    CIptvUtil::ReadDesFromStreamL( iUserName, aStream );
    CIptvUtil::ReadDesFromStreamL( iPassword, aStream );
    CIptvUtil::ReadDesFromStreamL( iDesc, aStream );

    iFlags = aStream.ReadInt32L();
    iApplicationUid = aStream.ReadInt32L();
    iDisplayOrder = aStream.ReadInt32L();    

    CIptvUtil::ReadDesFromStreamL( iProviderId, aStream );
    CIptvUtil::ReadDesFromStreamL( iAccountManagementUrl, aStream );

    //iEpgUpdateTime
    TUint low  = static_cast<TUint>( aStream.ReadUint32L() );
    TUint high = static_cast<TUint>( aStream.ReadUint32L() );
    TInt64 time;
    time = high;
    time = time << 32;
    time |= low;
    iEpgUpdateTime = time;
    
    CIptvUtil::ReadDesFromStreamL( iIapName, aStream );

    // Fill iSearchUrl, can't use ReadDesFromStreamL since we need to know length for HBufC creation
    TInt length( aStream.ReadUint16L() );
    delete iSearchUrl;
    iSearchUrl = NULL;
    iSearchUrl = HBufC::NewL( length );
    TPtr searchUrlPtr( iSearchUrl->Des() );
    for ( TInt i = 0; i < length; i++ )
        {
        searchUrlPtr.Append( aStream.ReadUint16L() );
        }
        
    iScheduleDownloadNetwork = aStream.ReadInt32L();
    iScheduleDownloadTime = aStream.ReadInt32L();
    iScheduleDownloadType = aStream.ReadInt32L();

    low  = static_cast<TUint>( aStream.ReadUint32L() );
    high = static_cast<TUint>( aStream.ReadUint32L() );
    time = high;
    time = time << 32;
    time |= low;
    iLastScheduledDownloadTime = time;

    iGroupId = aStream.ReadInt32L();
    iUseCategory = aStream.ReadInt8L();
    CIptvUtil::ReadDesFromStreamL( iUploadProvider, aStream );
    CIptvUtil::ReadDesFromStreamL( iIconUrl, aStream );
    
    // Fill iRecordUrl, can't use ReadDesFromStreamL since we need to 
    // know length for HBufC creation
    length = aStream.ReadUint16L();
    delete iRecordUrl;
    iRecordUrl = NULL;
    iRecordUrl = HBufC::NewL( length );
    TPtr recordUrlPtr( iRecordUrl->Des() );
    for ( TInt i = 0; i < length; i++ )
        {
        recordUrlPtr.Append( aStream.ReadUint16L() );
        }
    }

// -----------------------------------------------------------------------------
// CIptvService::ExternalizeL()
// -----------------------------------------------------------------------------
//        
EXPORT_C void CIptvService::ExternalizeL( RDesWriteStream& aStream ) const
    {    
    aStream.WriteInt32L( iId );

    CIptvUtil::WriteDesToStreamL( iName, aStream );
    CIptvUtil::WriteDesToStreamL( iAddress, aStream );
    CIptvUtil::WriteDesToStreamL( iIconPath, aStream );
                
    aStream.WriteInt32L( iEpgPluginUid );
    aStream.WriteInt32L( iVodPluginUid );
    aStream.WriteInt32L( iIptvPluginUid );

    aStream.WriteInt16L( iType );

    iIapList->ExternalizeL( aStream );

    CIptvUtil::WriteDesToStreamL( iUserName, aStream );
    CIptvUtil::WriteDesToStreamL( iPassword, aStream );
    CIptvUtil::WriteDesToStreamL( iDesc, aStream );

    aStream.WriteInt32L( iFlags );
    aStream.WriteInt32L( iApplicationUid );
    aStream.WriteInt32L( iDisplayOrder );    

    CIptvUtil::WriteDesToStreamL( iProviderId, aStream );
    CIptvUtil::WriteDesToStreamL( iAccountManagementUrl, aStream );
    
    aStream.WriteUint32L( iEpgUpdateTime.Int64() & 0x00000000FFFFFFFFULL );
    aStream.WriteUint32L( ( iEpgUpdateTime.Int64() & 0xFFFFFFFF00000000ULL ) >> 32 );
    
    CIptvUtil::WriteDesToStreamL( iIapName, aStream );

    TPtr searchUrlPtr( iSearchUrl->Des() );
    CIptvUtil::WriteDesToStreamL( searchUrlPtr, aStream );
        
    aStream.WriteInt32L( iScheduleDownloadNetwork );
    aStream.WriteInt32L( iScheduleDownloadTime );
    aStream.WriteInt32L( iScheduleDownloadType );
    aStream.WriteUint32L( 
        iLastScheduledDownloadTime.Int64() & 0x00000000FFFFFFFFULL );
    aStream.WriteUint32L( 
        ( iLastScheduledDownloadTime.Int64() & 0xFFFFFFFF00000000ULL ) >> 32 );

    aStream.WriteInt32L( iGroupId );
    aStream.WriteInt8L( iUseCategory );
    CIptvUtil::WriteDesToStreamL( iUploadProvider, aStream );
    CIptvUtil::WriteDesToStreamL( iIconUrl, aStream );
    TPtr recordUrlPtr( iRecordUrl->Des() );
    CIptvUtil::WriteDesToStreamL( recordUrlPtr, aStream );
    }

// -----------------------------------------------------------------------------    
// CIptvService::CountExternalizeSize()
// Return externalize size in 8bit bytes.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CIptvService::CountExternalizeSize()
    {
    return   4   //TUint32 iId                          // 1
           + 2 + ( iName.Length() * 2 )                 // 2
           + 2 + ( iAddress.Length() * 2 )              // 3
           + 2 + ( iIconPath.Length() * 2 )             // 4
           + 4   //TUint32 iEpdPluginUid                // 5
           + 4   //TUint32 iVodPlugingUid               // 6
           + 4   //TUint32 iIptvPluginUid               // 7
           + 2   //TUint16 iType                        // 8
           + ( iIapList->CountExternalizeSize() )       // 9
           + 2 + ( iUserName.Length() * 2 )             // 10
           + 2 + ( iPassword.Length() * 2 )             // 11
           + 2 + ( iDesc.Length() * 2 )                 // 12
           + 4   //TUint32 iFlags                       // 13
           + 4   //TInt32  iApplicationUid              // 14
           + 4   //TUint32 iDisplayOrder                // 15    
           + 2 + ( iProviderId.Length() * 2 )           // 16
           + 2 + ( iAccountManagementUrl.Length() * 2 ) // 17
           + 8 + //TTime iEpgUpdateTime                 // 18
           + 2 + ( iIapName.Length() * 2 )              // 19
           + 2 + ( iSearchUrl->Length() * 2 )           // 20
           + 4   //TUint iScheduleDownloadNetwork       // 21
           + 4   //TUint iScheduleDownloadTime          // 22
           + 4   //TUint iScheduleDownloadType          // 23
           + 8   //TTime iLastScheduledDownloadTime     // 24
           + 4 + //TUint32 iGroupId                     // 25
           + 1 + //TUint8 iUseCategory                  // 26
           + 2 + ( iUploadProvider.Length() * 2 )       // 27
           + 2 + ( iIconUrl.Length() * 2 )              // 28
           + 2 + ( iRecordUrl->Length() * 2 );          // 29
    }

// -----------------------------------------------------------------------------    
// CIptvService::CompareL()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CIptvService::CompareL( CIptvService& aService )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvService::CompareL() start" );
    
    HBufC8* serviceStreamed = aService.GetL();
    CleanupStack::PushL( serviceStreamed ); // 1->
    HBufC8* thisServiceStreamed = GetL();
    CleanupStack::PushL( thisServiceStreamed ); // 2->

    TBool result( EFalse );
    if ( serviceStreamed->Des() == thisServiceStreamed->Des() )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvService:: services are equal");
        result = ETrue;
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvService:: services are not equal");
        result = EFalse;
        }
           
    CleanupStack::PopAndDestroy( thisServiceStreamed ); // <-2
    CleanupStack::PopAndDestroy( serviceStreamed );    // <-1
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvService::CompareL() exit");

    return result;
    }

// -----------------------------------------------------------------------------    
// CIptvService::SecureProvisioningUrlParametersL()
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CIptvService::SecureProvisioningUrlParametersL()
    {
    CIptvSecureProvisioningParameters* spp = CIptvSecureProvisioningParameters::NewL();
    CleanupStack::PushL( spp ); // 1->
    
    HBufC* parameters = spp->SecureProvisioningUrlParametersL();
    CleanupStack::PopAndDestroy( spp ); // <-1
    return parameters;
    }

