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
* Description:   TVcxHgMyVideosIndicatorHelper class definition*
*/




#ifndef VCXHGMYVIDEOSINDICATORHELPER_H
#define VCXHGMYVIDEOSINDICATORHELPER_H

// INCLUDE FILES
#include <coemain.h>
 
// FORWARD DECLARATIONS
class CVcxHgMyVideosModel;
class CFbsBitmap;

// CLASS DECLARATION

/**
 *
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( TVcxHgMyVideosIndicatorHelper )  
    {
    public:

        /**
         * DRM status of video, for indicator selection.
         */
        enum TIndicatorDrmStatus
            {
            EIndicatorDrmStatusUnknown = 0,
            EIndicatorDrmStatusValid,
            EIndicatorDrmStatusExpired
            };

    public:        
        
        /**
         * Default constructor
         *
         */
        TVcxHgMyVideosIndicatorHelper();

        /**
         * Destructor.
         */
        virtual ~TVcxHgMyVideosIndicatorHelper();        

        /**
         * Selectes which indicators should be shown for a video.
         * 
         * @param aModel Reference to component's model.
         * @param aNewVideo ETrue if video is new.
         * @param aRecordingOngoing ETrue if video is recording.
         * @param aPath Path to video.
         * @param aDrmStatus TIndicatorDrmStatus.
         * @param aIndicator1 On return, indicator 1.
         * @param aIndicator2 On return, indicator 2.
         */
        void GetIndicatorsForVideoL( CVcxHgMyVideosModel& aModel,
                                     TBool aNewVideo,
                                     TBool aRecordingOngoing,
                                     const TDesC& aPath,
                                     TIndicatorDrmStatus aDrmStatus,
                                     TInt& aIndicator1,
                                     TInt& aIndicator2 );           
    };

#endif // VCXHGMYVIDEOSINDICATORHELPER_H
