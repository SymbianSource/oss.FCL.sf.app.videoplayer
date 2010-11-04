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
* Description:  STUB file for project STIF Video Playback Controls.
*
*/

// Version : %version: ou1cpsw#2 %




#ifndef __CVIDEOPLAYBACKCONTROL_STUB_H__
#define __CVIDEOPLAYBACKCONTROL_STUB_H__


// INCLUDES
#include <coecntrl.h>

/**
 *  Control class for video playback
 *
 */
class CMPXVideoPlaybackControl : public CCoeControl
{

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackControl* NewL();

        /**
        * Destructor.
        */
        virtual ~CMPXVideoPlaybackControl();

    protected:

        /**
        * C++ default constructor.
        */
        CMPXVideoPlaybackControl();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    protected:

        /**
        * From CCoeControl.
        * Gets the number of controls contained in a compound control.
        *
        * @return The number of component controls contained by this control.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl.
        * Gets an indexed component of a compound control.
        *
        * @param aIndex The index of the control.
        * @return The component control with an index of aIndex.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
        /**
        * From CCoeControl,Draw.
        * @param aRect drawable area.
        */
        void Draw(const TRect& aRect) const;        
        
    public:
    
        void DoHandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        void DoHandlePointerEventL( const TPointerEvent& aPointerEvent );
        
    private:
        
        TKeyEvent       iKeyEvent;
        TEventCode      iEventCode;        
        TPointerEvent   iPointerEvent;
        
    public:     // Friend class
        
        friend class CUserinputhandlertest;
        
};

#endif  // __CVIDEOPLAYBACKCONTROL_STUB_H__

// End of File
