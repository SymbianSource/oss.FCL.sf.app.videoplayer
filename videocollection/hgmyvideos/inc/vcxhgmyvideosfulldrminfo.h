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
* Description:   TVcxHgMyVideosFullDrmInfo class definition*
*/




#ifndef VCXHGMYVIDEOSFULLDRMINFO_H
#define VCXHGMYVIDEOSFULLDRMINFO_H

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>
#include <coemain.h>

/**
 * Collects the drm info of item and puts strings to given array
 *
 * @lib 
 */
class TVcxHgMyVideosFullDrmInfo
    {
    public: // Constructors and destructor

        /**
         * Static method for getting full OMA or WM DRM information of video clip.
         *
         * @param aArray Array which is filled with DRM information, to be 
         *               displayed in info popup.
         * @param aFullPath Full path to item which DRM info is needed.
         */
        static void GetFullDrmInfoL( CDesCArray& aArray,
                                     const TDesC& aFullPath );

    protected:

        /**
         * C++ default constructor.
         */
        TVcxHgMyVideosFullDrmInfo();
        

        /**
         * Destructor.
         */
        ~TVcxHgMyVideosFullDrmInfo();
    };

#endif // VCXHGMYVIDEOSFULLDRMINFO_H
