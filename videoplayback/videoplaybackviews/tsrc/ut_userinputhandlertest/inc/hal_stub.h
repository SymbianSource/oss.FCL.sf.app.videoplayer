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

// Version : %version: ou1cpsw#2 %

#ifndef __HAL_STUB_H__
#define __HAL_STUB_H__

#include <e32def.h>
#include <hal_data.h>
#include <e32property.h>


static TInt  iBacklightState = 1;

/**
@publishedPartner
@released

A set of static functions to get and set HAL attributes.

@see HALData
*/
class HAL : public HALData
	{
public:
    
    HAL();
    virtual ~HAL();
    
    /**
    Gets the value of the specified HAL attribute.

    @param aAttribute The HAL attribute.
    @param aValue      On successful return, contains the attribute value.

    @return  KErrNone, if successful;
             KErrNotSupported, if the attribute is not defined in the list
             of attributes, or is not meaningful for this device.
         
    @see HALData::TAttribute
    @see HALData::TAttributeProperty
    */
	static TInt Get(TAttribute aAttribute, TInt& aValue);

	
	/**
    Sets the specified HAL attribute.

    @param aAttribute The HAL attribute.
    @param aValue      The attribute value.

    @return  KErrNone, if successful;
             KErrNotSupported, if the attribute is not defined in the list
             of attributes, or is not meaningful for this device, or is
             not settable.
         
    @see HALData::TAttribute
    @see HALData::TAttributeProperty

    @capability WriteDeviceData or other capability specified
    for individual attributes in TAttribute
    */
	static TInt Set(TAttribute aAttribute, TInt aValue);
	
	};

#endif
