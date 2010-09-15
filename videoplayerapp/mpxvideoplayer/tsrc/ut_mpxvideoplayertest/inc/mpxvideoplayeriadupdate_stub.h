/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    IAD update handling.
* 
*/


// Version : %version: 1 %


#ifndef CMPXVIDEOPLAYERIADUPDATE_H
#define CMPXVIDEOPLAYERIADUPDATE_H

#include <e32std.h>

class MMPXViewUtility;


/**
 * This class handles IAD update.
 */
NONSHARABLE_CLASS( CMpxVideoPlayerIadUpdate ) : public CBase
    {

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aViewUtility Reference to view utility
     */
    static CMpxVideoPlayerIadUpdate* NewL( MMPXViewUtility& aViewUtility );

    /**
     * Destructor.
     */
    virtual ~CMpxVideoPlayerIadUpdate();
   
public: // New methods

    /**
     * Start IAD update process.
     */
    void StartL();

private: // Constructors and destructor

    /**
     * C++ default constructor.
     * @param aViewUtility Reference to view utility
     */
    CMpxVideoPlayerIadUpdate( MMPXViewUtility& aViewUtility );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
private: // Data

    /**
     * Reference to view utility.
     */
    MMPXViewUtility& iViewUtility;

    };

#endif // CMPXVIDEOPLAYERIADUPDATE_H

