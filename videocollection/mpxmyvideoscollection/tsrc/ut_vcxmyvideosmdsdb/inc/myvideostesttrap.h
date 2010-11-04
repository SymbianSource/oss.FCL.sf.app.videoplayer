/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#ifndef MYVIDEOSTESTTRAP_H_

#include <e32cmn.h>
#undef TRAPD
#undef TRAP_IGNORE

#define TRAPD(_r, _s)                                       \
    TInt _r;                                                \
    {                                                       \
    _r = 0;                                                 \
    { TRAP_INSTRUMENTATION_START; }                         \
    try {                                                   \
        __WIN32SEHTRAP                                      \
        TTrapHandler* ____t = User::MarkCleanupStack();     \
        _s;                                                 \
        User::UnMarkCleanupStack(____t);                    \
        { TRAP_INSTRUMENTATION_NOLEAVE; }                   \
        __WIN32SEHUNTRAP                                    \
        }                                                   \
    catch (XLeaveException& l)                              \
        {                                                   \
        _r = l.GetReason();                                 \
        { TRAP_INSTRUMENTATION_LEAVE(_r); }                 \
        }                                                   \
                                                            \
    __CALL_EMPTY_FUNCTION                                   \
    { TRAP_INSTRUMENTATION_END; }                           \
    }

#define TRAP_IGNORE(_s)                                     \
    {                                                       \
    { TRAP_INSTRUMENTATION_START; }                         \
    try {                                                   \
        __WIN32SEHTRAP                                      \
        TTrapHandler* ____t = User::MarkCleanupStack();     \
        _s;                                                 \
        User::UnMarkCleanupStack(____t);                    \
        { TRAP_INSTRUMENTATION_NOLEAVE; }                   \
        __WIN32SEHUNTRAP                                    \
        }                                                   \
    catch (XLeaveException& l)                              \
        {                                                   \
        l.GetReason();                                      \
        { TRAP_INSTRUMENTATION_LEAVE(l.Reason()); }         \
        }                                                   \
                                                            \
    __CALL_EMPTY_FUNCTION                                   \
    { TRAP_INSTRUMENTATION_END; }                           \
    }




#define MYVIDEOSTESTTRAP_H_


#endif /* MYVIDEOSTESTTRAP_H_ */
