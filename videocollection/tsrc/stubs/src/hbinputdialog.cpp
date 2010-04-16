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
* Description: stub hbinputdialog
* 
*/

#include "hbinputdialog.h"

bool HbInputDialog::mGetTextFails = false;
QString HbInputDialog::mGetTextReturnValue = QString();
int HbInputDialog::mGetTextCallCount = 0;


QString HbInputDialog::getText(const QString &label,const QString &text,
           bool *ok, QGraphicsScene *scene, QGraphicsItem *parent)
{
   Q_UNUSED(label);
   Q_UNUSED(scene);
   Q_UNUSED(parent);
   mGetTextCallCount++;
   QString textReturn;
   if(HbInputDialog::mGetTextReturnValue.isNull())
   {
       textReturn = text;
   }
   else
   {
       textReturn = mGetTextReturnValue;
   }
   if(mGetTextFails)
   {
       *ok = false;
       textReturn = "";
   }
   else
   {
       *ok = true;            
   }
   return textReturn;
}

