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
* Description:  mpxvideoplayerappui stub for STIF testing
*
*/

// Version : %version: ou1cpsw#3 %

#ifndef CMPXVIDEOPLAYERAPPUI_H
#define CMPXVIDEOPLAYERAPPUI_H

class MAppUiEngineStifTestObserver;
class CAknView;

// FORWARD DECLARATIONS

class CMpxVideoPlayerAppUi
{
    public:        // Constructors and destructor

        CMpxVideoPlayerAppUi( TBool aEmbedded, MAppUiEngineStifTestObserver* aObserver );

        ~CMpxVideoPlayerAppUi();

    public:       // New functions

        void HandleCommandL( TInt aCommand );

        TBool IsEmbedded() const;

        CAknView* View();
    private:

        TBool                           iEmbedded;
        MAppUiEngineStifTestObserver*   iStifObserver;
};

#endif             // CMPXVIDEOPLAYERAPPUI_H
