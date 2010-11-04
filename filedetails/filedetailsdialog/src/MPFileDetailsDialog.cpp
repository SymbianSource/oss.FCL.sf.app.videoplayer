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
* Description:   Implementation of the CMPFileDetailsDialog.*
*/


// Version : %version: 22 %

// INCLUDE FILES
#include <aknlists.h>
#include <bautils.h>
#include <StringLoader.h>
#include <e32base.h>
#include <utf.h>
#include <mmf/common/mmferrors.h>
#include <DRMHelper.h>
#include <aknclearer.h>
#include <e32math.h> //for size rounding

#include <MPFileDetails.rsg>
#include "mpxvideo_debug.h"

#include <AiwServiceHandler.h>
#include <AiwCdmaCommands.hrh>
#include <featmgr.h>
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <centralrepository.h>
#include "MediaPlayerVariant.hrh"
#include <MetaDataUtility.h>
#include <MetaDataFieldContainer.h>
#include <caf/content.h>
#include <Oma2Agent.h>

#include "MediaPlayerPrivateCRKeys.h"
#include "MPFileDetailsDialog.h"
#include "mppopuplist.h"
#include <mediarecognizer.h>
#include <streaminglinkmodel.h>
#include "MPFileDetails.h"
#include "MediaPlayerDrmHelper.h"

//
//  Inline methods
//
#include "MPFileDetailsDialog.inl"

//#include "MPAppConsts.h"

#include <drmutility.h>

#ifdef __WINDOWS_MEDIA
#include <asxparser.h>
#endif

// CONSTANTS
_LIT(KResourceFile,"z:MPFileDetails.rsc");

const TInt KOneKilobyte = 1;
const TInt KFileSizeOneKilobyte = 1024;
const TInt KThousandNotKilobyte = 1000;
const TInt KOneSecond(1000000);
const TInt KOneHourInSeconds(3600);
const TInt KMaxTimeLength = 36;
const TInt KMaxFilePathLength = 2048;

// For meta data
_LIT( KMPTitle, "Title" );
_LIT( KMPArtist, "Artist" );
_LIT( KMPAuthor, "Author" );
_LIT( KMPCopyright, "Copyright" );
_LIT( KMPAbstract, "Abstract" );

_LIT( KExt3gpp,     "3gp" );
_LIT( KExtMp4,      "mp4" );
_LIT( KExtM4a,      "m4a" );

_LIT(KDetaMime3gpp,  "audio/3gpp");
_LIT(KDetaMime3gpp2, "audio/3gpp2");
_LIT(KDetaMimeMp4,   "audio/mp4");

_LIT(KFormat,"MimeType");
_LIT(KMPX,   "x");


// ================= OTHER EXPORTED FUNCTIONS ==============

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::CMPFileDetailsDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPFileDetailsDialog::CMPFileDetailsDialog()
    : iResourceFileOffset(0)
    , iResult( KErrNone )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::CMPFileDetailsDialog()"));
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::ConstructL()
{

    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::ConstructL()"));

    LoadResourcesL();
    iListBox = new (ELeave) CAknDoublePopupMenuStyleListBox();
    iGenres = CCoeEnv::Static()->ReadDesCArrayResourceL( R_MP_GENRE_STRINGS );

}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPFileDetailsDialog* CMPFileDetailsDialog::NewL()
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog* CMPFileDetailsDialog::NewL()"));

    CMPFileDetailsDialog* self = new( ELeave ) CMPFileDetailsDialog();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::~CMPFileDetailsDialog
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMPFileDetailsDialog::~CMPFileDetailsDialog()
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::~CMPFileDetailsDialog()"));
    if( iPopupList )
        {
        iPopupList->CancelPopup();
        }
    delete iPopupList;  // cause removal from control stack
    delete iListBox;
    delete iGenres;
    UnLoadResources();
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPFileDetailsDialog::ExecuteLD(const TDesC& aFileName)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::ExecuteLD()"));

    CleanupStack::PushL( this );
    CMPFileDetails* fileDetails = new (ELeave) CMPFileDetails();
    CleanupStack::PushL( fileDetails );

	//
	// Obtain file path first so it can be used later
	//
    fileDetails->iFilePath = aFileName.AllocL();

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    RFile64 fileHandle;
#else
    RFile fileHandle;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    TUint fileMode = EFileShareReadersOrWriters | EFileStream | EFileRead;
    RFs fs = CEikonEnv::Static()->FsSession();

    User::LeaveIfError(fileHandle.Open(fs, aFileName, fileMode));
    CleanupClosePushL(fileHandle);

    DRM::CDrmUtility* drmUtil = DRM::CDrmUtility::NewLC();

    TRAP_IGNORE( fileDetails->iDrmProtected =
        drmUtil->IsProtectedL(fileHandle) );

    CleanupStack::PopAndDestroy(2); // fileHandle, drmUtil

    if ( fileDetails->iDrmProtected )
        {
        //
		// All DRM rights are handled & displayed by DRM component.
        // MP only needs to display DCF header info (Title, Provider, Description).
        //
        FetchDCFHeaderDataL(aFileName, fileDetails);
        }

    // recognize file and get format
    CMediaRecognizer* recognizer = CMediaRecognizer::NewL();
    CleanupStack::PushL(recognizer);
    CMediaRecognizer::TMediaType mediaType =
                recognizer->IdentifyMediaTypeL( aFileName, ETrue );
    // mime type not shown for playlists and ram files
    if ( mediaType != CMediaRecognizer::ELocalRamFile &&
         mediaType != CMediaRecognizer::ELocalAsxFile &&
         mediaType != CMediaRecognizer::ELocalAudioPlaylist )
        {
        fileDetails->iFormat = recognizer->MimeTypeL( aFileName ).AllocL();
        }
    CleanupStack::PopAndDestroy();  // recognizer

    // filename
    TParse parse;
    parse.Set(aFileName,NULL,NULL);
    fileDetails->iFileName = parse.Name().AllocL();

    switch (mediaType)
        {
        case CMediaRecognizer::ELocalVideoFile:
            {
#ifdef SYMBIAN_BUILD_GCE
            CVideoPlayerUtility2* videoPlayer = CVideoPlayerUtility2::NewL(*this,
                EMdaPriorityNormal,EMdaPriorityPreferenceNone );
#else
            CMPDummyController* dummy = CMPDummyController::NewLC();
            TRect tmp;
            CVideoPlayerUtility* videoPlayer = CVideoPlayerUtility::NewL(*this,
                            EMdaPriorityNormal,EMdaPriorityPreferenceNone,
                            dummy->GetClientWsSession(),dummy->GetScreenDevice(),
                            dummy->GetClientWindow(),tmp,tmp);
#endif // SYMBIAN_BUILD_GCE
            CleanupStack::PushL(videoPlayer);
            // open
            videoPlayer->OpenFileL(aFileName);
            // wait open to complete
            iWait.Start();

            if (iResult && iResult != KErrMMPartialPlayback)
                {
#ifdef SYMBIAN_BUILD_GCE
                CleanupStack::PopAndDestroy(1); // videoPlayer
#else
                CleanupStack::PopAndDestroy(2); // dummy and videoPlayer
#endif //SYMBIAN_BUILD_GCE
                break;
                }
            // prepare
            videoPlayer->Prepare();
            // wait prepare to complete
            iWait.Start();

            if (iResult && iResult != KErrMMPartialPlayback)
                {
#ifdef SYMBIAN_BUILD_GCE
                CleanupStack::PopAndDestroy(1); // videoPlayer
#else
                CleanupStack::PopAndDestroy(2); // dummy and videoPlayer
#endif //SYMBIAN_BUILD_GCE
                break;
                }
            // get video file details
            // duration
            fileDetails->iDurationInSeconds =
                    videoPlayer->DurationL().Int64() / KOneSecond;
            // resolution
            TSize size;
            videoPlayer->VideoFrameSizeL(size);
            fileDetails->iResolutionHeight = size.iHeight;
            fileDetails->iResolutionWidth = size.iWidth;
            // bitrate
            fileDetails->iBitrate = videoPlayer->VideoBitRateL() +
                                    videoPlayer->AudioBitRateL();

            // video track
            fileDetails->iVideoTrack = videoPlayer->VideoBitRateL();

            TPtrC8 videoFormat = videoPlayer->VideoFormatMimeType();

            delete fileDetails->iFormat;
            fileDetails->iFormat = NULL;

            if ( videoFormat.Length())
            {
                HBufC* videoFormat16 = HBufC::NewLC(videoFormat.Length());
                TPtr videoFormatDes16(videoFormat16->Des());
                videoFormatDes16.Copy(videoFormat);

                fileDetails->iFormat = videoFormatDes16.AllocL();
                CleanupStack::PopAndDestroy();   //videoFormat16
            }

            // copyright & title & format
            TInt metaCount = videoPlayer->NumberOfMetaDataEntriesL();
            CMMFMetaDataEntry* metaData;
            for (TInt i = 0; i < metaCount; i++)
                {
                metaData = videoPlayer->MetaDataEntryL(i);
                if (!metaData->Name().CompareF(KMPCopyright))
                    {
                    fileDetails->iCopyright = metaData->Value().AllocL();
                    }
                else if (!metaData->Name().CompareF(KMPArtist) || !metaData->Name().Compare(KMPAuthor))
                    {
                    fileDetails->iArtist = metaData->Value().AllocL();
                    }
                else if (!metaData->Name().CompareF(KMPTitle))
                    {
                    fileDetails->iTitle = metaData->Value().AllocL();
                    }
                else if (!metaData->Name().CompareF(KMPAbstract))
                     {
                          fileDetails->iDescription = metaData->Value().AllocL();
                     }
                else if (!(metaData->Name().CompareF(KFormat) || fileDetails->iFormat))
                     {
                          fileDetails->iFormat = metaData->Value().AllocL();
                     }
                delete metaData;
                }
#ifdef SYMBIAN_BUILD_GCE
            CleanupStack::PopAndDestroy(1); // videoPlayer
#else
            CleanupStack::PopAndDestroy(2); // dummy and videoPlayer
#endif //SYMBIAN_BUILD_GCE
            }
            break;
        case CMediaRecognizer::ELocalRamFile:
            {
            // get first link
            CStreamingLinkModel* linkModel = CStreamingLinkModel::NewL();
            CleanupStack::PushL(linkModel);
            User::LeaveIfError(linkModel->OpenLinkFileL( aFileName, EFalse ));
            fileDetails->iUrl = HBufC::NewL(linkModel->MaxLinkLength());
            TPtr filePtr = fileDetails->iUrl->Des();
            TBool dummy;
            linkModel->GetNextLinkL(filePtr,dummy);
            CleanupStack::PopAndDestroy(); // linkModel
            }
            break;
        case CMediaRecognizer::ELocalAsxFile:
            {
#ifdef __WINDOWS_MEDIA
			CAsxParser* asxParser = NULL;
			TRAPD(err, asxParser = CAsxParser::NewL( aFileName ));
			CleanupStack::PushL( asxParser );

			if ( !err && asxParser && asxParser->FileVersion() > 0 )
				{
				TUint asxUrlCount = 0;
				TPtrC8 urlPtr;

				asxParser->GetUrlCount(asxUrlCount);
				if ( asxUrlCount )
					{
					// get first link
					asxParser->GetUrl(1,urlPtr);
					fileDetails->iUrl = HBufC::NewL(urlPtr.Length());
					TPtr filePtr = fileDetails->iUrl->Des();
					asxParser->PrintUrl(urlPtr,filePtr);
					}
				}
			CleanupStack::PopAndDestroy();  // asxparser
#endif
            }
            break;
        case CMediaRecognizer::ELocalAudioFile:
            {
            FetchMetaDataL( aFileName, fileDetails );
            }
            break;
        default:
            break;
        }


    if (mediaType != CMediaRecognizer::ELocalRamFile &&
		mediaType != CMediaRecognizer::ELocalAsxFile )
        {
        RFs fs = CEikonEnv::Static()->FsSession();
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        RFile64 file; 
#else
        RFile file;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        TInt error = file.Open(fs, aFileName, EFileRead | EFileShareReadersOnly);
        if (error)
            {
            // Try different sharing method if the first one failed.
            error = file.Open(fs, aFileName, EFileRead | EFileShareAny);
            }

        if (!error)
            {
            // Set item size, ignore error code
            if (mediaType != CMediaRecognizer::ELocalAudioPlaylist)
                {
                file.Size(fileDetails->iSize);
                }
            // Set modification date & time, ignore error code
            file.Modified(fileDetails->iTime);
            file.Close();
            }
        }

    // Show File Details in differend ExecuteLD
    ExecuteLD( fileDetails );
    CleanupStack::PopAndDestroy(); // fileDetails
    CleanupStack::Pop(); // this
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPFileDetailsDialog::ExecuteLD(CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::ExecuteLD()"));

    if ( aFileDetails->iDrmProtected && aFileDetails->iFilePath )
    {
        // drm data
        FetchDrmDataL( *aFileDetails->iFilePath, aFileDetails );
    }


    if ( iPopupList )
    {
        iPopupList->CancelPopup();
        iPopupList = NULL;
    }

    //
    //  use Media Player's own PopupList, not Avkon's PopupList
    //
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    TInt64 size(0);
#else
    TInt size(0);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    TInt error(0);
    TInt handleExists(0);
    if (  aFileDetails->iDetailsFileHandle.SubSessionHandle() )
    {
        error = aFileDetails->iDetailsFileHandle.Size( size );
    }

    handleExists =(!error && size )? ETrue:EFalse;

    if( handleExists )
    {
        iPopupList = CMPPopupList::NewL( iListBox,
                                         aFileDetails->iDetailsFileHandle,
                                         aFileDetails->iDrmProtected,
                                         aFileDetails->iReadingDCFOnly );
    }
    else
    {
        //
        //  file path is not available when this ExecuteLD(aFileDetails) method is
        //	launched by MP Video View for streaming link. Hence, this value is set
        //  to Null in such case.
        //
        HBufC* temp = HBufC::NewL(KMaxFilePathLength);
        CleanupStack::PushL( temp );
        TPtr filePathPtr = temp->Des();

        if ( aFileDetails->iFilePath )
        {
            filePathPtr.Append( aFileDetails->iFilePath->Des() );
        }
        else
        {
            filePathPtr.Append( KNullDesC() );
        }

        //
        //  use Media Player's own PopupList, not Avkon's PopupList
        //
        iPopupList = CMPPopupList::NewL( iListBox,
                                     filePathPtr,
                                     aFileDetails->iDrmProtected,
                                     aFileDetails->iReadingDCFOnly );

        CleanupStack::PopAndDestroy( temp );
    }

    TRAPD( err,
    {
        iListBox->ConstructL(iPopupList, CEikListBox::EDisableHighlight );
        iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

        iListBox->CreateScrollBarFrameL(ETrue);
        iListBox->ScrollBarFrame()->
            SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                     CEikScrollBarFrame::EAuto );

        SetTitleL(iPopupList);
        FillListBoxL(aFileDetails);

        iPopupList->ExecuteLD();
        iPopupList = NULL;
    } );//TRAP

	//explicitly cancel the Popup if it's not terminated properly
    if ( err && iPopupList )
    {
        iPopupList->CancelPopup();
    }

    delete this;
    
    if ( KLeaveExit == err )
        {
        User::Leave( err );
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::UnLoadResourceFile
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::UnLoadResources()
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::UnLoadResources()"));

    if (iResourceFileOffset > 0)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceFileOffset);
        iResourceFileOffset = 0;
        }
}
// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::LoadResourceFileL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::LoadResourcesL()
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::LoadResourcesL()"));

    if (iResourceFileOffset == 0)
        {
        CCoeEnv* enviro = CCoeEnv::Static();
        TFileName resourceFile;

        //
        // Get the path & file name with the drive not specified
        //
        _LIT(KMPTempFile,"MPFileDetails.rsc");
        TParse parse;
        parse.Set(KMPTempFile, &KDC_APP_RESOURCE_DIR, NULL);
        TPtrC rscFile = parse.FullName();

        //
        // This is done to ensure upgraded file is used first.
        // If no upgraded file is found, default file in Z: drive will be used.
        //
        TFindFile find( CCoeEnv::Static()->FsSession() );
        TInt err = find.FindByDir( rscFile, KNullDesC );
        MPX_DEBUG(_L("CMPFileDetailsDialog::LoadResourcesL() : err(%d)"),err );

        if ( err )
        {
            parse.Set(KResourceFile, &KDC_APP_RESOURCE_DIR, NULL);
            TPtrC defaultRscFile = parse.FullName();
            resourceFile.Append( defaultRscFile );
            MPX_DEBUG(_L("CMPFileDetailsDialog::LoadResourcesL() : defaultRsc(%S)"),&resourceFile );
        }
        else
        {
            resourceFile.Append( find.File() );
            MPX_DEBUG(_L("CMPFileDetailsDialog::LoadResourcesL() : upgradeRsc(%S)"),&resourceFile );
        }

        BaflUtils::NearestLanguageFile( enviro->FsSession(), resourceFile );
        iResourceFileOffset = enviro->AddResourceFileL( resourceFile );
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::SetTitleL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::SetTitleL(CAknPopupList* aPopupList)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::SetTitleL()"));

    HBufC* title = StringLoader::LoadLC(R_MP_FILE_DETAILS);
    aPopupList->SetTitleL(*title);
    CleanupStack::PopAndDestroy(); //title
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::FillListBoxL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::FillListBoxL(CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::FillListBoxL()"));

    CDesCArray* itemArray =
        static_cast<CDesCArray*>( iListBox->Model()->ItemTextArray() );

    TBool drmProtected = aFileDetails->iDrmProtected;

    // re-evaluate drm protection
    // for playlists it may be inaccurate
    if ( ! drmProtected )
    {
        if ( aFileDetails->iDetailsFileHandle.SubSessionHandle() )
        {
            CContent* content = CContent::NewL( aFileDetails->iDetailsFileHandle );
            content->GetAttribute( EIsProtected, drmProtected );
            delete content;
        }
        else if (aFileDetails->iFilePath != NULL)
        {
            CContent* content = CContent::NewL( aFileDetails->iFilePath->Des() );
            content->GetAttribute( EIsProtected, drmProtected );
            delete content;
        }

        //
        // since 'aFileDetails->iDrmProtected' is EFalse earlier
        // and now it becomes ETrue after the re-evaluation,
        // the related softkeys need to be re-initalized here
        //
        if ( drmProtected )
        {
            iPopupList->ReConstructDrmSoftkeysL(drmProtected);
        }
    }

    // if content is drm protected, fetch the drm data
    if ( drmProtected && aFileDetails->iFilePath )
    {
		FetchDrmDataL( *aFileDetails->iFilePath, aFileDetails );
    }

    if ( drmProtected && ! aFileDetails->iReadingDCFOnly )
    {
        //
        // All DRM rights & constraints are handled & displayed by DRM component.
        // So, MP only needs to display the Heading here.
        //

		//
		// According to UI Spec, DRM info is always the 1st entry of the Details dlg.
		// The softkeys of the Details pop-up (CMPPopupList) are constructed based
        // on this info. Hence, if the order entry of DRM is changed, softkeys in
        // CMPPopupList need to be updated accordingly.
		//
        DisplayDrmHeadingInfoL( itemArray );
    }

    MakeTitleItemL(itemArray,aFileDetails);
    MakeArtistItemL(itemArray,aFileDetails);
    MakeOriginalArtistItemL(itemArray,aFileDetails);
    MakeAlbumItemL(itemArray,aFileDetails);
    MakeAlbumTrackItemL(itemArray,aFileDetails);
    MakeGenreItemL(itemArray,aFileDetails);
    MakeComposerItemL(itemArray,aFileDetails);
    MakeProviderItemL(itemArray,aFileDetails);
    MakeFormatItemL(itemArray,aFileDetails);
    MakeResolutionItemL(itemArray,aFileDetails);
    MakeDurationItemL(itemArray,aFileDetails);
    MakeBitrateItemL(itemArray,aFileDetails);
    MakeSamplerateItemL(itemArray,aFileDetails);
    MakeSizeItemL(itemArray,aFileDetails);
    MakeDateItemL(itemArray,aFileDetails);
    MakeTimeItemL(itemArray,aFileDetails);

    if ( FeatureManager::FeatureSupported( KFeatureIdLightSurf ) )
    {
        MakeUploadStatusL( itemArray, aFileDetails );
    }

    MakeYearItemL(itemArray,aFileDetails);
    MakeDescriptionItemL(itemArray,aFileDetails);
    MakeCommentItemL(itemArray,aFileDetails);
    MakeUrlItemL(itemArray,aFileDetails);
    MakeAudioFileWebPageItemL(itemArray,aFileDetails);
    MakeCopyrightItemL(itemArray,aFileDetails);

    iListBox->HandleItemAdditionL();
    iListBox->SetCurrentItemIndex(0);
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::AddItemToListBoxL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::AddItemToListBoxL(const TDesC& aHeading,
                                             const TDesC& aValue,
                                             CDesCArray* aItemArray)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::AddItemToListBoxL()"));

    CDesCArrayFlat* items = new (ELeave) CDesCArrayFlat(2);
    CleanupStack::PushL(items);
    items->AppendL(aHeading); //First string (heading)

    //remove tabs from aValue
    HBufC* aValueBuf = aValue.AllocLC();
    TPtr aValuePtr = aValueBuf->Des();

    // remove EndOfString character
    if (aValuePtr.Length() > 0)
        {
        TChar c = aValuePtr[aValuePtr.Length()-1];
        if (c.Eos())
            {
            aValuePtr.Delete(aValuePtr.Length()-1,aValuePtr.Length());
            }
        }

    for( TInt i = 0; i < aValuePtr.Length(); i++ )
        {
        if( aValuePtr[i] == '\t' )
            {
            aValuePtr[i] = ' ';
            }
        }
    items->AppendL(aValuePtr);   //Second string (value)
    CleanupStack::PopAndDestroy( aValueBuf );  //aValueBuf

    HBufC* headingAndValue =
            StringLoader::LoadLC( R_MP_FILE_DETAILS_ROW_FORMAT, *items );
    aItemArray->AppendL(*headingAndValue);
    CleanupStack::PopAndDestroy(2); // items & headingAndValue
}


// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeTitleItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeTitleItemL( CDesCArray* aItemArray,
                                           CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeTitleItemL()"));

    if (aFileDetails->iTitle)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_TITLE_HEADING);
        AddItemToListBoxL(*heading,*aFileDetails->iTitle,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        return;
        }

    if (aFileDetails->iFileName)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_NAME_HEADING);
        AddItemToListBoxL(*heading,*aFileDetails->iFileName,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}


// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeUrlItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeUrlItemL( CDesCArray* aItemArray,
                                         CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeUrlItemL()"));

    if (aFileDetails->iUrl)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_URL_HEADING);
        AddItemToListBoxL(*heading,*aFileDetails->iUrl,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}


// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeFormatItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeFormatItemL(CDesCArray* aItemArray,
                                           CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeFormatItemL()"));

    if (aFileDetails->iFormat)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_FORMAT_HEADING);
        AddItemToListBoxL(*heading,*aFileDetails->iFormat,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeResolutionItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeResolutionItemL(CDesCArray* aItemArray,
                                               CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeResolutionItemL()"));

    if (aFileDetails->iResolutionWidth && aFileDetails->iResolutionHeight)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_RESOLUTION_HEADING);
        CArrayFix<TInt>* items = new (ELeave) CArrayFixFlat<TInt>(2);
        CleanupStack::PushL(items);
        items->AppendL(aFileDetails->iResolutionWidth);
        items->AppendL(aFileDetails->iResolutionHeight);
        HBufC* value = StringLoader::LoadLC(R_MP_FILE_RESOLUTION,*items);
        TPtr text = value->Des();
        TInt index = text.FindC(KMPX);
        LanguageSpecificNumberConversion( text );
        text.Replace(index,1,KMPX);
        AddItemToListBoxL(*heading,*value,aItemArray);
        CleanupStack::PopAndDestroy(3); // heading,items,value
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeDurationItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeDurationItemL(CDesCArray* aItemArray,
                                             CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeDurationItemL()"));

    TInt64 duration = aFileDetails->iDurationInSeconds;
    if (duration > 0)
        {
        HBufC* dateFormatString;
        if (duration < KOneHourInSeconds)
            {
            // Read time format string from AVKON resource
            dateFormatString = StringLoader::LoadLC(R_QTN_TIME_DURAT_MIN_SEC);
            }
        else
            {
            // Read time format string from AVKON resource
            dateFormatString = StringLoader::LoadLC(R_QTN_TIME_DURAT_LONG);
            }
        TBuf<16> dur;
        TTime durTime = TTime(duration*KOneSecond);
        durTime.FormatL(dur, *dateFormatString);
        LanguageSpecificNumberConversion( dur );
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_DURATION_HEADING);
        AddItemToListBoxL(*heading,dur,aItemArray);
        CleanupStack::PopAndDestroy(2); // heading & dateFormatString
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeBitrateItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeBitrateItemL(CDesCArray* aItemArray,
                                            CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeBitrateItemL()"));

    if (aFileDetails->iBitrate > 0)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_BITRATE_HEADING);
        HBufC* value = StringLoader::LoadLC(
              R_MP_FILE_BITRATE,aFileDetails->iBitrate / KThousandNotKilobyte );
        TPtr text = value->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*value,aItemArray);
        CleanupStack::PopAndDestroy(2); // heading & value
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeSamplerateItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeSamplerateItemL(CDesCArray* aItemArray,
                                               CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeSamplerateItemL()"));

    if (aFileDetails->iSamplerate > 0)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_SAMPLERATE_HEADING);
        HBufC* value = StringLoader::LoadLC(
              R_MP_FILE_SAMPLERATE,aFileDetails->iSamplerate);
        TPtr text = value->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*value,aItemArray);
        CleanupStack::PopAndDestroy(2); // heading & value
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeSizeItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeSizeItemL(CDesCArray* aItemArray,
                                         CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeSizeItemL()"));

    if (aFileDetails->iSize)
    {
        TRealFormat format = TRealFormat(KMaxFileName,2);
        TBuf<KMaxFileName> sizeString;
        HBufC* value;
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_SIZE_HEADING);
        TReal size = aFileDetails->iSize;
        TReal truncsize;
        size = size/KFileSizeOneKilobyte;
        // Show kilobytes with 3 digits. If more digits needed,
        // show size in megabytes. Only round up if showing kB
        if ( size < KThousandNotKilobyte )
        {
            if (size > KOneKilobyte) // CR rounding only if size between 1kB and 1MB
            { // rounding for Gallery/Messaging display equivalency
                const TReal tempsize = size; // need const
                TInt err = Math::Int(truncsize, tempsize);
                if (err == KErrNone) // need to catch error after convert to Tint
                {
                    if ((size > truncsize) && (size < truncsize + 1))
                    {
                        size =  truncsize +1;
                    }
                // else do nothing, size will be used
                } // if conversion failed, keep it TReal
            }
            sizeString.Num(size,format);
            switch (sizeString.Length())
            {
                case 6:
                    sizeString = sizeString.Left(3);
                    break;
                case 5:
                    sizeString = sizeString.Left(4);
                    break;
                default:
                    break;
            }
            value = StringLoader::LoadLC(R_MP_FILE_SIZE_KB,sizeString);
        }
        else
        {
            size = size/KFileSizeOneKilobyte;
            sizeString.Num(size,format);
            switch (sizeString.Length())
            {
                case 6:
                    sizeString = sizeString.Left(3);
                    break;
                case 5:
                    sizeString = sizeString.Left(4);
                    break;
                default:
                    break;
            }
            value = StringLoader::LoadLC(R_MP_FILE_SIZE_MB,sizeString);
        }
        TPtr text = value->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*value,aItemArray);
        CleanupStack::PopAndDestroy(2); // heading & value
    }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeDateItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeDateItemL(CDesCArray* aItemArray,
                                         CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeDateItemL()"));

    if (aFileDetails->iTime > TTime(0))
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_DATE_HEADING);
        HBufC* dateFormatString =
                StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
        TBuf<KMaxTimeLength> date;
        TTime localDate = aFileDetails->iTime;
        localDate.FormatL(date,*dateFormatString);
        LanguageSpecificNumberConversion( date );
        AddItemToListBoxL(*heading,date,aItemArray);
        CleanupStack::PopAndDestroy(2); // heading & dateFormatString
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeTimeItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeTimeItemL(CDesCArray* aItemArray,
                                         CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeTimeItemL()"));

    if (aFileDetails->iTime > TTime(0))
        {
        HBufC* heading = StringLoader::LoadLC( R_MP_FILE_TIME_HEADING );
        HBufC* timeFormatString =
                StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );
        TBuf<32> time;
        TTime localTime = aFileDetails->iTime;
        localTime.FormatL(time,*timeFormatString);
        LanguageSpecificNumberConversion( time );
        AddItemToListBoxL(*heading,time,aItemArray);
        CleanupStack::PopAndDestroy(2); // heading & timeFormatString
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeUploadStatusL
// -----------------------------------------------------------------------------
//

void CMPFileDetailsDialog::MakeUploadStatusL(CDesCArray* aItemArray,
                                             CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeUploadStatusL()"));

    if(aFileDetails->iVideoTrack)
        {
        HBufC* heading = StringLoader::LoadLC( R_QTN_MP_FILE_UPLOAD_STATUS );

        CAiwServiceHandler* AIWServiceHandler = CAiwServiceHandler::NewLC();

        // AIW generic input param list
        CAiwGenericParamList& mpBaseInputParamsList =
                                AIWServiceHandler->InParamListL();
        // Aiw generic output param list
        CAiwGenericParamList& mpBaseOutputParamsList =
                                AIWServiceHandler->OutParamListL();

        //assign the file name
        TAiwVariant fileNameVariant( aFileDetails->iFileName );
        TAiwGenericParam fileNameParam( EGenericParamFile, fileNameVariant );
        mpBaseInputParamsList.AppendL( fileNameParam );  //file name

        // Execute the AIW service command
        AIWServiceHandler->ExecuteServiceCmdL( KAIWCmdAmsExtendedLightSurfData,
                                            mpBaseInputParamsList,
                                            mpBaseOutputParamsList );

        HBufC* str;
        str = StringLoader::LoadLC(R_QTN_MP_VIDEO_NOT_UPLOADED );
        //if file is uploaded, then only provider will add the result
        //to the output param list
        if ( mpBaseOutputParamsList.Count() > 0)
            {
            TInt Index = 0;
            //check for file type
            const TAiwGenericParam* uploadParam =
                            mpBaseOutputParamsList.FindFirst( Index,
                EGenericParamCharSet, EVariantTypeDesC );
            if(uploadParam != NULL )
                {
                CleanupStack::PopAndDestroy();
                str = StringLoader::LoadLC(R_QTN_MP_VIDEO_UPLOADED );
                }
            }

        AddItemToListBoxL(*heading,*str,aItemArray);
        CleanupStack::PopAndDestroy(3); // heading, str & AIWServiceHandler
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeCopyrightItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeCopyrightItemL(CDesCArray* aItemArray,
                                              CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeCopyrightItemL()"));

    if (aFileDetails->iCopyright)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_COPYRIGHT_HEADING);
        TPtr text = aFileDetails->iCopyright->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*aFileDetails->iCopyright,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeOriginalArtistItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeOriginalArtistItemL(CDesCArray* aItemArray,
                                                   CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeOriginalArtistItemL()"));

    if (aFileDetails->iOriginalArtist)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_ARTIST_ORIGINAL_HEADING);
        TPtr text = aFileDetails->iOriginalArtist->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*aFileDetails->iOriginalArtist,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeAlbumTrackItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeAlbumTrackItemL(CDesCArray* aItemArray,
                                               CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeAlbumTrackItemL()"));

    if (aFileDetails->iAlbumTrack)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_ALBUM_TRACK_HEADING);
        TPtr text = aFileDetails->iAlbumTrack->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*aFileDetails->iAlbumTrack,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeGenreItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeGenreItemL(CDesCArray* aItemArray,
                                          CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeGenreItemL()"));
    if (aFileDetails->iGenre)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_GENRE_HEADING);
        TPtr text = aFileDetails->iGenre->Des();

        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*aFileDetails->iGenre,aItemArray);

        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeComposerItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeComposerItemL(CDesCArray* aItemArray,
                                             CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeComposerItemL()"));

    if (aFileDetails->iComposer)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_COMPOSER_HEADING);
        TPtr text = aFileDetails->iComposer->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*aFileDetails->iComposer,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeAudioFileWebPageItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeAudioFileWebPageItemL(
        CDesCArray* aItemArray,
        CMPFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeAudioFileWebPageItemL()"));

    if (aFileDetails->iAudioFileWebPage)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_INFO_URL_HEADING);
        TPtr text = aFileDetails->iAudioFileWebPage->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*aFileDetails->iAudioFileWebPage,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeCommentItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeCommentItemL(CDesCArray* aItemArray,
                                            CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeCommentItemL()"));

    if (aFileDetails->iComment)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_COMMENT_HEADING);
        TPtr text = aFileDetails->iComment->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*aFileDetails->iComment,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeArtistItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeArtistItemL(CDesCArray* aItemArray,
                                           CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeArtistItemL()"));

    if (aFileDetails->iArtist)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_ARTIST_HEADING);
        AddItemToListBoxL(*heading,*aFileDetails->iArtist,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeAlbumItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeAlbumItemL(CDesCArray* aItemArray,
                                          CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeAlbumItemL()"));

    if (aFileDetails->iAlbum)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_ALBUM_HEADING);
        AddItemToListBoxL(*heading,*aFileDetails->iAlbum,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeYearItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeYearItemL(CDesCArray* aItemArray,
                                         CMPFileDetails* aFileDetails)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeYearItemL()"));

    if (aFileDetails->iYear)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_YEAR_HEADING);
        TPtr text = aFileDetails->iYear->Des();
        LanguageSpecificNumberConversion( text );
        AddItemToListBoxL(*heading,*aFileDetails->iYear,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}


// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::DisplayDrmHeadingInfoL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::DisplayDrmHeadingInfoL(CDesCArray* aItemArray)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::DisplayDrmHeadingInfoL()"));

    //
    // All DRM rights & constraints are handled & displayed by DRM component.
    // So, MP only needs to display the Heading here.
    //

	//
	// License
	//   For details click 'View'
	//
    HBufC* heading = StringLoader::LoadLC(R_QTN_MP_DRM_LICENSE_HEADING);
    HBufC* value = StringLoader::LoadLC(R_QTN_MP_DRM_DETAILS_VIEW);
    AddItemToListBoxL(*heading,*value,aItemArray);
    CleanupStack::PopAndDestroy(2); // heading && value
}


// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::LanguageSpecificNumberConversion
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::LanguageSpecificNumberConversion( TDes& aText ) const
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::LanguageSpecificNumberConversion()"));

    if ( AknTextUtils::DigitModeQuery( AknTextUtils::EDigitModeShownToUser ) )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( aText );
        }
}

// Dummy implementations for audio callback

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MapcInitComplete
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MapcInitComplete(
        TInt aError,
        const TTimeIntervalMicroSeconds& /*aDuration*/)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MapcInitComplete()"));

    // Open completed, resume ExecuteLD(const TDesC& aFileName)
    iResult = aError;
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MapcPlayComplete
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MapcPlayComplete(TInt /*aError*/)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MapcPlayComplete()"));
}


// Dummy implementations for video callback

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MvpuoOpenComplete
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MvpuoOpenComplete(TInt aError)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MvpuoOpenComplete()"));

    // Open completed, resume ExecuteLD(const TDesC& aFileName)
    iResult = aError;
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MvpuoFrameReady
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MvpuoFrameReady( CFbsBitmap& /*aFrame*/,
                                            TInt /*aError*/ )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MvpuoFrameReady()"));
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MvpuoPlayComplete
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MvpuoPlayComplete(TInt /*aError*/)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MvpuoPlayComplete()"));
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MvpuoPrepareComplete
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MvpuoPrepareComplete(TInt aError)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MvpuoPrepareComplete()"));

    // Prepare completed, resume ExecuteLD(const TDesC& aFileName)
    iResult = aError;
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MvpuoEvent
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MvpuoEvent(const TMMFEvent& /*aEvent*/)
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MvpuoEvent()"));
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::FetchMetaDataL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::FetchMetaDataL( const TDesC& aFileName,
                                           CMPFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::FetchMetaDataL()"));

    CMdaAudioPlayerUtility* audioPlayer = CMdaAudioPlayerUtility::NewL(*this);
    CleanupStack::PushL( audioPlayer );
    // Open audio player with the file.
    audioPlayer->OpenFileL( aFileName );
    // Wait open to complete. Active Scheduler is resumed, when the player is
    // initialized. See CMPFileDetailsDialog::MapcInitComplete.
    iWait.Start();

    // iResult is updated according to error code in MapcInitComplete
    if ( iResult )
        {
        // Get metadata
        if( aFileDetails->iDrmProtected )
            {
            switch( aFileDetails->iDRMRightsStatus )
                {
                case EMediaPlayerFull:
                case EMediaPlayerRestricted:
                case KMediaPlayerWmdrmValid:
                    {
                    FetchAdditionalMetaDataL( aFileName, aFileDetails );
                    break;
                    }
                case EMediaPlayerMissing:
                case EMediaPlayerExpired:
                case EMediaPlayerPreview:
                case KMediaPlayerWmdrmExpired:
                    {
                    FetchDCFHeaderDataL( aFileName, aFileDetails );
                    break;
                    }
                default:
                    {
                    // nothing to do
                    break;
                    }
                }
            }
        else
            {
            FetchAdditionalMetaDataL( aFileName, aFileDetails );
            }
        CleanupStack::PopAndDestroy(); // audioPlayer
        return;
        }

    // get audio file details
    aFileDetails->iDurationInSeconds =
            audioPlayer->Duration().Int64() / KOneSecond;

    // Bitrate
    TPckgBuf<TMMFAudioConfig> data;
    const TMMFMessageDestinationPckg destinationPckg(
            KUidInterfaceMMFAudioController );
    TInt err = audioPlayer->CustomCommandSync( destinationPckg,
                                           EMMFAudioControllerGetSourceBitRate,
                                           KNullDesC8, KNullDesC8, data );
    if ( !err )
        {
        aFileDetails->iBitrate = data().iSampleRate;
        }

    // Samplerate
    err = audioPlayer->CustomCommandSync( destinationPckg,
                                        EMMFAudioControllerGetSourceSampleRate,
                                        KNullDesC8, KNullDesC8, data );
    if ( !err )
        {
        aFileDetails->iSamplerate = data().iSampleRate;
        }

    CleanupStack::PopAndDestroy(); // audioPlayer

    // get metadata
    FetchAdditionalMetaDataL( aFileName, aFileDetails );
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::FetchDrmDataL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::FetchDrmDataL( const TDesC& aFileName,
                                          CMPFileDetails* aFileDetails )
{
        MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::FetchDrmDataL()"));

    //
    // Only 'aFileDetails->iDRMRightsStatus' is still needed to display additional meta data.
    // Those additional meta data are obtained in FetchAdditionalMetaDataL() method.
    // All other DRM related info (expire constraints, counts, valid from/until, etc.)
    // are obtained and displayed by DRM Details component.
    //
    CDRMHelper* drmHelper = CDRMHelper::NewLC(*CCoeEnv::Static());
    CMediaPlayerDrmHelper* mediaDrmHelper = CMediaPlayerDrmHelper::NewL(drmHelper);
    CleanupStack::PushL(mediaDrmHelper);

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    TInt64 size(0);
#else
    TInt size(0);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    TInt err(0);

    if ( aFileDetails->iDetailsFileHandle.SubSessionHandle() )
        err = aFileDetails->iDetailsFileHandle.Size( size );

    if (!err && size )
        mediaDrmHelper->LoadDrmUsageRightsInfoL(aFileDetails->iDetailsFileHandle);
    else
        {
        RFs fs = CEikonEnv::Static()->FsSession();
        TInt error = aFileDetails->iDetailsFileHandle.Open(fs, aFileName, EFileRead | EFileShareReadersOnly);
        if (error)
            error = aFileDetails->iDetailsFileHandle.Open(fs, aFileName, EFileRead | EFileShareAny);

        if (!error)
            mediaDrmHelper->LoadDrmUsageRightsInfoL(aFileDetails->iDetailsFileHandle);
        else
            mediaDrmHelper->LoadDrmUsageRightsInfoL(aFileName);
        }

    mediaDrmHelper->GetDrmUsageRightsStatus(aFileDetails->iDRMRightsStatus);

    // cleanup
    CleanupStack::PopAndDestroy(2); // drmHelper, mediaDrmHelper
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeProviderItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeProviderItemL( CDesCArray* aItemArray,
                                              CMPFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeProviderItemL()"));

    if (aFileDetails->iProvider)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_PROVIDER_HEADING);
        AddItemToListBoxL(*heading,*aFileDetails->iProvider,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::MakeDescriptionItemL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::MakeDescriptionItemL( CDesCArray* aItemArray,
                                                 CMPFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::MakeDescriptionItemL()"));

    if (aFileDetails->iDescription)
        {
        HBufC* heading = StringLoader::LoadLC(R_MP_FILE_DESCRIPTION_HEADING);
        AddItemToListBoxL(*heading,*aFileDetails->iDescription,aItemArray);
        CleanupStack::PopAndDestroy(); // heading
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::FetchAdditionalMetaDataL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::FetchAdditionalMetaDataL( const TDesC& aFileName,
                                                     CMPFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::FetchAdditionalMetaDataL()"));

    CMetaDataUtility *metaDataUtility = CMetaDataUtility::NewL();
    CleanupStack::PushL( metaDataUtility );
    TRAPD( err, metaDataUtility->OpenFileL( aFileName ) );
    if( err == KErrNone ) // leaves if file in use
        {
        TInt count = metaDataUtility->MetaDataCount();
        for(TInt i = 0; i < count; i++)
            {
            TMetaDataFieldId fieldId;
            TPtrC field = metaDataUtility->MetaDataFieldsL().At( i, fieldId );
            if( field != KNullDesC )
                {
                switch( fieldId )
                    {
                    case EMetaDataSongTitle:
                        {
                        SetFileDetailL( aFileDetails->iTitle, field );
                        break;
                        }
                    case EMetaDataArtist:
                        {
                        SetFileDetailL( aFileDetails->iArtist, field );
                        break;
                        }
                    case EMetaDataAlbum:
                        {
                        SetFileDetailL( aFileDetails->iAlbum, field );
                        break;
                        }
                    case EMetaDataYear:
                        {
                        SetFileDetailL( aFileDetails->iYear, field );
                        break;
                        }
                    case EMetaDataCopyright:
                        {
                        SetFileDetailL( aFileDetails->iCopyright, field );
                        break;
                        }
                    case EMetaDataOriginalArtist:
                        {
                        SetFileDetailL( aFileDetails->iOriginalArtist, field );
                        break;
                        }
                    case EMetaDataAlbumTrack:
                        {
                        SetFileDetailL( aFileDetails->iAlbumTrack, field );
                        break;
                        }
                    case EMetaDataGenre:
                        {
                        SetFileDetailL( aFileDetails->iGenre, field );
                        break;
                        }
                    case EMetaDataComposer:
                        {
                        SetFileDetailL( aFileDetails->iComposer, field );
                        break;
                        }
                    case EMetaDataUserUrl:
                    case EMetaDataUrl:     // (L616) fallthrough
                        {
                        SetFileDetailL( aFileDetails->iAudioFileWebPage, field );
                        break;
                        }
                    case EMetaDataComment:
                        {
                        SetFileDetailL( aFileDetails->iComment, field );
                        break;
                        }
                    default:
                        {
                        // nothing to do
                        break;
                        }
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(); // metaDataUtility
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::FetchDCFHeaderDataL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::FetchDCFHeaderDataL( const TDesC& aFileName,
                                                CMPFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::FetchDCFHeaderDataL()"));

    TBuf <256> headerValue;
    TInt err( KErrNone );
    CContent* content = CContent::NewLC( aFileName );

    // Content-Name
    err = content->GetStringAttribute(ETitle, headerValue);
    if( err == KErrNone )
        {
        SetFileDetailL( aFileDetails->iTitle, headerValue );
        }

    // Content-Vendor
    err = content->GetStringAttribute(EContentVendor, headerValue);
    if( err == KErrNone )
        {
        SetFileDetailL( aFileDetails->iProvider, headerValue );
        }

    // Content-Description
    err = content->GetStringAttribute(EDescription, headerValue);
    if( err == KErrNone )
        {
        SetFileDetailL( aFileDetails->iDescription, headerValue );
        }

    // clean up
    CleanupStack::PopAndDestroy(); // content
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::Is3gpFile
// -----------------------------------------------------------------------------
//
TBool CMPFileDetailsDialog::Is3gpFile( const TDesC& aFileName,
                                       CMPFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::Is3gpFile()"));

    TBool retVal( EFalse );
    // File extension
    if( aFileName.FindF( KExt3gpp ) != KErrNotFound ||
        aFileName.FindF( KExtMp4 )  != KErrNotFound ||
        aFileName.FindF( KExtM4a )  != KErrNotFound )
        {
        retVal = ETrue;
        }

    // Mimetype
    if( !retVal && aFileDetails->iFormat )
        {
        TPtrC mime = aFileDetails->iFormat->Des();
        if (mime == KDetaMime3gpp  ||
            mime == KDetaMime3gpp2 ||
            mime == KDetaMimeMp4)
            {
            retVal = ETrue;
            }
        }

    return retVal;
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::SetFileDetailL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::SetFileDetailL( HBufC*& aBuf, const TDesC& aDetail )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::SetFileDetailL()"));

    if( !aBuf && aDetail.Length() )
        {
        aBuf = aDetail.AllocL();
        }
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::SetFileDetailL
// -----------------------------------------------------------------------------
//
void CMPFileDetailsDialog::SetFileDetailL( HBufC*& aBuf, const TDesC8& aDetail )
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::SetFileDetailL()"));

    if( !aBuf && aDetail.Length())
        {
        aBuf = HBufC::NewL( aDetail.Length() );
        aBuf->Des().Copy( aDetail );
        }
}

// CMPFileDetailsDialog::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void CMPFileDetailsDialog::Close()
{
    MPX_ENTER_EXIT(_L("CMPFileDetailsDialog::Close()"));

    if ( iPopupList )
    {
       iPopupList->CancelPopup();
    }
}

// ============================ CMPDummyController ============================

// -----------------------------------------------------------------------------
// CMPDummyController::CMPDummyController
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPDummyController::CMPDummyController()
{
    MPX_ENTER_EXIT(_L("CMPDummyController::CMPDummyController()"));
}

// -----------------------------------------------------------------------------
// CMPFileDetailsDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPDummyController::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPDummyController::ConstructL()"));

    CreateWindowL();
    SetExtent(TPoint(), TSize());
    ActivateL();
    SetPointerCapture(ETrue);
    ClaimPointerGrab(ETrue);
}

// -----------------------------------------------------------------------------
// CMPDummyController::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPDummyController* CMPDummyController::NewLC()
{
    MPX_ENTER_EXIT(_L("CMPDummyController::NewLC()"));

    CMPDummyController* self= new (ELeave) CMPDummyController();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CMPDummyController::~CMPDummyController
// Destructor
// -----------------------------------------------------------------------------
//
CMPDummyController::~CMPDummyController()
{
    MPX_ENTER_EXIT(_L("CMPDummyController::~CMPDummyController()"));

    if (iCoeEnv && iEikonEnv->EikAppUi())
        iEikonEnv->EikAppUi()->RemoveFromStack(this);
}

// -----------------------------------------------------------------------------
// CMPDummyController::OfferKeyEventL
// Absorbs all the key presses
// -----------------------------------------------------------------------------
//
TKeyResponse CMPDummyController::OfferKeyEventL(
        const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
{
    MPX_ENTER_EXIT(_L("CMPDummyController::OfferKeyEventL()"));
    return EKeyWasConsumed;
}

//  End of File
