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
* Description:    Debug print macros*
*/




#ifndef MPXLOG_H
#define MPXLOG_H

// INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <e32property.h>

#include <mpxvideo_debug.h>

#ifdef _DEBUG

#define MPX_FUNC(fn)                TFunctionTrace _s(_L(fn));
#define MPX_FUNC_EX(fn)             TFunctionTrace _s(_L(fn), this);
    
// print out (index, id) for each level and the first 15 ids at top level
#define MPX_DEBUG_PATH(aPath)
#define MPX_DEBUG_THREAD(ss)
#define MPX_DEBUG_PATH(aPath)
#define MPX_DEBUG_THREAD(ss)

#else // _DEBUG

#define MPX_FUNC(fn)                
#define MPX_FUNC_EX(fn)             
    
// print out (index, id) for each level and the first 15 ids at top level
#define MPX_DEBUG_PATH(aPath)
#define MPX_DEBUG_THREAD(ss)
#define MPX_DEBUG_PATH(aPath)
#define MPX_DEBUG_THREAD(ss)

#endif

#endif  // MPXLOG_H

// End of File
