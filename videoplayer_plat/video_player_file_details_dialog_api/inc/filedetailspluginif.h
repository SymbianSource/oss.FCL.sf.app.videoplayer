/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Interface for filedetails plugin.*
*/



#ifndef FILEDETAILSPLUGINIF_H
#define FILEDETAILSPLUGINIF_H

#include <e32std.h>
#include <e32base.h>
#include <e32def.h>

#include "filedetailspluginif.hrh"

class CMPXMedia;

/**
 * ECom plugin interface definition.
 */
class CFileDetailsPluginIF : public CBase
    {
public:
    /**
    * Construction method.
    * @return   Created object.
    */
    static CFileDetailsPluginIF* NewL( );

    /**
    * Destructor
    */    
    virtual ~CFileDetailsPluginIF();
    
    /**
    * Shows file details based on given mpx media
    * @param    aDetails  Already constructed details to be shown    
    * @return   None
    */  
    virtual void ShowFileDetailsL( const CMPXMedia& aMedia ) = 0; 
    
protected:    
    /**
    * Instance identifier key. When instance of an
    * implementation is created by ECOM framework, the
    * framework will assign UID for it. The UID is used in
    * destructor to notify framework that this instance is
    * being destroyed and resources can be released.
    */    
    TUid iDtorIDKey;

    };



#include <ecom/ecom.h>
#include "filedetailspluginif.inl"

#endif //FILEDETAILSPLUGINIF_H
