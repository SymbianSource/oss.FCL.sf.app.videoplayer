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
* Description: stub videocollectionuiloaderdata
* 
*/

#ifndef VIDEOCOLLECTIONUILOADERDATA_H
#define VIDEOCOLLECTIONUILOADERDATA_H

#include <qstringlist.h>

class VideoCollectionUiLoaderData
{
public: // methods
    static void reset()
    {
        mFindFailure = false;
        mFailDialogLoad = false;
        mFindFailureNameList.clear();
    }
    
public: // data
    static bool mFindFailure;
    static bool mFailDialogLoad;
    static QStringList mFindFailureNameList;
};

#endif /* VIDEOCOLLECTIONUILOADERDATA_H */
