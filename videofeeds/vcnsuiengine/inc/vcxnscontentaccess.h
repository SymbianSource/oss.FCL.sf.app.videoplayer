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




#ifndef C_VCXNSVODCONTENTACCESS_H
#define C_VCXNSVODCONTENTACCESS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "vcxnscontent.h"

// FORWARD DECLARATIONS
class CMPXMedia;

// CLASS DECLARATION

/**
 * CVcxNsContentAccess
 * 
 * @lib vcxnsuiengine.lib
 */
class CVcxNsContentAccess: public CBase
    {
    
public:
    
    virtual ~CVcxNsContentAccess();
    
    static CVcxNsContentAccess* NewL();
    
    /**
     * SetUrlL
     * @param aUrl Url
     */
    void SetUrlL( const TDesC& aUrl );
    
    /**
     * GetUrl
     * @return Url
     */
    const TDesC& GetUrl();

    /**
     * Set mpx media
     * @param aMpxMedia Mpx media object
     */
    void SetMpxMediaL( CMPXMedia* aMpxMedia );
    
    /**
     * Get mpx media
     * @return  Mpx media object
     */
    CMPXMedia* GetMpxMedia();

    /**
     * Content access' type
     */
    CVcxNsContent::TVcxNsContentAccessType iType;

    /**        
     * Mpx media id. needed just before the actual mpx item is loaded.
     * Used only with local videos.
     */
    TUint32 iMpxId;

    /**        
     * Index of the content access data in server side.
     */
    TUint32 iIndex;
    
    /**        
     * Last play position, used only with stream,
     * MPX collection stores the local video pos.
     */
    TUint32 iLastPosition;

private:

    CVcxNsContentAccess();
    
    /**
     * Url
     */
    HBufC* iUrl;
    
    /**        
     * Mpx media data.
     * Used only with local videos.
     */
    CMPXMedia* iMpxMedia;

    };

#endif // C_VCXNSVODCONTENTACCESS_H
