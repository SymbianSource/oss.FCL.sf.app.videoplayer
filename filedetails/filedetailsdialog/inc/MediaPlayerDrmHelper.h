/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   DRM helper for Media files.*
*/



#ifndef MEDIAPLAYERDRMHELPER_H
#define MEDIAPLAYERDRMHELPER_H

// INCLUDES
#include <e32std.h>
#include <MPFileDetails.h>  

// FORWARD DECLARATIONS
class CDRMHelper;
class CDRMHelperRightsConstraints;
class MDesCArray;
class CCoeEnv;
class CMPFileDetails;
namespace DRM
{
class CDrmUtility;	
class CDrmUiHandling;
}

// CONSTANTS
const TInt KMediaPlayerRightsAboutToExpire = -40000;

// CLASS DECLARATION

/**
*  DRM helper for Media files.
*
*  @lib MPFileDetailsDialog.lib
*  @since 3.2
*/
NONSHARABLE_CLASS( CMediaPlayerDrmHelper ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor called by CMPVideoPlayerUIController class
        * @param aDrmHelper Pointer to CDRMHelper class. Ownership is not transferred.
        */
        static CMediaPlayerDrmHelper* NewL(CDRMHelper* aDrmHelper, CMPVideoPlayerUIController* aUIController);

        /**
        * Two-phased constructor called by CMPFileDetailsDialog class or MediaPlayerUtility class
        * @param aDrmHelper Pointer to CDRMHelper class. Ownership is not transferred.
        */
        static CMediaPlayerDrmHelper* NewL(CDRMHelper* aDrmHelper );

        /**
        * Destructor.
        */
        virtual ~CMediaPlayerDrmHelper();

    public: // New functions

        /**
        * Loads usage rights information for a particular media file using
        * CDRMHelper. The loaded constraint is DRMCommon::EPlay.
        * @since 3.2
        * @param aFileName File path, which usage rights info is loaded.
        */
        void LoadDrmUsageRightsInfoL(const TDesC& aFileName);

        /**
        * Loads usage rights information for a particular media file using
        * CDRMHelper. The loaded constraint is DRMCommon::EPlay.
        * @since 3.2
        * @param aFile File handle, which usage rights info is loaded.
        */
        void LoadDrmUsageRightsInfoL(
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                     RFile64& aFile);
#else
                                     RFile& aFile);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        /**
        * Gets usage rigths status.
        * @since 3.2
        * @param aStatus:
        *           EMediaPlayerDrmFull:        Full rights
        *           EMediaPlayerDrmMissing:     Rights missing
        *           EMediaPlayerDrmRestricted:  Restricted rights
        *           EMediaPlayerDrmExpired:     Expired rights (could be also future rights)
        *           EMediaPlayerDrmPreview:     Preview rights
        */
        void GetDrmUsageRightsStatus(TMediaPlayerRightsStatus& aStatus);

        /**
        * Gets start time of time based rights.
        * @since 3.2
        * @param aStartTime Start time.
        * @return Error code:
        *           KErrNone:       Start time returned.
        *           KErrNotFound:   Object doesn't have time based rights.
        *           KErrNotReady:   Usage rights info not loaded.
        */
        TInt GetDrmStartTime(TTime& aStartTime);

        /**
        * Get pointer to DRMHelper object (as given in constructor).
        * @since 3.2
        * @return Pointer to DRMHelper. Ownership not transferred.
        */
        CDRMHelper* DrmHelper(); 

        /**
        * Creates a file handle to the file passed in the aName parameter.
        * @param aHandle      Handle to a file
        * @param aName        Name of the file
        * @param aFileMode    Mode to open file handle in
        */
        static void CreateFileHandleLC(
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                       RFile64& aHandle, 
#else
                                       RFile& aHandle, 
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                                       const TDesC& aName, 
									   TUint aFileMode);

    private: // Private contructors

        /**
        * C++ default constructor.
        */
        CMediaPlayerDrmHelper(CDRMHelper* aDrmHelper, CMPVideoPlayerUIController* aUIController);

        /**
        * C++ default constructor.
        */
        CMediaPlayerDrmHelper(CDRMHelper* aDrmHelper );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        CDRMHelper* iDrmHelper;
        HBufC* iFileName;
        CDRMHelperRightsConstraints* iRightsConstraints;
        CCoeEnv* iEnv;
        DRM::CDrmUtility* iDrmUtility;  // not owned
        DRM::CDrmUiHandling* iDrmUiHandling;
        TBool iOMAProtected; // ETrue if protected content is OMA, EFalse if it is WMDRM

        TBool iProtected;
        TBool iExpired;
        TBool iDrmSendingAllowed;
        HBufC8* iInfoUrl;
        HBufC8* iPreviewUri;
    };

#endif      // MEDIAPLAYERDRMHELPER_H

// End of File
