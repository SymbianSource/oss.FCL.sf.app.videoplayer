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
 * Description:  Test Harness for Video Playback Views
 *
 */

// Version : %version: 3 %

#ifndef __HAL_STUB_H__
#define __HAL_STUB_H__

#include <e32base.h>

enum TLightStatus
            {
            ELightStatusUnknown = 0,
            ELightOn,              
            ELightOff,             
            ELightBlink            
            };

static TLightStatus  iBacklightState = ELightOn;

/**
@publishedPartner
@released

A set of static functions to get and set HAL attributes.

@see CHWRMLight
*/
class CHWRMLight : public CBase
{
    public:

       enum TLightTarget
            {
            ENoTarget                    = 0x0,    
            EPrimaryDisplay              = 0x1,     
            ESystemTarget                = 0x80000000  
		    };

    public:

        static CHWRMLight* NewL();

        void ConstructL(); 

        CHWRMLight();

        ~CHWRMLight();

    public: 
        void ReserveLightL(TInt aTarget);

        void ReleaseLight(TInt aTarget);

        void LightOnL(TInt aTarget);

   	    void LightOffL(TInt aTarget);

        TLightStatus LightStatus(TInt aTarget) const;

};

#endif
