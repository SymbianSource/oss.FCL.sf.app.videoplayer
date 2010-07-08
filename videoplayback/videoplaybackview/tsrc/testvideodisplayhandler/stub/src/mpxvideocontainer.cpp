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
* Description:
*
*/

// Version : %version:  1 %



#include "mpxvideocontainer.h"


CMPXVideoContainer::CMPXVideoContainer()
{
}

CMPXVideoContainer::~CMPXVideoContainer()
{
    CloseWindow();
}

void CMPXVideoContainer::ConstructL()
{
    CreateWindowL();
    ActivateL();
}

void CMPXVideoContainer::Draw( const TRect& /*aRect*/ ) const
{
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
    gc.SetBrushColor( TRgb::Color16MA( 0 ) );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( Rect() );
}

// End of file
