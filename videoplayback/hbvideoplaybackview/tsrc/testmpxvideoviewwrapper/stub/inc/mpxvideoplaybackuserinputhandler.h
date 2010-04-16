/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of MPXVideoPlaybackUserInputHandler
*
*/

// Version : %version:  4 %



#ifndef MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_
#define MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_

// INCLUDES


// FORWARD DECLARATIONS
class CMPXVideoViewWrapper;

// CLASS DECLARATION

class CMPXVideoPlaybackUserInputHandler : public CBase
{
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMPXVideoPlaybackUserInputHandler* NewL(
                CMPXVideoViewWrapper* aWrapper, TBool aTvOutConnected );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMPXVideoPlaybackUserInputHandler();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackUserInputHandler( CMPXVideoViewWrapper* aWrapper );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( TBool aTvOutConnected );

    public:
        void ProcessKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        void SetForeground( TBool aForeground );
         
        void HandleTVOutEventL(TBool aTVOutConnected);
                
    private:
        TBool                                   iTVOutConnected;        // Flag to indicate if TV is connected
        TBool                                   iForeground;
        CMPXVideoViewWrapper*                   iViewWrapper;
};


#endif /*MPXVIDEOPLAYBACKUSERINPUTHANDLER_H_*/

// End of File
