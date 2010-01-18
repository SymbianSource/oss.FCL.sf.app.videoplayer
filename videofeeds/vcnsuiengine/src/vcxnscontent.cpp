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
* Description: 
*
*/

// Version : %version: 38 %

#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <vcxmyvideosdefs.h>

#include "CIptvUtil.h"
#include "vcxnscontent.h"
#include "vcxnscontentaccess.h"
#include "vcxnsmpxcollectionclienthandler.h"

// -----------------------------------------------------------------------------
// CVcxNsContent::CVcxNsContent()
// -----------------------------------------------------------------------------
//
CVcxNsContent::CVcxNsContent()
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::~CVcxNsContent()
// -----------------------------------------------------------------------------
//
CVcxNsContent::~CVcxNsContent( )
    {
    delete iUserName;
    delete iPasswd;
    delete iBrowserUrl;
    delete iIconPath;
    delete iName;
    delete iLanguage;
    delete iDescription;
    delete iAuthor;
    delete iCopyright;
    
    iContentAccesses.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsContent::NewL()
    {
    CVcxNsContent* self = new ( ELeave ) CVcxNsContent;
    
    return self;
    }


// -----------------------------------------------------------------------------
// CVcxNsContent::NewL
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsContent::NewL( CVcxNsContent& aContent )
    {
    CVcxNsContent* self = new ( ELeave ) CVcxNsContent ();
    CleanupStack::PushL( self );

    self->iServiceId  = aContent.iServiceId;
    self->iContentId  = aContent.iContentId;
    self->iSize = aContent.iSize; 
    self->iLength = aContent.iLength;
    self->iAgeProfile = aContent.iAgeProfile;
    self->iPendingCommand = aContent.iPendingCommand;
    self->iCmdTransactionId = aContent.iCmdTransactionId;
    self->iDlFailNotified = aContent.iDlFailNotified;    
    self->iFullDetailsFetched = aContent.iFullDetailsFetched;
	
    self->SetNameL( aContent.GetName() );
    self->SetLanguageL( aContent.GetLanguage() );
    self->SetDescriptionL( aContent.GetDescription() );
    self->SetAuthorL( aContent.GetAuthor() );
    self->SetCopyrightL( aContent.GetCopyright() ) ;
    self->SetUsernameL( aContent.GetUsername() );
    self->SetPasswordL( aContent.GetPassword() );
    self->SetIconPathL( aContent.GetIconPath() );
    
    for ( TInt i = 0; i < aContent.iContentAccesses.Count(); i++ )        
        {
        CVcxNsContentAccess* ca = CVcxNsContentAccess::NewL();
        CleanupStack::PushL( ca );
        CVcxNsContentAccess* cb = aContent.iContentAccesses[i];
        
        ca->SetMpxMediaL( cb->GetMpxMedia() );
        ca->SetUrlL( cb->GetUrl() );
        
        ca->iIndex = cb->iIndex;
        ca->iLastPosition = cb->iLastPosition;
        ca->iMpxId = cb->iMpxId;
        ca->iType = cb->iType;
        
        self->AddContentAccess( ca );
        CleanupStack::Pop( ca );
        }
    
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetName()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsContent::GetName()
    {
    if( iName && iName->Length() > 0 )
        {
        return *iName;
        }
    
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetLanguage()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsContent::GetLanguage()
    {
    if( iLanguage && iLanguage->Length() > 0 )
        {
        return *iLanguage;
        }
    
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetDescription()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsContent::GetDescription()
    {
    if( iDescription && iDescription->Length() > 0 )
        {
        return *iDescription;
        }
    
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetAuthor()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsContent::GetAuthor()
    {
    if( iAuthor && iAuthor->Length() > 0 )
        {
        return *iAuthor;
        }
    
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetCopyright()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsContent::GetCopyright()
    {
    if( iCopyright && iCopyright->Length() > 0 )
        {
        return *iCopyright;
        }
    
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetIconPath()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsContent::GetIconPath()
    {
    if( iIconPath && iIconPath->Length() > 0 )
        {
        return *iIconPath;
        }
    
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetServiceId()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsContent::GetServiceId()
    {
    return iServiceId;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetContentId()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsContent::GetContentId()
    {
    return iContentId;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetSize()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsContent::GetSize()
    {
    return iSize;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetLength()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsContent::GetLength()
    {
    return iLength;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetBrowserUrl
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsContent::GetBrowserUrl()
    {
    if( iBrowserUrl && iBrowserUrl->Length() > 0 )
         {
         return *iBrowserUrl;
         }
     
     return KNullDesC;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TUint16 CVcxNsContent::GetAgeProfile()
    {
    return iAgeProfile;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetUsernameL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsContent::SetUsernameL( const TDesC& aUsername )
    {
    delete iUserName;
    iUserName = NULL;
    
    if( aUsername.Length() > 0 )
        {
        iUserName = aUsername.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetPasswordL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsContent::SetPasswordL( const TDesC& aPassword )
    {
    delete iPasswd;
    iPasswd = NULL;
    
    if( aPassword.Length() > 0 )
        {
        iPasswd = aPassword.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetUsername
// -----------------------------------------------------------------------------
//
const TDesC& CVcxNsContent::GetUsername()
    {
    if( iUserName && iUserName->Length() > 0 )
         {
         return *iUserName;
         }
     
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetPassword
// -----------------------------------------------------------------------------
//
const TDesC& CVcxNsContent::GetPassword()
    {
    if( iPasswd && iPasswd->Length() > 0 )
         {
         return *iPasswd;
         }
     
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsContent::TVcxContentDlStatus CVcxNsContent::GetDlStatus()
    {
    CVcxNsContentAccess* ca = GetContentAccess( EVcxContentTypeVideo );
    
    if( !ca )
        {
        return EVcxContentDlStateNotDownloadable;
        }
    
    switch( iPendingCommand )
        {
        case EVcxNsMpxContentCommandWaitingForMpxItem:
        case EVcxNsMpxContentCommandStartDownload:
        case EVcxNsMpxContentCommandResumeDownload:
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsContent::GetDlStatus() iPendingCommand has value -> returning EVcxContentDlStateDownloading");
            
            return EVcxContentDlStateDownloading;
            }
        case EVcxNsMpxContentCommandCancelDownload:
            {
            if ( !GetMpxMedia(CVcxNsContent::EVcxContentTypeVideo) )
                {
                IPTVLOGSTRING_LOW_LEVEL("CVcxNsContent::GetDlStatus() iPendingCommand = EVcxNsMpxContentCommandCancelDownload -> returning EVcxContentDlStateNotFound");
                return EVcxContentDlStateNotFound;
                }
            else
                {
                return EVcxContentDlStateCanceling;
                }
            }
        default:
            break;
        }
    
    if( ca->GetMpxMedia() )
        {
        TVcxContentDlStatus status = ( TVcxContentDlStatus ) ca->GetMpxMedia()->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );

        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsContent::GetDlStatus() media object download state = %d", status);
        
        TUint32  dlId = ca->GetMpxMedia()->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadId );

        if ( dlId == 0 ) 
            {
            // MPX item found and dl id = 0 -> item is downloaded.
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsContent::GetDlStatus() dl id = 0 -> returning EVcxContentDlStateDownloaded");
            return EVcxContentDlStateDownloaded;
            }
        else 
            {
            return status;
            }
        }

    return EVcxContentDlStateNotFound;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt8 CVcxNsContent::GetDlProgress()
    {
    CVcxNsContentAccess* ca = GetContentAccess( EVcxContentTypeVideo );
    
    if( ca && ca->GetMpxMedia() )
        {
        TInt8 progress = ca->GetMpxMedia()->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );

        const TInt KFullPercents = 100;

        if( progress > 0 && progress <= KFullPercents )
            {
            return progress;
            }
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::GetContentTypes()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsContent::GetContentTypes()
    {
    TUint32 flags( 0 );
    
    for( TInt idx = iContentAccesses.Count()-1; idx >= 0; idx-- )
        {
        flags |= ( TUint32 ) (iContentAccesses[idx])->iType;
        }
    
    return flags;
    }


// -----------------------------------------------------------------------------
// CVcxNsContent::GetUrl()
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsContent::GetUrl( TVcxNsContentAccessType aType )
    {
    CVcxNsContentAccess* ca = GetContentAccess( aType );
    
    if( ca )
        {
        return ca->GetUrl();
        }

    return KNullDesC;        
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetLocalFilepath()
// -----------------------------------------------------------------------------
//
const TDesC& CVcxNsContent::GetLocalFilepath( TVcxNsContentAccessType aType )
    {
    CVcxNsContentAccess* ca = GetContentAccess( aType );
    
    if( ca && ca->GetMpxMedia() )
        {
        return ca->GetMpxMedia()->ValueText( KMPXMediaGeneralUri );
        }

    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetLastPlaybackPosition()
// -----------------------------------------------------------------------------
//
TInt32 CVcxNsContent::GetLastPlaybackPosition( TVcxNsContentAccessType aType )
    {
    CVcxNsContentAccess* ca = GetContentAccess( aType );
    
    if ( ca && CIptvUtil::LastPlaybackPositionFeatureSupported() )
        {
        if( aType == EVcxContentTypeVideo )
            {
            TInt position( 0 );
            CMPXMedia* media( ca->GetMpxMedia() );
            if ( media && media->IsSupported( KMPXMediaGeneralLastPlaybackPosition ) )
                {
                position = *media->Value<TInt>( KMPXMediaGeneralLastPlaybackPosition );
                }
            return position > 0 ? position : 0;
            }
        else if ( aType == EVcxContentTypeStream )
            {
            return ca->iLastPosition;
            }
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetLastPlaybackPosition()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetLastPlaybackPosition( const TReal32 aPos, TVcxNsContentAccessType aType )
    {
    CVcxNsContentAccess* ca = GetContentAccess( aType );

    if ( ca && CIptvUtil::LastPlaybackPositionFeatureSupported() )
        {
        ca->iLastPosition = aPos;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetNameL()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetNameL( const TDesC& aName )
    {
    delete iName;
    iName = NULL;
    
    if( aName.Length() > 0 )
        {
        iName = aName.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetLanguageL()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetLanguageL( const TDesC& aLanguage )
    {
    delete iLanguage;
    iLanguage = NULL;
    
    if( aLanguage.Length() > 0 )
        {
        iLanguage = aLanguage.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetDescriptionL()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetDescriptionL( const TDesC& aDescription )
    {
    delete iDescription;
    iDescription = NULL;
    
    if( aDescription.Length() > 0 )
        {
        iDescription = aDescription.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetAuthorL()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetAuthorL( const TDesC& aAuthor )
    {
    delete iAuthor;
    iAuthor = NULL;
    
    if( aAuthor.Length() > 0 )
        {
        iAuthor = aAuthor.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetCopyrightL()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetCopyrightL( const TDesC& aCopyright )
    {
    delete iCopyright;
    iCopyright = NULL;
    
    if( aCopyright.Length() > 0 )
        {
        iCopyright = aCopyright.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetIconPathL()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetIconPathL( const TDesC& aPath )
    {
    delete iIconPath;
    iIconPath = NULL;
    
    if( aPath.Length() > 0 )
        {
        iIconPath = aPath.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetServiceId()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetServiceId( const TUint32 aServiceId )
    {
    iServiceId = aServiceId;
    }
// -----------------------------------------------------------------------------
// CVcxNsContent::SetContentId()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetContentId( const TUint32 aContentId )
    {
    iContentId = aContentId;
    }
// -----------------------------------------------------------------------------
// CVcxNsContent::SetSize()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetSize( const TUint32 aSize )
    {
    iSize = aSize;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetLength()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetLength( const TUint32 aLength )
    {
    iLength = aLength;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetBrowserUrlL()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetBrowserUrlL( const TDesC& aBrowserUrl )
    {
    delete iBrowserUrl;
    iBrowserUrl = NULL;
    
    if( aBrowserUrl.Length() > 0 )
        {
        iBrowserUrl = aBrowserUrl.AllocL();
        }    
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetAgeProfile()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetAgeProfile( const TUint16 aAgeProfile )
    {
    iAgeProfile = aAgeProfile;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetMpxMediaL()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetMpxMediaL( CMPXMedia* aMpxMedia, TVcxNsContentAccessType aType )
    {
    CVcxNsContentAccess* ca = GetContentAccess( aType );

    if ( iPendingCommand == EVcxNsMpxContentCommandWaitingForMpxItem ||
         iPendingCommand == EVcxNsMpxContentCommandStartDownload ||
         iPendingCommand == EVcxNsMpxContentCommandResumeDownload )
        {
        iPendingCommand = EVcxNsMpxContentCommandNone;
        }
    
    if( ca )
        {
        ca->SetMpxMediaL( aMpxMedia );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetMpxMedia()
// -----------------------------------------------------------------------------
//
CMPXMedia* CVcxNsContent::GetMpxMedia( TVcxNsContentAccessType aType )
    {
    CVcxNsContentAccess* ca = GetContentAccess( aType );
    
    if( ca )
        {
        return ca->GetMpxMedia();
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::GetMpxId()
// -----------------------------------------------------------------------------
//
TUint32 CVcxNsContent::GetMpxId( TVcxNsContentAccessType aType )
    {
    CVcxNsContentAccess* ca = GetContentAccess( aType );
    
    if( ca )
        {
        return ca->iMpxId;
        }
    
    return 0;
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::SetMpxId()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetMpxId( const TUint32 aMpxId, TVcxNsContentAccessType aType )
    {
    CVcxNsContentAccess* ca = GetContentAccess( aType );
    
    if( ca )
        {
        ca->iMpxId = aMpxId;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsContent::AddContentAccess()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::AddContentAccess( CVcxNsContentAccess* aContentAccess )
    {
    iContentAccesses.Append( aContentAccess );
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::GetContentAccess()
// -----------------------------------------------------------------------------
//
CVcxNsContentAccess* CVcxNsContent::GetContentAccess( TVcxNsContentAccessType aType )
    {
    for( TInt idx = iContentAccesses.Count()-1; idx >= 0; idx-- )
        {
        if ( (iContentAccesses[idx])->iType == aType )
            {
            return (iContentAccesses[idx]);
            }
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::SetPendingCommand()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetPendingCommand( TVcxNsMpxContentCommand aCmd )
    {
    iPendingCommand = aCmd;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::PendingCommand()
// -----------------------------------------------------------------------------
//
CVcxNsContent::TVcxNsMpxContentCommand CVcxNsContent::PendingCommand()
    {
    return iPendingCommand;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::SetCmdTransactionId()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetCmdTransactionId( TUint32 aTrId )
    {
    iCmdTransactionId = aTrId;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::CmdTransactionId()
// -----------------------------------------------------------------------------
//
TUint32 CVcxNsContent::CmdTransactionId()
    {
    return iCmdTransactionId;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::SetDlFailNotified()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetDlFailNotified( TBool aDlFailNotified )
    {
    iDlFailNotified = aDlFailNotified;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::DlFailNotified()
// -----------------------------------------------------------------------------
//
TBool CVcxNsContent::DlFailNotified()
    {
    return iDlFailNotified;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::SetFullDetailsFetched()
// -----------------------------------------------------------------------------
//
void CVcxNsContent::SetFullDetailsFetched( TBool aFullDetailsFetched )
    {
    iFullDetailsFetched = aFullDetailsFetched;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpx::FullDetailsFetched()
// -----------------------------------------------------------------------------
//
TBool CVcxNsContent::FullDetailsFetched()
    {
    return iFullDetailsFetched;
    }
