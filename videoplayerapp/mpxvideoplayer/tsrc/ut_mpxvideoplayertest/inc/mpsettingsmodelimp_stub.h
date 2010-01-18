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

// Version : %version: ou1cpsw#2 %

#ifndef MPSETTINGSMODELIMP_STUB_H_
#define MPSETTINGSMODELIMP_STUB_H_

#include <MPSettingsModel.h>

NONSHARABLE_CLASS( CMPSettingsModelImp ) : public CMPSettingsModel
{
    public:

        CMPSettingsModelImp();
        ~CMPSettingsModelImp();

        virtual void LoadSettingsL(TInt aConfigVersion);
        virtual void StoreSettingsL();
        virtual TInt SetVideoContrast(TInt aContrast);
        virtual TInt GetVideoContrast(TInt& aContrast);
        virtual TInt SetProxyMode(TInt aMode);
        virtual TInt GetProxyMode(TInt& aMode);
        virtual TInt SetProxyHostNameL(const TDesC& aHostName);
        virtual TInt GetProxyHostName(TDes& aHostName);
        virtual TInt SetProxyPort(TInt aPort);
        virtual TInt GetProxyPort(TInt& aPort);
        virtual TInt SetDefaultAp(TUint32 aApId);
        virtual TInt GetDefaultAp(TUint32& aApId);
        virtual TInt SetBandwidthControlMode(TInt aMode);
        virtual TInt GetBandwidthControlMode(TInt& aMode);
        virtual TInt SetMaxBandwidth(TInt aMaxBw, TDataBearer aBearer);
        virtual TInt GetMaxBandwidth(TInt& aMaxBw, TDataBearer aBearer);
        virtual TInt SetConnectionTimeout(TInt aTimeout);
        virtual TInt GetConnectionTimeout(TInt& aTimeout);
        virtual TInt SetServerTimeout(TInt aTimeout);
        virtual TInt GetServerTimeout(TInt& aTimeout);
        virtual TInt SetMinUDPPort(TInt aPort);
        virtual TInt GetMinUDPPort(TInt& aPort);
        virtual TInt SetMaxUDPPort(TInt aPort);
        virtual TInt GetMaxUDPPort(TInt& aPort);
        virtual TInt GetControllerVersionInfo(TDes& aVersion);
        virtual TInt GetControllerBuildDate(TDes& aBldDate);
        virtual TInt GetControllerAdditionalInfo(TDes& aAdditionalInfo);
        virtual TInt SetDemandBwFactor(TInt aFactor);
        virtual TInt GetDemandBwFactor(TInt& aFactor);
        virtual TInt SetSustainBandwidth(TInt aSustainBw, TDataBearer aBearer);
        virtual TInt GetSustainBandwidth(TInt& aSustainBw, TDataBearer aBearer);
        virtual TInt GetSustainBwPresetsL(RArray<TInt>& aBwArray, TDataBearer aBearer);
        virtual TInt GetMaxBwPresetsL(RArray<TInt>& aBwArray, TDataBearer aBearer);
        virtual TBool IsVideoRepeatOnL();
        virtual void SetVideoRepeatL(const TBool aRepeat);
        virtual TBool IsDefaultViewOnL();
        virtual void SetDefaultViewL(const TBool aView);
        virtual TBool IsRockerKeysSupportedL();
        virtual void SetRockerKeysL(const TBool aRockerKeys);
        virtual TBool ShowRockerKeysL();
        virtual void SetAutoDisconTimeL(const TInt aTime);
        virtual TInt AutoDisconTimeL();
};

#endif /*MPSETTINGSMODELIMP_STUB_H_*/
