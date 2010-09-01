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



#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>

#include "vcxnscontent.h"
#include "vcxnscontentaccess.h"
#include "IptvDebug.h"

// -----------------------------------------------------------------------------
// CVcxNsContentAccess::CVcxNsContentAccess()
// -----------------------------------------------------------------------------
//
CVcxNsContentAccess::CVcxNsContentAccess()
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsContentAccess::Newl
// -----------------------------------------------------------------------------
//
CVcxNsContentAccess* CVcxNsContentAccess::NewL()
    { 
    return new (ELeave) CVcxNsContentAccess; 
    }
// -----------------------------------------------------------------------------
// CVcxNsContentAccess::~CVcxNsContentAccess()
// -----------------------------------------------------------------------------
//
CVcxNsContentAccess::~CVcxNsContentAccess( )
    {
    delete iUrl;
    delete iMpxMedia;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentAccess::SetUrlL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentAccess::SetUrlL(  const TDesC& aUrl )
    {
    delete iUrl;
    iUrl = NULL;
    
    if( aUrl.Length() > 0 )
        {
        iUrl = aUrl.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsContentAccess::GetUrl()
// -----------------------------------------------------------------------------
//
const TDesC& CVcxNsContentAccess::GetUrl()
    {
    if( iUrl && iUrl->Length() > 0 )
        {
        return *iUrl;
        }
    
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxNsContentAccess::SetMpxMediaL()
// -----------------------------------------------------------------------------
//
void CVcxNsContentAccess::SetMpxMediaL( CMPXMedia* aMpxMedia )
    {
    delete iMpxMedia;
    iMpxMedia = NULL;

    if( aMpxMedia )
        {
        iMpxMedia = CMPXMedia::NewL( *aMpxMedia );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContentAccess::GetMpxMedia()
// -----------------------------------------------------------------------------
//
CMPXMedia* CVcxNsContentAccess::GetMpxMedia()
    {
    return iMpxMedia;
    }
