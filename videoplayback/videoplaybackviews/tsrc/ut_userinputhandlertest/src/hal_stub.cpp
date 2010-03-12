/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of HAL stub for STIF
*
*/

// Version : %version: 3 %


// INCLUDE FILES
#include <e32std.h>

#include "hal_stub.h"
#include "mpxvideo_debug.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CHWRMLight::CHWRMLight()
// C++ default constructor can NOT contain any code, that
// might leave.
// -------------------------------------------------------------------------------------------------
//
CHWRMLight::CHWRMLight()
{
}

CHWRMLight::~CHWRMLight()
{
    MPX_DEBUG(_L("CHWRMLight::~CHWRMLight()"));
}

CHWRMLight* CHWRMLight::NewL()
{
    MPX_DEBUG(_L("CHWRMLight::NewL()"));

    CHWRMLight* self = new (ELeave) CHWRMLight();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

void CHWRMLight::ConstructL()
{
	 MPX_DEBUG(_L("CHWRMLight::ConstructL()"));
}

void CHWRMLight::ReserveLightL(TInt aTarget)
{
	MPX_DEBUG(_L("CHWRMLight::ReserveLightL()"));
}

void CHWRMLight::ReleaseLight(TInt aTarget)
{
	MPX_DEBUG(_L("CHWRMLight::ReleaseLight()"));
}

void CHWRMLight::LightOnL(TInt aTarget)
{
	MPX_DEBUG(_L("CHWRMLight::LightOnL()"));
	iBacklightState = ELightOn;	
}
void CHWRMLight::LightOffL(TInt aTarget)
{
	MPX_DEBUG(_L("CHWRMLight::LightOffL()"));
	iBacklightState = ELightOff;
}

TLightStatus CHWRMLight::LightStatus(TInt aTarget) const
{
	MPX_DEBUG(_L("CHWRMLight::LightStatus()"));
	return iBacklightState;
}


// End of File
