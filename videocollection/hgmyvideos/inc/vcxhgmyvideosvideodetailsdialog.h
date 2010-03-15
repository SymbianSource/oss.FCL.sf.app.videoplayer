/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      My Videos Video Details Dialog.*
*/




#ifndef VCXHGMYVIDEOSVIDEODETAILS_H_
#define VCXHGMYVIDEOSVIDEODETAILS_H_


#include <mpxmedia.h>

NONSHARABLE_CLASS ( CVcxHgMyVideosVideoDetailsDialog ) : public CBase
    {
    public:// Constructors and destructor

        /**
        * Two-phased constructor.        
        */
        static CVcxHgMyVideosVideoDetailsDialog* NewL();
        
        /**
        * C+ destructor
        */
        virtual ~CVcxHgMyVideosVideoDetailsDialog();        

    public:        
        
        /**
         *  Provides the static function for the callback to show file details.
         *  Called by CIdle iDetailsAo.
         *
         *  @param aPtr Pointer to callback class
         */
        static TInt ShowDetailsL( TAny* aPtr );
                
        
    public:
        
        /**
         * Displays video details dialog.
         * 
         * @param aMedia MPX media object that contains video details.
         */
        void ShowVideoDetailsDialogL( const CMPXMedia& aMedia );      

    private:
    
        /**
         * Default C++ constructor. By default it is private
         */
        CVcxHgMyVideosVideoDetailsDialog();
      
        /**
         * Symbian 2nd phase constructor.
         */        
        void ConstructL();

        /*
         *  Activates an active object to display details.
         */    
        void ActivateDetailsActiveObject();

        /*
         *  Called to display file details.
         */   
        void DoShowDetailsL();

        /*
         *  Pointer to MPX media for which details are displayed. Own.
         */   
        CMPXMedia* iDetailsMedia;

        /*
         *  Active object to cut call stack and display details. Own.
         */  
        CIdle* iDetailsAo;
        
    };

#endif /*VCXHGMYVIDEOSVIDEODETAILS_H_*/
