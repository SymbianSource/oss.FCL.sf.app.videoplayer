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

NONSHARABLE_CLASS ( TVcxHgMyVideosVideoDetailsDialog )
    {
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
        TVcxHgMyVideosVideoDetailsDialog();
    };

#endif /*VCXHGMYVIDEOSVIDEODETAILS_H_*/
