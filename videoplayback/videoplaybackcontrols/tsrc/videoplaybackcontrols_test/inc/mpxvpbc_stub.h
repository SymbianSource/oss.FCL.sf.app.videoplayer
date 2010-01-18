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
* Description:  Test Harness for VideoPlaybackControls
*
*/

// Version : %version: ou1cpsw#7 %




#ifndef VIDEOPLAYBACKTESTCONTAINER_H
#define VIDEOPLAYBACKTESTCONTAINER_H

//  INCLUDES
#include <coecntrl.h>

// CLASS DECLARATION
class CMPXVideoPlaybackControlsController;
class CVideoPlaybackControlsTest;
class CMPXVideoPlaybackViewFileDetails;
class CMPXVideoPlaybackUserInputHandler;

class CMPXVideoPlaybackContainer: public CCoeControl
{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXVideoPlaybackContainer* NewL( const TDesC& aFileName );
        
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
        void ConstructL( const TDesC& aFileName );

    private:

        virtual TInt CountComponentControls() const;

    protected:

        /**
        * From CCoeControl,Draw.
        * @param aRect drawable area.
        */
        void Draw(const TRect& aRect) const;
       
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    public:
        /**
        * Handle commands from control controller
        * @return void
        */
        void HandleCommandL( TInt aCommand, TInt aValue = 0 );

        /**
        * Expected Values
        */
        void ExpectedResultL( TInt aCommand, TInt aValue );

        /**
        * Set a control's controller
        */
        void SetController( CMPXVideoPlaybackControlsController* aControlsController );

        /**
        * add default file details
        */
        void AddFileDetailsL( TBool aHasVideoTrack = ETrue );
        
        void DoHandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Return default file details
        */
        CMPXVideoPlaybackViewFileDetails* FileDetails( TInt aMode );
        
        CMPXVideoPlaybackUserInputHandler* UserInputHandler();

    private:    
        // Data
        TInt                                 iCommand;
        TInt                                 iValue;
        CMPXVideoPlaybackControlsController* iControlsController;
        CMPXVideoPlaybackViewFileDetails*    iFileDetails;
        TKeyResponse                         iKeyResponse;    
        
        CMPXVideoPlaybackUserInputHandler*       iUserInputHandler;  // owned        

};

#endif      // VIDEOPLAYBACKTESTCONTAINER_H

// End of File
