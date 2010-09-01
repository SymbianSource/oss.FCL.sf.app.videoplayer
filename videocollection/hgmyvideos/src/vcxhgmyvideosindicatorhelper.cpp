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
* Description:   TVcxHgMyVideosIndicatorHelper implementation*
*/




// INCLUDE FILES
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <ganes/HgItem.h>
#include "CIptvDriveMonitor.h"
#include <vcxhgmyvideos.mbg>
#include "vcxhgmyvideosmainview.h"
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideosindicatorhelper.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TVcxHgMyVideosIndicatorHelper::TVcxHgMyVideosIndicatorHelper()
    {
    // No implementation needed.
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TVcxHgMyVideosIndicatorHelper::~TVcxHgMyVideosIndicatorHelper()
    {
    // No implementation needed.
    }

// -----------------------------------------------------------------------------
// Sorts indicators to correct location based on indicator priorities, caller
// supplies all the necessary data for knowing which indicators to show. 
// -----------------------------------------------------------------------------
//
void TVcxHgMyVideosIndicatorHelper::GetIndicatorsForVideoL( CVcxHgMyVideosModel& aModel,
                                                            TBool aNewVideo,
                                                            TBool /*aRecordingOngoing*/,
                                                            const TDesC& aPath,
                                                            TIndicatorDrmStatus aDrmStatus,
                                                            TInt& aIndicator1,
                                                            TInt& aIndicator2 )
    {
    RArray<TInt> indicators;

    if ( aNewVideo )
        {
        indicators.AppendL( CHgItem::EHgItemFlagsNewVideoIndicator );
        }

    if ( aPath.Length() > 0 )
        {
        TInt drive = 0;

        if ( aModel.FileServerSessionL().CharToDrive( aPath[0], drive ) == KErrNone )
            {
            if ( drive == aModel.DriveMonitorL().SystemDrive() )
                {
                indicators.AppendL( CHgItem::EHgItemFlagsPhoneStorageIndicator );
                }
            else if ( aModel.DriveMonitorL().IsMemoryCardDriveL( drive ) )
                {
                indicators.AppendL( CHgItem::EHgItemFlagsMmc );
                }
            else
                {           
                indicators.AppendL( CHgItem::EHgItemFlagsMassStorageIndicator );
                }
            }
        }

    if ( aDrmStatus == EIndicatorDrmStatusValid )
        {
        indicators.AppendL( CHgItem::EHgItemFlagsDrmRightsValid );
        }
    else if ( aDrmStatus == EIndicatorDrmStatusExpired )
        {
        indicators.AppendL( CHgItem::EHgItemFlagsDrmRightsExpired );
        }

    if ( indicators.Count() >= 2 )
        {
        aIndicator1 = indicators[1];
        aIndicator2 = indicators[0];
        }
    else if ( indicators.Count() == 1 )
        {
        aIndicator1 = indicators[0];
        }

    indicators.Reset();
    }
