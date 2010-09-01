/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Observer for .rtp to .3gp file converter*
*/




#ifndef MIPTVRTPTO3GPOBSERVER_H
#define MIPTVRTPTO3GPOBSERVER_H

#include <e32base.h>

class CIptvRtpTo3gpConverter;

/**
*  Observer for .rtp to .3gp file converter
*
*  @lib    IptvUtil
*  @since  S60 v3.1
*/
class MIptvRtpTo3gpObserver
    {
public:
    /**
    *  Notification for .rtp to .3gp conversion completed
    *  @param aConverter reference to converter reporting conversion ready
    *  @param aError KErrNone on conversion success, system error code otherwise
    */
    virtual void RtpTo3gpConversionReady( CIptvRtpTo3gpConverter& aConverter, TInt aError ) = 0;
    };

#endif // MIPTVRTPTO3GPOBSERVER_H
