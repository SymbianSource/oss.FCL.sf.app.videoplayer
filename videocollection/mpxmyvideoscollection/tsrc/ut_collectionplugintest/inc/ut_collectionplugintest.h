/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  video Playback User Input Handler test class for STIF testing
*
*/

// Version : %version: 1 %

#ifndef COLLECTIONPLUGIN_TEST_H
#define COLLECTIONPLUGIN_TEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <e32base.h>
#include <e32def.h>
#include <StifItemParser.h>
#include <W32STD.H>

//#include "mpxlog.h"
//#include "mpxvideo_debug.h"
#include <mpxcollectionpluginobserver.h>

#include <mpxcollectionplugin.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>
#include "vcxmyvideosactivetask.h"
#include "vcxmyvideosmdsdb_stub.h"
#include "vcxmyvideosvideocache_stub.h"

#define private public
#include "vcxmyvideoscollectionplugin.h"
#undef private

// CLASS DECLARATION

class CMPXMedia;
class CVcxMyVideosMdsDb;
class CMPXResource;
class CMPXMediaArray;
//class CVcxMyVideosDownloadUtil;
class CVcxMyVideosVideoCache;
class CVcxMyVideosCategories;
class CVcxMyVideosMessageList;
class CVcxMyVideosAsyncFileOperations;
class CVcxMyVideosOpenHandler;
class CCollectionPluginTest;
class CVcxMyVideosCollectionPlugin;
class MMPXCollectionPluginObserver;

// DATA

// CONSTANTS


// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KCollectionplugintest_LogPath, "\\logs\\testframework\\collectionplugintest\\" );

// Log file
_LIT( KCollectionplugintest_LogFile, "collectionplugintest.txt" );
_LIT( KCollectionplugintest_LogFileWithTitle, "collectionplugintest[%S].txt" );


/**
*  CCollectionPluginTest class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CCollectionPluginTest) : public CScriptBase, MMPXCollectionPluginObserver
{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCollectionPluginTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CCollectionPluginTest();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
        CCollectionPluginTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void CreateL();
        
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * LogMethod
        */
        void LogMethod( TPtrC aMethod );

        /**
        * DeleteStubL
        */
        TInt DeleteStubL( CStifItemParser& aItem );
        
        /**
        * CheckCurrentCmd
        * 
        * Checks current command in vcxmyvideosactivetask
        */        
        TInt CheckCurrentCmdL( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        
        virtual TInt SetVideoListFetchingL( CStifItemParser& aItem );
        virtual TInt CancelCacheListFetchingL( CStifItemParser& aItem );
        
        CMPXCommand* CreateMpxCommandLC( TInt aCommandGeneralId, TInt aMyVideosCommandId, TBool aSync );
        virtual TInt CreateNewCmdL( CStifItemParser& aItem );
        virtual TInt SetNewCmdL( CStifItemParser& /*aItem*/ );
        virtual TInt CreateNewMediaL( CStifItemParser& aItem );
        virtual TInt AddMediaToCacheL( CStifItemParser& /*aItem*/ );
        virtual TInt SetMediaAttributeL( CStifItemParser& aItem );
        virtual TInt SelectAttributesL( CStifItemParser& aItem );
        
        /**
         * Set iPath
         */
        virtual TInt InsertToPathL( CStifItemParser& aItem );
        virtual TInt SelectPathL( CStifItemParser& aItem );
        virtual TInt SelectPathIdL( CStifItemParser& aItem );
        virtual TInt RemoveMediaFromCacheL( CStifItemParser& aItem );
        virtual TInt CheckMediaCountL( CStifItemParser& aItem );

        // TEST FUNCS
        virtual TInt OpenL( CStifItemParser& aItem );
        virtual TInt MediaL( CStifItemParser& aItem );
        virtual TInt CancelRequest( CStifItemParser& aItem );
        virtual TInt CommandL( CStifItemParser& aItem );
        virtual TInt FindAllL( CStifItemParser& aItem );
        virtual TInt FindAllSyncL( CStifItemParser& aItem );
        virtual TInt GetCapabilities( CStifItemParser& aItem );
        virtual TInt AddL( CStifItemParser& aItem );
        virtual TInt RemoveL( CStifItemParser& aItem );
        virtual TInt SetL( CStifItemParser& aItem );
        virtual TInt HandleStepL( CStifItemParser& aItem );
        virtual TInt HandleMyVideosDbEvent( CStifItemParser& aItem );
        virtual TInt HandleCreateVideoListResp( CStifItemParser& aItem );
        virtual TInt HandleObjectPresentNotificationL( CStifItemParser& aItem );
        virtual TInt AddVideoToMdsAndCacheL( CStifItemParser& aItem );
        virtual TInt SetVideoL( CStifItemParser& aItem );
        virtual TInt SendMyVideosMessageL( CStifItemParser& aItem );
        virtual TInt SendMessages( CStifItemParser& aItem );
        virtual TInt CategoriesL( CStifItemParser& aItem );
        virtual TInt NotifyNewVideosCountDecreasedL( CStifItemParser& aItem );        
        virtual TInt AlbumsL( CStifItemParser& aItem );

        
    public: // from observer
        virtual void HandleMessage(const CMPXMessage& aMsg)
            {
            HandleMessage(const_cast<CMPXMessage*>(&aMsg), KErrNone);
            }
        virtual void HandleMessage(CMPXMessage* /*aMsg*/, TInt /*aError*/);
        virtual void HandleOpen(CMPXMedia* aMedia, TInt aErr);
        virtual void HandleOpen(CMPXCollectionPath* aPath, TInt aErr);
        virtual void HandleOpen(CMPXMedia* aMedia, const CMPXCollectionPath* aPath, TInt aErr);
        
        virtual void HandleMedia(CMPXMedia* aMedia, TInt aError);
        virtual void HandleCommandComplete(CMPXCommand* aCommandResult, TInt aError);
        virtual void HandleRemove(const CDesCArray& aUriArray, TInt aError);
        virtual void HandleFindAll(CMPXMedia* aMedia, TInt aError);        
        
    private:    

        TUid iCollectionUid;
        
        // Tested class
        CVcxMyVideosCollectionPlugin*   iCollection;
        
        CMPXCollectionPath* iPath;
        RArray<TMPXAttribute> iAttrs;
        
        CMPXMediaArray* iMessageList;
        
        CMPXMedia* iMedia;
        CMPXCommand* iCommand;
};

#endif      // COLLECTIONPLUGIN_TEST_H

// End of File
