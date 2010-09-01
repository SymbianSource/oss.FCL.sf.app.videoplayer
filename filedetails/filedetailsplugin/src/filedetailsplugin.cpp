/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of file details plugin*
*/
#include <MPFileDetailsDialog.h>
#include <MPFileDetails.h>
#include "filedetailsplugin.h"
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxmedia.h>
#include <mpxmediavideodefs.h>
#include <vcxmyvideosdefs.h>

const TInt KThousandNotKilobyte = 1000;

// -----------------------------------------------------------------------------
// CFileDetailsPlugin::NewL
//
// -----------------------------------------------------------------------------
//
CFileDetailsPlugin* CFileDetailsPlugin::NewL()
    {
    CFileDetailsPlugin* self =
            new (ELeave) CFileDetailsPlugin();

    return self;        
    }

// -----------------------------------------------------------------------------
// CFileDetailsPlugin::CFileDetailsPlugin
//
// -----------------------------------------------------------------------------
//
CFileDetailsPlugin::CFileDetailsPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CFileDetailsPlugin::~CFileDetailsPlugin
//
// -----------------------------------------------------------------------------
//  
CFileDetailsPlugin::~CFileDetailsPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CFileDetailsPlugin::~CFileDetailsPlugin
//
// -----------------------------------------------------------------------------
//  
void CFileDetailsPlugin::ShowFileDetailsL( const CMPXMedia& aMedia )
    {  
    CMPFileDetails* details = new (ELeave) CMPFileDetails();
    CleanupStack::PushL( details );
    
    //File path    
    if ( aMedia.IsSupported( KMPXMediaGeneralUri ) )
        {
        details->iFilePath = aMedia.ValueText( KMPXMediaGeneralUri ).AllocL();                
        }
    
    //Name    
    if ( aMedia.IsSupported( KMPXMediaGeneralTitle ) )
        {
        details->iTitle = aMedia.ValueText( KMPXMediaGeneralTitle ).AllocL();                
        }
    else if ( details->iFilePath )
        {
        details->iTitle = details->iFilePath->AllocLC();
        }
        
    //Duration
    if ( aMedia.IsSupported( KVcxMediaMyVideosDuration ) )
        {
        TReal32 length = *(aMedia.Value<TReal32>( KVcxMediaMyVideosDuration ));
        if ( length > 0 )
            {
            details->iDurationInSeconds = static_cast<TUint>( length );                
            }   
        }
    
    //File size
    if ( aMedia.IsSupported( KMPXMediaGeneralExtSizeInt64 ) )
       {
       details->iSize = *(aMedia.Value<TInt64>( KMPXMediaGeneralExtSizeInt64 ));           
       }
    
    //Copyright
    if ( aMedia.IsSupported( KMPXMediaGeneralCopyright ) )
        {
        details->iCopyright = aMedia.ValueText( KMPXMediaGeneralCopyright ).AllocL();        
        }
    
    //MIME
    if ( aMedia.IsSupported( KMPXMediaGeneralMimeType ) )
        {
        details->iFormat = aMedia.ValueText( KMPXMediaGeneralMimeType ).AllocL();        
        }
    
    // File creation date
    if ( aMedia.IsSupported( KMPXMediaGeneralDate ) )
        {        
        details->iTime = *aMedia.Value<TTime>( KMPXMediaGeneralDate ) ;                
        }
    
    // Bit rate
    if ( aMedia.IsSupported( KMPXMediaVideoBitRate ) )
        {    
        // Multiplied by 1k as the value provided by MDS is kbps and file
        // details dialog assumes it get bps        
        details->iBitrate = *aMedia.Value<TUint16>( KMPXMediaVideoBitRate ) * KThousandNotKilobyte ;                
        }
    
    // Height
    if ( aMedia.IsSupported( KMPXMediaVideoHeight ) )
        { 
        details->iResolutionHeight = *aMedia.Value<TUint16>( KMPXMediaVideoHeight );                
        }
    
    // Width
    if ( aMedia.IsSupported( KMPXMediaVideoWidth ) )
        {
        details->iResolutionWidth = *aMedia.Value<TUint16>( KMPXMediaVideoWidth );                
        }
    
    //Artist    
   if ( aMedia.IsSupported( KMPXMediaVideoArtist ) )
       {
       details->iArtist = aMedia.ValueText( KMPXMediaVideoArtist ).AllocL();                
       } 

    // Show details dialog
    CMPFileDetailsDialog* detailsDialog = CMPFileDetailsDialog::NewL(); 
    detailsDialog->ExecuteLD( details );

    CleanupStack::PopAndDestroy( details );
    }
