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
void CFileDetailsPlugin::ShowFileDetails( const CMPXMedia& aMedia )
    {  
    CMPFileDetailsDialog* detailsDialog = CMPFileDetailsDialog::NewL(); 
    CMPFileDetails* details = new (ELeave) CMPFileDetails();
    
    //File path    
    if ( aMedia.IsSupported( KMPXMediaGeneralUri ) )
        {
        details->iFilePath = aMedia.ValueText( KMPXMediaGeneralUri ).AllocLC();                
        }
    
    //Name    
    if ( aMedia.IsSupported( KMPXMediaGeneralTitle ) )
        {
        details->iTitle = aMedia.ValueText( KMPXMediaGeneralTitle ).AllocLC();                
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
        details->iCopyright = aMedia.ValueText( KMPXMediaGeneralCopyright ).AllocLC();        
        }
    
    //MIME
    if ( aMedia.IsSupported( KMPXMediaGeneralMimeType ) )
        {
        details->iFormat = aMedia.ValueText( KMPXMediaGeneralMimeType ).AllocLC();        
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
       details->iArtist = aMedia.ValueText( KMPXMediaVideoArtist ).AllocLC();                
       } 
             
    detailsDialog->ExecuteLD( details );
    
    // cleanup
    if( details->iFormat )
        {
        CleanupStack::PopAndDestroy( details->iFormat );
        details->iFormat = NULL;
        }    
    if( details->iCopyright )
        {
        CleanupStack::PopAndDestroy( details->iCopyright );
        details->iCopyright = NULL;
        }
    if( details->iTitle )
        {
        CleanupStack::PopAndDestroy( details->iTitle );
        details->iTitle = NULL;
        }
    if( details->iFilePath )
        {
        CleanupStack::PopAndDestroy( details->iFilePath );
        details->iFilePath = NULL;
        }   
  
    delete details;
    }
