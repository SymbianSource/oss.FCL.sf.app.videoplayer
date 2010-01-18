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
* Description:   Declares media file details dialog*
*/


// Version : %version: 6 %




#ifndef MPFILEDETAILSDIALOG_H
#define MPFILEDETAILSDIALOG_H

//  INCLUDES
#include <aknPopup.h>
#ifdef SYMBIAN_BUILD_GCE
#include <videoplayer2.h>
#else
#include <videoplayer.h>
#endif //SYMBIAN_BUILD_GCE
#include <mdaaudiosampleplayer.h>
#include <MPFileDetails.h>

// FORWARD DECLARATIONS
class CAknDoublePopupMenuStyleListBox;
class CMPPopupList;


// CLASS DECLARATIONS

/**
*  CMPFileDetailsDialog
*
*  @lib MPFileDetailsDialog.lib
*  @since 2.0
*/
class CMPFileDetailsDialog : public CBase, public MMdaAudioPlayerCallback,
                             public MVideoPlayerUtilityObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMPFileDetailsDialog* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMPFileDetailsDialog();

    public: // New functions

        /**
        * Shows file details to user in dialog.
        * @since 2.0
        * @param aFileName Media file which details are shown to user.
        * @leaves Leaves with KErrNotSupported if file is not valid media file.
        * @return void
        */
        IMPORT_C void ExecuteLD(const TDesC& aFileName);

        /**
        * Shows file details to user in dialog.
        * @since 2.0
        * @param aFileDetails File details which are shown to user.
        * @return void
        */
        IMPORT_C void ExecuteLD(CMPFileDetails* aFileDetails);
        
        /** 
        * Attempts to force the dialog to close 
        * @since 5.0 
        * @return void 
        */ 
        IMPORT_C void Close(); 

    public: // Functions from MMdaAudioPlayerCallback

        /** @see MMdaAudioPlayerCallback::MapcInitComplete() */
        void MapcInitComplete(TInt aError,
                const TTimeIntervalMicroSeconds& aDuration);

        /** @see MMdaAudioPlayerCallback::MapcPlayComplete */
        void MapcPlayComplete(TInt aError);

    public: // Functions from MVideoPlayerUtilityObserver

        /** @see MVideoPlayerUtilityObserver::MvpuoOpenComplete */
        void MvpuoOpenComplete(TInt aError);

        /** @see MVideoPlayerUtilityObserver::MvpuoPrepareComplete */
        void MvpuoPrepareComplete(TInt aError);

        /** @see MVideoPlayerUtilityObserver::MvpuoFrameReady */
        void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);

        /** @see MVideoPlayerUtilityObserver::MvpuoPlayComplete */
        void MvpuoPlayComplete(TInt aError);

        /** @see MVideoPlayerUtilityObserver::MvpuoEvent */
        void MvpuoEvent(const TMMFEvent& aEvent);

    private:

        /**
        * C++ default constructor.
        */
        CMPFileDetailsDialog();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New functions

        /**
        * Loads MPFileDetails.rsc resource file.
        * @since 2.0
        * @return void
        */
        void LoadResourcesL();

        /**
        * Unloads MPFileDetails.rsc resource file.
        * @since 2.0
        * @return void
        */
        void UnLoadResources();

        /**
        * Sets title for CAknPopupList.
        * @since 2.0
        * @param aPopupList CAknPopupList which title is modified
        * @return void
        */
        void SetTitleL( CAknPopupList* aPopupList );

        /**
        * Fills listbox with file details information.
        * @since 2.0
        * @param aFileDetails File details
        * @return void
        */
        void FillListBoxL( CMPFileDetails* aFileDetails );

        /**
        * Adds header and value to list.
        * @since 2.0
        * @param aHeading Heading
        * @param aValue Value
        * @param aItemArray Array where value and header are added.
        * @return void
        */
        void AddItemToListBoxL( const TDesC& aHeading, const TDesC& aValue,
                CDesCArray* aItemArray );

        /**
        * Constructs clip title item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeTitleItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs clip url item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeUrlItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs media format item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeFormatItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs video resolution item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeResolutionItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs media duration item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeDurationItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs bitrate item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeBitrateItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs samplerate item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeSamplerateItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs file size item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeSizeItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs date item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeDateItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs time item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeTimeItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs upload status item.
        * @since 2.8
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeUploadStatusL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs copyright item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeCopyrightItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs original artist of the track item.
        * @since 3.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeOriginalArtistItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs track position in the album to which
        * the track belongs item.
        * @since 3.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeAlbumTrackItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs Genre information item.
        * @since 3.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeGenreItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs Composer information item.
        * @since 3.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeComposerItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs URL from which more information about
        * the track can be found item.
        * @since 3.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeAudioFileWebPageItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs comment someone has added about the track item.
        * @since 3.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeCommentItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs Artist item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeArtistItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs Album item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeAlbumItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Constructs Year item.
        * @since 2.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeYearItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Converts between arabic-indic digits and european digits.
        * @since 2.0
        * @param aText numbers to be converted.
        * @return void
        */
        void LanguageSpecificNumberConversion( TDes& aText) const;

        /**
        * Fetches file details from the meta data.
        * @since 2.1
        * @param aFileName Name of the audio file.
        * @param aFileDetails Pointer to the details to be fetched.
        * @return void
        */
        void FetchMetaDataL( const TDesC& aFileName,
                CMPFileDetails* aFileDetails );

        /**
        * Fetches drm file details.
        * @since 2.6
        * @param aFileName Name of the audio file.
        * @param aFileDetails Pointer to the details to be fetched.
        * @return void
        */
        void FetchDrmDataL( const TDesC& aFileName,
                CMPFileDetails* aFileDetails );

        /**
        * Display DRM heading info
        * @since 9.1
        * @param aItemArray Array where constructed item is put.
        * @return void
        */
        void DisplayDrmHeadingInfoL( CDesCArray* aItemArray );

        /**
        * Makes provider item.
        * @since 3.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeProviderItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Makes description item.
        * @since 3.0
        * @param aItemArray Array where constructed item is put.
        * @param aFileDetails File details.
        * @return void
        */
        void MakeDescriptionItemL( CDesCArray* aItemArray,
                CMPFileDetails* aFileDetails );

        /**
        * Fetches additional meta data.
        * @since 3.0
        * @param aFileName Name of the audio file.
        * @param aFileDetails Pointer to the details to be fetched.
        * @return None
        */
        void FetchAdditionalMetaDataL( const TDesC& aFileName,
                CMPFileDetails* aFileDetails );

        /**
        * Fetches DCF meta data.
        * @since 3.0
        * @param aFileName Name of the audio file.
        * @param aFileDetails Pointer to the details to be fetched.
        * @return None
        */
        void FetchDCFHeaderDataL( const TDesC& aFileName,
                CMPFileDetails* aFileDetails );

        /**
        * Checks whether file is a 3gp/mp4 file.
        * @param    aFileName: Name of the file
        * @param    aFileDetails Pointer to the details to be fetched.
        * @return   ETrue if file is 3gp file
        */
        TBool Is3gpFile( const TDesC& aFileName,
                         CMPFileDetails* aFileDetails );

        /**
        * Sets file detail in buffer.
        * @param    aBuf: Buffer where to store
        * @param    aDetail: Details which to store
        */
        void SetFileDetailL( HBufC*& aBuf, const TDesC& aDetail );
        void SetFileDetailL( HBufC*& aBuf, const TDesC8& aDetail );

    private:    // Data
        CAknDoublePopupMenuStyleListBox* iListBox;
        CDesCArrayFlat* iGenres;
        CMPPopupList* iPopupList;

        TInt iResourceFileOffset;
        CActiveSchedulerWait iWait;
        TInt iResult;        
    };


/**
*  CMPDummyController
*  Dummy controller for CVideoPlayerUtility.
*  Absorbs all the key presses while opening video clip.
*
*  @lib MPFileDetailsDialog.lib
*  @since 2.0
*/
class CMPDummyController : public CCoeControl
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPDummyController* NewLC();

        /**
        * Destructor.
        */
        virtual ~CMPDummyController();

    public: // New functions

        /**
        * Controls window handle.
        * @since 2.0
        * @return Controllers RWindow
        */
        inline RWindow& GetClientWindow();

        /**
        * Controls WsSession.
        * @since 2.0
        * @return Controllers RWsSession
        */
        inline RWsSession& GetClientWsSession();

        /**
        * Controls ScreenDevice
        * @since 2.0
        * @return Controllers CWsScreenDevice
        */
        inline CWsScreenDevice& GetScreenDevice();

    private: // Functions from CCoeControl

        /** @see CCoeControl::OfferKeyEventL() */
        virtual TKeyResponse OfferKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                TEventCode /*aType*/ );

    private:

        /**
        * C++ default constructor.
        */
        CMPDummyController();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    };

#endif      // MPFILEDETAILSDIALOG_H

// End of File
