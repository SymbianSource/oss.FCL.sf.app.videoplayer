/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef TIPTVMIMETYPE_H
#define TIPTVMIMETYPE_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"

// CLASS DECLARATION
class TIptvMimeType
    {
    public: // Data members

        /**
        * Mime-type
        */
        TBuf<KIptvCAMimeTypeLength> iMimeType;

        /**
        * Max bitrate
        */
        TUint32 iMaxBitrate;

        /**
        * Max horizontal screensize
        */
        TUint32 iMaxScreensizeH;

        /**
        * Max vertical screensize
        */
        TUint32 iMaxScreensizeV;

        /**
        * Max framerate
        */
        TUint32 iMaxFramerate;   
    };

#endif  // TIPTVMIMETYPE_H

// End of File
