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
* Description:    HG VOD*
*/




#ifndef VCXHGVODVIDEODETAILSDIALOG_H_
#define VCXHGVODVIDEODETAILSDIALOG_H_

// INCLUDES

NONSHARABLE_CLASS ( TVcxHgVodVideoDetailsDialog )
    {
    public:
        
        /**
         * Displays video details dialog.
         */
        void ShowVideoDetailsDialogL( CVcxNsContent& aContent );
        
    private:
    
        /**
         * Default C++ constructor. By default it is private
         */
        TVcxHgVodVideoDetailsDialog();
    };

#endif /*VCXHGVODVIDEODETAILSDIALOG_H_*/
