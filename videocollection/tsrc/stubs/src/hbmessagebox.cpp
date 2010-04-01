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
* Description: stub hbmessagebox
* 
*/

#include <qstring.h>

#include "hbmessagebox.h"
#include "hbmessageboxdata.h"

bool HbMessageBoxData::mQuestionReturnValue = false;
QString HbMessageBoxData::mLatestTxt = "";
int HbMessageBoxData::mWarningCallCount = 0;
int HbMessageBoxData::mInformationCallCount = 0;

bool HbMessageBox::question(const QString &questionText,
    const QString &primaryButtonText,
    const QString &secondaryButtonText,
    QGraphicsWidget *headWidget,
    QGraphicsScene *scene,
    QGraphicsItem *parent)
{
    Q_UNUSED(primaryButtonText);
    Q_UNUSED(secondaryButtonText);
    Q_UNUSED(headWidget);
    Q_UNUSED(scene);
    Q_UNUSED(parent);
    
    HbMessageBoxData::mLatestTxt = questionText;
    return HbMessageBoxData::mQuestionReturnValue;
}

void HbMessageBox::information(const QString &informationText,
    QGraphicsWidget *headWidget,
    QGraphicsScene *scene,
    QGraphicsItem *parent)
{
    Q_UNUSED(headWidget);
    Q_UNUSED(scene);
    Q_UNUSED(parent);

    HbMessageBoxData::mLatestTxt = informationText;
    HbMessageBoxData::mInformationCallCount++;
}

void HbMessageBox::warning(const QString &warningText,
    QGraphicsWidget *headWidget,
    QGraphicsScene *scene,
    QGraphicsItem *parent)
{
    Q_UNUSED(headWidget);
    Q_UNUSED(scene);
    Q_UNUSED(parent);

    HbMessageBoxData::mLatestTxt = warningText;
    HbMessageBoxData::mWarningCallCount++;
}

// end of file
