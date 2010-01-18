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
* Description:  Stubs for Access Point selection for STIF testing
*
*/


// Version : %version: ou1cpsw#2 %


#include <commsdattypesv1_1.h>
#include <metadatabase.h>
#include <cmmanagerext.h>
#include <elements/metacontainer.h>
#include <cmpluginwlandef.h>
#include <f32file.h>
#include "mpxvideo_debug.h"

static TBool wlanAccessPoint = EFalse;

RCmConnectionMethodExt::RCmConnectionMethodExt()
{
}

RCmConnectionMethodExt::~RCmConnectionMethodExt()
{
}

TUint32 RCmConnectionMethodExt::GetIntAttributeL( TUint32 /*aAttribute*/ ) const
{
    MPX_DEBUG(_L("RCmConnectionMethodExt::GetIntAttributeL(%d)"), wlanAccessPoint);

    TUint32 bearer( 0 );

    if ( wlanAccessPoint )
    {
        bearer = KUidWlanBearerType;
    }

    return bearer;
}

void
RCmManagerExt::OpenL()
{
}

void
RCmManagerExt::Close()
{
}


RCmConnectionMethodExt
RCmManagerExt::ConnectionMethodL( TUint32 aConnectionMethodId )
{
    MPX_DEBUG(_L("RCmManagerExt::ConnectionMethodL(%d)"), aConnectionMethodId);

    RCmConnectionMethodExt connMethod;

    if ( aConnectionMethodId == 5 || aConnectionMethodId == 6 )
    {
        //
        //  WLan Access Points
        //
        wlanAccessPoint = ETrue;
    }

    return connMethod;
}

CBase::CBase( CBase const & /*a*/ )
{
}

namespace CommsDat
{

using namespace Meta;


static const SRecordTypeInfo bearerRecord = { 0, 0, ENoAttrs, KCDNull };

const SRecordTypeInfo* const CCDWAPIPBearerRecord::iRecordInfo = &bearerRecord;


CMDBTextFieldBase::CMDBTextFieldBase()
{
}

CMDBTextFieldBase::~CMDBTextFieldBase()
{
}

TVersion
CMDBSession::LatestVersion()
{
    return TVersion( 1, 1, 1 );
}

CMDBSession::CMDBSession()
{
}

CMDBSession::~CMDBSession()
{
}

CMDBSession* 
CMDBSession::NewL( TVersion /*aRequiredVersion*/ )
{
    CMDBSession* self = new( ELeave ) CMDBSession();

    return self;
}

CCDRecordBase::CCDRecordBase( TMDBElementId /*aElementId*/ )
{
}

CCDWAPIPBearerRecord::CCDWAPIPBearerRecord( TMDBElementId aElementId )
    : CCDRecordBase( aElementId )
{
}

TBool
MMetaDatabase::FindL( CMDBSession& /*aSession*/ )
{
    CCDWAPIPBearerRecord* br = static_cast<CCDWAPIPBearerRecord*>(this);
    br->iWAPIAP = br->iWAPAccessPointId;

    return ETrue;
}

CMDBRecordBase* 
CCDRecordBase::RecordFactoryL( TMDBElementId /*aInput*/ )
{
    TMDBElementId elementId(0);

    CCDWAPIPBearerRecord* recordBase = new( ELeave ) CCDWAPIPBearerRecord( elementId );

    return recordBase;
}

CMDBNumFieldBase::CMDBNumFieldBase()
{
}

CMDBRecordLinkBase::CMDBRecordLinkBase()
{
}

MMetaDatabase::MMetaDatabase()
{
}

MMetaDatabase::~MMetaDatabase()
{
}

CMDBElement::CMDBElement()
{
}

CMDBElement::~CMDBElement()
{
}

CMDBRecordLinkBase::~CMDBRecordLinkBase()
{
}

CMDBElement* 
CMDBRecordBase::GetFieldByIdL( TMDBElementId /*aId*/ )
{
    CMDBNumFieldBase* element = new( ELeave ) CMDBNumFieldBase();
    return element;
}

CMDBElement* 
CommsDat::CMDBRecordBase::GetFieldByNameL( const TPtrC& /*aFieldName*/, TInt& /*aValType*/ )
{
    CMDBNumFieldBase* element = new( ELeave ) CMDBNumFieldBase();
    return element;
}

SVDataTableEntry const * 
CCDWAPIPBearerRecord::GetVDataTable() const
{
    SVDataTableEntry* entry = NULL;

    return entry;
}

TUint8* 
CCDWAPIPBearerRecord::GetAttribPtr( const TInt /*aOffset*/ ) const
{
    TUint8* ptr = new( ELeave ) TUint8();
    return ptr;
}

SVDataTableEntry const * 
CMDBNumFieldBase::GetVDataTable() const
{
    SVDataTableEntry* entry = NULL;

    return entry;
}

TUint8* 
CMDBNumFieldBase::GetAttribPtr( const TInt /*aOffset*/ ) const
{
    TUint8* ptr = new( ELeave ) TUint8();
    return ptr;
}

SVDataTableEntry const * 
CMDBRecordLinkBase::GetVDataTable() const
{
    SVDataTableEntry* entry = NULL;

    return entry;
}

TUint8* 
CMDBRecordLinkBase::GetAttribPtr( const TInt /*aOffset*/ ) const
{
    TUint8* ptr = new( ELeave ) TUint8();
    return ptr;
}

CMDBRecordLinkBase& 
CMDBRecordLinkBase::operator=( const TMDBElementId aValue ) 
{
    iValue = (TAny*)aValue;
    
    // iElementId |= (KCDChangedFlag | KCDNotNullFlag);
            
    return *this;
}

CMDBRecordLinkBase::operator TMDBElementId()
{ 
    return (TMDBElementId&)iValue; 
}

SVDataTableEntry const * 
CCDRecordBase::GetVDataTable() const
{
    SVDataTableEntry* entry = NULL;

    return entry;
}

TUint8* 
CCDRecordBase::GetAttribPtr( const TInt /*aOffset*/ ) const
{
    TUint8* ptr = new( ELeave ) TUint8();
    return ptr;
}

const SRecordTypeInfo* 
CMDBRecordBase::GetRecordInfo()
{
    SRecordTypeInfo* typeInfo = NULL;
    return typeInfo;
}

SVDataTableEntry const * 
CMDBTextFieldBase::GetVDataTable() const
{
    SVDataTableEntry* entry = NULL;

    return entry;
}

TUint8* 
CMDBTextFieldBase::GetAttribPtr( const TInt /*aOffset*/ ) const
{
    TUint8* ptr = new( ELeave ) TUint8();
    return ptr;
}

SVDataTableEntry const * 
CMDBElement::GetVDataTable() const
{
    SVDataTableEntry* entry = NULL;

    return entry;
}

TUint8* 
CMDBElement::GetAttribPtr( const TInt /*aOffset*/ ) const
{
    TUint8* ptr = new( ELeave ) TUint8();
    return ptr;
}

}  // namespace CommsDat


namespace Meta
{

SMetaData::SMetaData()
{
}

SMetaData::~SMetaData()
{
}

SMetaDataECom::SMetaDataECom()
{
}

SMetaDataECom::~SMetaDataECom()
{
}

SMetaDataECom* 
SMetaDataECom::LoadL( TPtrC8& /*aDes*/ )
{
    SMetaDataECom* data = NULL;
    return data;
}

RMetaDataContainerBase::RMetaDataContainerBase()
{
}

} // namespace Meta


#include <es_sock.h>

_LIT( KQueryAccessPointFilename, "c:\\queryApId.txt" );

TInt
ReadApIdFromFile()
{
    RFs fs;
    RFile file;

    TInt err = fs.Connect();

    err = file.Open( fs, KQueryAccessPointFilename, EFileRead | EFileShareAny );

    TInt apId(0);

    HBufC8* fBuf = HBufC8::NewL( 16 );
    TPtr8 fileBuf = fBuf->Des();

    file.Read( fileBuf, 16 );

    TLex8 number( fileBuf );
    number.Val( apId );

    file.Close();
    fs.Close();

    return apId;
}

void
WriteApIdToFile( TInt aNewValue )
{
    RFile file;

    RFs fs;
    User::LeaveIfError( fs.Connect() );

    TBuf8<16> tgt;
    tgt.Num( aNewValue );

    file.Replace( fs, KQueryAccessPointFilename, EFileWrite );

    file.Write( tgt );

    file.Close();
    fs.Close();
}

TInt
RConnection::Open( RSocketServ& /*aSocketServer*/, TUint /*aConnectionType*/ )
{
    return KErrNone;
}
    
TInt
RConnection::Start( TConnPref& /*aPref*/ )
{
    TInt retVal = KErrNone;

    TInt apId = ReadApIdFromFile();

    if ( apId == 0 )
    {
        WriteApIdToFile( KErrCancel );
        retVal = KErrNotFound;
    }
    else if ( apId == 8 )
    {
        WriteApIdToFile( 9 );
        retVal = KErrNotFound;
    }
    else if ( apId == KErrCancel )
    {
        retVal = KErrCancel;
    }

    MPX_DEBUG( _L("RConnection::Start(%d)"), retVal );

    return retVal;
}
    
void
RConnection::Close()
{
}

RConnection::RConnection()
    : iNewISPId(0)
{
}

RConnection::~RConnection()
{
}

TInt 
RConnection::GetIntSetting( const TDesC& /*aSettingName*/, TUint32& aValue )
{
    MPX_ENTER_EXIT(_L("RConnection::GetIntSetting()"));

    TInt apId = ReadApIdFromFile();

    MPX_DEBUG( _L("RConnection::GetIntSetting(%d)"), apId );

    aValue = apId;

    return KErrNone;
}


#include <connpref.h>

TConnPref::TConnPref()
{
}

TConnPrefList::TConnPrefList()
{
}

TConnPrefList::~TConnPrefList()
{
}

void 
TConnPrefList::AppendL( SMetaDataECom* /*aFamily*/ )
{
}



#include <extendedconnpref.h>

TExtendedConnPref::TExtendedConnPref()
{
}

void 
TExtendedConnPref::SetSnapPurpose( CMManager::TSnapPurpose /*aSnapPurpose*/ )
{
}

void 
TExtendedConnPref::SetNoteBehaviour( TUint32 /*aNoteBehaviour*/ )
{
}

void 
TExtendedConnPref::SetConnSelectionDialog( TBool /*aConnSelectionDialog*/ )
{
}

TUint8* 
TExtendedConnPref::GetAttribPtr( const TInt /*aOffset*/ ) const
{
    TUint8* ptr = new( ELeave ) TUint8();
    return ptr;
}

SVDataTableEntry const * 
TExtendedConnPref::GetVDataTable() const
{
    SVDataTableEntry* entry = NULL;

    return entry;
}



RSocketServ::RSocketServ()
{
}

TInt
RSocketServ::Connect( TUint /*aMessageSlots*/ )
{
    return KErrNone;
}


// EOF
