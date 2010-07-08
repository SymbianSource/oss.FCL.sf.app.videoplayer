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
* Description: CCoeControl to provide RWindow for videoplayback
*
*/

// Version : %version:  2 %


#ifndef CMPXVIDEOCONTAINER_H
#define CMPXVIDEOCONTAINER_H

#include <coecntrl.h>


class CMPXVideoContainer : public CCoeControl
{

    public:
        CMPXVideoContainer();
        virtual ~CMPXVideoContainer();

        void ConstructL();
};

#endif  // CMPXVIDEOCONTAINER_H

// End of file