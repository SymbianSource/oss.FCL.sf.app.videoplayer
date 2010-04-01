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
* Description: stub videolistselectiondialogdata
* 
*/

#ifndef VIDEOLISTSELECTIONDIALOGDATA_H
#define VIDEOLISTSELECTIONDIALOGDATA_H

class VideoListSelectionDialogData
{
public: // methods
    static void reset()
    {
        mExecReturnPrimary = false;
        mSelectionCount = 0;
        mMultiSelectionLaunchCount = 0;
        mMultiSelectionItemSelectionCount = 0;
    }
    
public: // data
    static bool mExecReturnPrimary;
    static int mSelectionCount;
    static int mMultiSelectionLaunchCount;
    static int mMultiSelectionItemSelectionCount;
};

#endif /* VIDEOLISTSELECTIONDIALOGDATA_H */
