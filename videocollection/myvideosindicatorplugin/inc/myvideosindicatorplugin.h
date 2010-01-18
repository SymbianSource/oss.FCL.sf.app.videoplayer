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
* Description:  My Videos indicator plug-in.*
*/


#ifndef MYVIDEOSINDICATORPLUGIN_H
#define MYVIDEOSINDICATORPLUGIN_H

#include <myvideosindicator.h>

/**
 *  My Videos indicator plug-in implementation.
 */
class CMyVideosIndicatorPlugin : public CMyVideosIndicator
    {
public:

    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
    static CMyVideosIndicatorPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CMyVideosIndicatorPlugin();

    /**
     * Returns overlay indicator. Ownership is transferred.
     *
     * @return Pointer to an icon.
     */
    CGulIcon* VideoOverlayIndicatorL();

    /**
     * Tells whether to show the overlay indicator for a given media object.
     * 
     * @param aMedia The media object.
     * @return Whether to show the indicator (true/false).
     */
    TBool IsIndicatorShown( const CMPXMedia& aMedia );

private:

    /**
     * C++ constructor.
     */
    CMyVideosIndicatorPlugin();

    };

#endif // MYVIDEOSINDICATORPLUGIN_H
