/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

// Version : %version: 12 %


#include <e32std.h>
#include <e32base.h>
#include <eikenv.h>
#include <f32file.h>

#include <avkon.hrh>
#include <bautils.h>
#include <akndiscreetpopup.h>
#include <tultextresourceutils.h>
#include <data_caging_path_literals.hrh>

#include <vcxmyvideosdefs.h>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <vcxmyvideosuids.h>
#include <videoplayeractivationmessage.h>
#include <videocontentharvester.rsg>

#include "videochmcategorylistener.h"
#include "videocontentharvesterplugin.h"
#include "videonotificationlauncher.h"
#include "videochpublishabledata.h"
#include "videochpublisher.h"
#include "iptvlastwatchedapi.h"
#include "iptvlastwatcheddata.h"
#include "IptvDebug.h"

// CONSTANTS
const TInt KMediaExtSize( 8 );

_LIT( KResourceFile, "videocontentharvester.rsc");



// ============================ MEMBER FUNCTIONS ==============================


// ----------------------------------------------------------------------------
// CVcxNsNotificationLauncher::CVcxNsNotificationLauncher
// C++ default constructor
// ----------------------------------------------------------------------------
//
CVcxNsNotificationLauncher::CVcxNsNotificationLauncher( CVcxNsChPublishableData* aPublishableData,
                                                         CVcxNsChPublisher* aPublisher ) :
    iPublishableData( aPublishableData ),
    iPublisher( aPublisher )
    {          
    }

// ----------------------------------------------------------------------------
// CVcxNsNotificationLauncher::ConstructL
// ----------------------------------------------------------------------------
//
void CVcxNsNotificationLauncher::ConstructL()
    {
    iDownloadCompletedBuffer = LoadResourceStringL( R_IPTV_DOWNLOAD_COMPLETE );
    
    iDownloadsContinueBuffer = LoadResourceStringL( R_IPTV_UNFINISHED_DNLOADS_CONTINUE );
    
    TUid domain   = TUid::Uid( KVcxNsMpxMediatorDomain );
    TUid category = TUid::Uid( KVcxNsMpxMediatorCategory );
    TVersion version = TVersion( KVcxNsMpxEventVersion, 0, 0 );
    iCatListener = CVcxNsChMCategoryListener::NewL( this, 
                                                    domain,
                                                    category,
                                                    version );
    
    iCatListener->RegisterAndSubscribeL( KVcxNsMpxEventDownloadCompleted );
    }

// ----------------------------------------------------------------------------
// CVcxNsNotificationLauncher::NewL
// ----------------------------------------------------------------------------
//
CVcxNsNotificationLauncher* CVcxNsNotificationLauncher::NewL( 
                                     CVcxNsChPublishableData* aPublishableData,
                                     CVcxNsChPublisher* aPublisher )
    {
    CVcxNsNotificationLauncher* self = 
        new( ELeave ) CVcxNsNotificationLauncher( aPublishableData, aPublisher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CVcxNsNotificationLauncher::~CVcxNsNotificationLauncher
// ----------------------------------------------------------------------------
//
CVcxNsNotificationLauncher::~CVcxNsNotificationLauncher()
     {   
     delete iCatListener;
     delete iDownloadCompletedBuffer;
     delete iDownloadsContinueBuffer;
     }

// ----------------------------------------------------------------------------
// CVcxNsNotificationLauncher::MediatorEventL
// ----------------------------------------------------------------------------
//
void CVcxNsNotificationLauncher::MediatorEventL( TUid aDomain,
                                                 TUid aCategory,
                                                 TInt aEventId,
                                                 const TDesC8& aData )
    {
    IPTVLOGSTRING_LOW_LEVEL("Cvcxnsnotificationlauncher::MediatorEventL IN");
        
    TBool validData( aData.Length() == KMediaExtSize );
    TBool validDomain( aDomain == TUid::Uid( KVcxNsMpxMediatorDomain ) ); 
    TBool validCategory( aCategory == TUid::Uid( KVcxNsMpxMediatorCategory ) );
    TBool validEvent( aEventId == KVcxNsMpxEventDownloadCompleted  );
    
    if ( validDomain && validCategory && validEvent && validData )
        {                
        HBufC8* buffer = HBufC8::NewL( KMediaExtSize );
        CleanupStack::PushL( buffer );

        TPtr8 des = buffer->Des();
        des.Copy( aData );
        
        RDesReadStream stream;
        CleanupClosePushL( stream );
        stream.Open( aData );

        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL( media );

        media->InternalizeL( stream );

        if ( media->IsSupported( KMPXMediaGeneralTitle ) )
            {
            DlCompletedNotificationL( *media );
            }

        CleanupStack::PopAndDestroy( media );
        CleanupStack::PopAndDestroy( &stream );
        CleanupStack::PopAndDestroy( buffer );
        }
    
    IPTVLOGSTRING_LOW_LEVEL("Cvcxnsnotificationlauncher::MediatorEventL OUT");
    }

// ----------------------------------------------------------------------------
// CVcxNsNotificationLauncher::LaunchExitingNotificationL
// ----------------------------------------------------------------------------
//
void CVcxNsNotificationLauncher::LaunchExitingNotificationL()
    {
    CAknDiscreetPopup::ShowGlobalPopupL( *iDownloadsContinueBuffer );
    }

// ----------------------------------------------------------------------------
// CVcxNsNotificationLauncher::DlCompletedNotificationL 
// ----------------------------------------------------------------------------
//
void CVcxNsNotificationLauncher::DlCompletedNotificationL( const CMPXMedia& aMedia )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsNotificationLauncher::DlCompletedNotificationL");            

    TPtrC name = aMedia.ValueText( KMPXMediaGeneralTitle );

    TUint popupFlags( KAknDiscreetPopupDurationLong );
    
    const TInt KMaxDlCompleteTextLength = 300;
    
    TBuf<KMaxDlCompleteTextLength> text;
    
    TulTextResourceUtils::Format( text, *iDownloadCompletedBuffer, -1, name );

    CAknDiscreetPopup::ShowGlobalPopupL(
           text,
           KNullDesC,
           KAknsIIDNone,
           KNullDesC,
           -1,
           -1,
           popupFlags );
    }
//
// ----------------------------------------------------------
//  LoadResourceString.
// ----------------------------------------------------------
//
HBufC* CVcxNsNotificationLauncher::LoadResourceStringL( TInt aResourceId )
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);  

    TFileName dllName;
    Dll::FileName( dllName );
    TParsePtrC parse( dllName );
    
    TFileName fileName;
    fileName = parse.Drive();
    fileName.Append( KDC_APP_RESOURCE_DIR );
    fileName.Append( KResourceFile() );
    
    BaflUtils::NearestLanguageFile( fs, fileName );
    
    if( !BaflUtils::FileExists( fs, fileName ) )
        {
        User::Leave(KErrNotFound);
        }
    
    RResourceFile resourceFile;
    resourceFile.OpenL( fs,fileName );
    CleanupClosePushL( resourceFile );
    resourceFile.ConfirmSignatureL( 0 );

    HBufC8* dataBuffer = resourceFile.AllocReadLC( aResourceId );
    TResourceReader theReader;
    theReader.SetBuffer( dataBuffer );
    HBufC* locstring = theReader.ReadHBufCL();
    
    CleanupStack::PopAndDestroy( dataBuffer );
    CleanupStack::PopAndDestroy( &resourceFile );
    CleanupStack::PopAndDestroy( &fs );

    return locstring;
    }
