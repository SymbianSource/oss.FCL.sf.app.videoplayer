/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Inline methods for media file details dialog.*
*/


// Version : %version: 2 %



// INCLUDE FILES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Return client window handle
// ---------------------------------------------------------
//
inline RWindow& CMPDummyController::GetClientWindow()
    {
    return Window();
    }

// ---------------------------------------------------------
// Return client WsSession
// ---------------------------------------------------------
//
inline RWsSession& CMPDummyController::GetClientWsSession()
    {
    return ControlEnv()->WsSession();
    }

// ---------------------------------------------------------
// Return client ScreenDevice
// ---------------------------------------------------------
//
inline CWsScreenDevice& CMPDummyController::GetScreenDevice()
    {
    return *(ControlEnv()->ScreenDevice());
    }


//  End of File  
