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
* Description:  stub HbMessageBox class declaration.
*
*/


#ifndef HBMESSAGEBOX_H
#define HBMESSAGEBOX_H

#include <QString>


class HbMessageBox 
{
   
public:
    
    /**
     * saves provided text to mLatestTxt
     * returns mQuestionReturnValue
     */
    static bool question(QString text)
    {
        mLatestTxt = text;
        return mQuestionReturnValue;
    }
    
    /**
     * saves provided text to mLatestTxt
     */
    static void warning(QString text)
    {
        mLatestTxt = text;
    }
    
    /**
     * saves provided text to mLatestTxt
     */
    static void information(QString text)
    {
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
    
};



#endif // HBMESSAGEBOX_H
