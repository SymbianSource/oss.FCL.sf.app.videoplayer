/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MediaSetting constants.*
*/


// Version : %version: 3 %




#ifndef MPSETTINGSCONSTANTS_H
#define MPSETTINGSCONSTANTS_H

// CONSTANTS

// UIDs of views

// Main view
const TUid KMPSettMainViewId = {1};

// UIDs for views, which are opened from Main view.
// The UID values have to be in the same order as
// the views appear in Main view's list, i.e. the
// first item has to have smallest UID and so forth.
const TUid KMPSettVideoViewId = {2};
const TUid KMPSettAudioViewId = {3};
const TUid KMPSettStreamingViewId = {4};
const TUid KMPSettFeedsViewId = {5};

// UIDs for subviews
const TUid KMPSettProxyViewId = {11};
const TUid KMPSettNetworkViewId = {12};
const TUid KMPSettAdvancedBwViewId = {13};

// App uid for help context
const TUid KIptvHelpContextUid = {0x102750E2};
const TUid KGSVideoPluginUid = {0x10275069}; 

// Middle Softkey control ID.
const TInt KMediaSettingsMSKControlID = 3;

#endif      // MPSETTINGSCONSTANTS_H   
            
// End of File
