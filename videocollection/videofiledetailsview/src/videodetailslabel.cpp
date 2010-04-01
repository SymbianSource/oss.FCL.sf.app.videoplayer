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
* Description:   VideoDetailsLabel class implementation
*
*/

#include "videodetailslabel.h"

// SYSTEM INCLUDES
#include <HbLabel>
#include <QGraphicsSceneMouseEvent>

//---------------------------------------------------------------
// VideoDetailsLabel::VideoDetailsLabel
// @see header
//---------------------------------------------------------------
VideoDetailsLabel::VideoDetailsLabel(QGraphicsItem *parent) :
    HbLabel(parent)
{
}

//---------------------------------------------------------------
// VideoDetailsLabel::~VideoDetailsLabel
// @see header
//---------------------------------------------------------------
VideoDetailsLabel::~VideoDetailsLabel()
{
}

//---------------------------------------------------------------
// VideoDetailsLabel::click
// @see header
//---------------------------------------------------------------
void VideoDetailsLabel::click()
{
	emit clicked(true);
}

//---------------------------------------------------------------
// VideoDetailsLabel::init
// @see header
//---------------------------------------------------------------
void VideoDetailsLabel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    emit clicked(true);
}

// EOF
