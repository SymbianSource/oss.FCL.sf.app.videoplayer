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
* Description:      Implementation of UI functionality for category list.*
*/




#ifndef VCXHGMYVIDEOSMEMORYSTATUS_H_
#define VCXHGMYVIDEOSMEMORYSTATUS_H_

// FORWARD DECLARATIONS
class CVcxHgMyVideosModel;

// INCLUDES

NONSHARABLE_CLASS( TVcxHgMyVideosMemoryStatusDialog )
    {
    public:

        /**
         * Displays memory status dialog.
         * 
         * @param aModel Reference to document model.
         */
        void ShowMemoryStatusDialogL( CVcxHgMyVideosModel& aModel );       

    private:
    
        /**
         * Default C++ constructor. By default it is private
         */
        TVcxHgMyVideosMemoryStatusDialog();
    };
#endif /*VCXHGMYVIDEOSMEMORYSTATUS_H_*/
