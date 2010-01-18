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
* Description:  Settings model stub for STIF testing
*
*/

// Version : %version: ou1cpsw#4 %


#include "mpsettingsmodelimp_stub.h"
#include "mpxvideo_debug.h"


CMPSettingsModel* CMPSettingsModel::NewL( TUid /*aUid*/ )
{
    MPX_ENTER_EXIT(_L("CMPSettingsModel::NewL()"));

    CMPSettingsModel* self = new (ELeave) CMPSettingsModelImp();
    return self;
}

CMPSettingsModel::~CMPSettingsModel()
{
    MPX_ENTER_EXIT(_L("CMPSettingsModel::~CMPSettingsModel()"));
}

CMPSettingsModelImp::CMPSettingsModelImp()
{
    MPX_ENTER_EXIT(_L("CMPSettingsModelImp::CMPSettingsModelImp()"));
}

CMPSettingsModelImp::~CMPSettingsModelImp()
{
    MPX_ENTER_EXIT(_L("CMPSettingsModelImp::~CMPSettingsModelImp()"));
}

void CMPSettingsModelImp::LoadSettingsL( TInt /*aConfigVersion*/ )
{
    MPX_ENTER_EXIT(_L("CMPSettingsModelImp::LoadSettingsL()"));
}

TInt CMPSettingsModelImp::GetDefaultAp( TUint32& aApId )
{
    MPX_ENTER_EXIT(_L("CMPSettingsModelImp::GetDefaultAp()"));

    _LIT( KDefaultAccessPointFilename, "c:\\defaultApId.txt" );

    RFs fs;
    RFile file;

    TInt err = fs.Connect();

    err = file.Open( fs, KDefaultAccessPointFilename, EFileRead | EFileShareAny );

    TInt apId;

    HBufC8* fBuf = HBufC8::NewL( 16 );
    TPtr8 fileBuf = fBuf->Des();

    file.Read( fileBuf, 16 );

    TLex8 number( fileBuf );
    number.Val( apId );

    file.Close();
    fs.Close();

    aApId = apId;

    MPX_DEBUG(_L("CMPSettingsModelImp::GetDefaultAp(%d)"), aApId);

    return KErrNone;
}

void CMPSettingsModelImp::StoreSettingsL()
{
}

TInt CMPSettingsModelImp::SetVideoContrast( TInt /*aContrast*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetVideoContrast( TInt& /*aContrast*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetProxyMode( TInt /*aMode*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetProxyMode( TInt& /*aMode*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetProxyHostNameL( const TDesC& /*aHostName*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetProxyHostName( TDes& /*aHostName*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetProxyPort( TInt /*aPort*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetProxyPort( TInt& /*aPort*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetDefaultAp( TUint32 /*aApId*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetBandwidthControlMode( TInt /*aMode*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetBandwidthControlMode( TInt& /*aMode*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetMaxBandwidth( TInt /*aMaxBw*/, TDataBearer /*aBearer*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetMaxBandwidth( TInt& /*aMaxBw*/, TDataBearer /*aBearer*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetConnectionTimeout( TInt /*aTimeout*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetConnectionTimeout( TInt& /*aTimeout*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetServerTimeout( TInt /*aTimeout*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetServerTimeout( TInt& /*aTimeout*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetMinUDPPort( TInt /*aPort*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetMinUDPPort( TInt& /*aPort*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetMaxUDPPort( TInt /*aPort*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetMaxUDPPort( TInt& /*aPort*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetControllerVersionInfo( TDes& /*aVersion*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetControllerBuildDate( TDes& /*aBldDate*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetControllerAdditionalInfo( TDes& /*aAdditionalInfo*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetDemandBwFactor( TInt /*aFactor*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetDemandBwFactor( TInt& /*aFactor*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::SetSustainBandwidth( TInt /*aSustainBw*/, TDataBearer /*aBearer*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetSustainBandwidth( TInt& /*aSustainBw*/, TDataBearer /*aBearer*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetSustainBwPresetsL( RArray<TInt>& /*aBwArray*/, TDataBearer /*aBearer*/ )
{
    return KErrNone;
}

TInt CMPSettingsModelImp::GetMaxBwPresetsL( RArray<TInt>& /*aBwArray*/, TDataBearer /*aBearer*/ )
{
    return KErrNone;
}

TBool CMPSettingsModelImp::IsVideoRepeatOnL()
{
    return EFalse;
}

void CMPSettingsModelImp::SetVideoRepeatL( const TBool /*aRepeat*/ )
{
}

TBool CMPSettingsModelImp::IsDefaultViewOnL()
{
    return EFalse;
}

void CMPSettingsModelImp::SetDefaultViewL( const TBool /*aView*/ )
{
}

TBool CMPSettingsModelImp::IsRockerKeysSupportedL()
{
    return EFalse;
}

void CMPSettingsModelImp::SetRockerKeysL( const TBool /*aRockerKeys*/ )
{
}

TBool CMPSettingsModelImp::ShowRockerKeysL()
{
    return EFalse;
}

void CMPSettingsModelImp::SetAutoDisconTimeL( const TInt /*aTime*/ )
{
}

TInt CMPSettingsModelImp::AutoDisconTimeL()
{
    return KErrNone;
}

