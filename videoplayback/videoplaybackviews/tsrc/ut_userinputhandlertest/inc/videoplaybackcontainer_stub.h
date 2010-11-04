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
* Description:  STUB file for STIF Video Playback Views.
*
*/

// Version : %version: ou1cpsw#2 %




#ifndef __CVIDEOPLAYBACKCONTAINER_STUB_H__
#define __CVIDEOPLAYBACKCONTAINER_STUB_H__


/**
 *  Stub Container class.
 *
 */
class CMPXVideoPlaybackContainer: public CBase
{
    public:

        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackContainer* NewL();

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackContainer();

    private:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackContainer();
    
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    public:

        void HandleCommandL( TInt aCommand, TInt aValue = 0 );
        void DoHandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        void DoHandlePointerEventL( const TPointerEvent& aPointerEvent );
        
    private:
        
        TKeyEvent       iKeyEvent;
        TEventCode      iEventCode;
        TPointerEvent   iPointerEvent;
        TInt            iCommand;
        
    public:     // Friend class
        
        friend class CUserinputhandlertest;

};

#endif  // __CVIDEOPLAYBACKCONTAINER_STUB_H__

// End of File
