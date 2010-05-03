/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: stub videoservicebrowse
* 
*/

#ifndef VIDEOSERVICEBROWSEDATA_H
#define VIDEOSERVICEBROWSEDATA_H

class VideoServiceBrowseData
{
public: // methods
    static void reset()
    {
        mBrowseCategory = 0;
    }
    
public: // data
    static int mBrowseCategory;
};

#endif /* VIDEOSERVICEBROWSEDATA_H */
