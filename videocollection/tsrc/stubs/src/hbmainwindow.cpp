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
* Description: stub hblabel
* 
*/

#include "hbmainwindow.h"
#include "hbview.h"
#include "qlist.h"

// ---------------------------------------------------------------------------
// addView
// ---------------------------------------------------------------------------
//
HbView* HbMainWindow::addView(QGraphicsWidget *widget)
{
    HbView* view = qobject_cast<HbView*>(widget);
    mViews.append(view);
    return view;
}

// ---------------------------------------------------------------------------
// removeView
// ---------------------------------------------------------------------------
//
void HbMainWindow::removeView(QGraphicsWidget *widget)
{
    mViews.removeAll(qobject_cast<HbView*>(widget));
}

// ---------------------------------------------------------------------------
// views
// ---------------------------------------------------------------------------
//
QList<HbView *> HbMainWindow::views() const
{
    return mViews;
}

HbView *currentView() const
{
    return 0;
}
