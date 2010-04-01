/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HbMessageBox class declaration.
*
*/


#ifndef HBMESSAGEBOX_H
#define HBMESSAGEBOX_H

#include <QString>

class HbMessageBox;


class HbMessageBox 
{
   
public:
    
    static bool question(QString text)
    {
        mLatestTxt = text;
        return mQuestionReturnValue;
    }
    
    static void information(QString text)
    {
        mInformationCallCount++;
        mLatestTxt = text;
    }
    
    static void warning(QString text)
    {
        mWarningCallCount++;
        mLatestTxt = text;
    }
    
    /**
     * returned from question
     */
    static bool mQuestionReturnValue;
    
    /**
     * saved text
     */
    static QString mLatestTxt;
    
    static int mWarningCallCount;
    
    static int mInformationCallCount;
};

#endif // HBMESSAGEBOX_H
