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
* Description: stub videothumbnailtestdata
* 
*/

#ifndef VIDEOTHUMBNAILTESTDATA_H
#define VIDEOTHUMBNAILTESTDATA_H

class VideoThumbnailTestData
{
public: // methods
    static void reset()
    {
        mInstanceCallCount = 0;
        mFreeThumbnailDataCallCount = 0;
        mBackgroundThumbnailFetchingEnabled = 0;
        mStartBackgroundFetchingCallCount = 0;
    }
    
public: // data
    static int mInstanceCallCount;
    static int mFreeThumbnailDataCallCount;
    static int mBackgroundThumbnailFetchingEnabled;
    static int mStartBackgroundFetchingCallCount;
};

#endif /* VIDEOTHUMBNAILTESTDATA_H */
