/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Test Harness for VideoPlaybackControls
*
*/

// Version : %version: ou1cpsw#8 %




#ifndef VIDEOPLAYBACKCONTROLSTEST_H
#define VIDEOPLAYBACKCONTROLSTEST_H

//  INCLUDES
#include <W32STD.H>
#include <StifLogger.h>
#include <StifTestModule.h>
#include <TestScripterInternal.h>

#include "mpxvpbc_stub.h"

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
#ifdef __WINSCW__
_LIT( KVideoPlaybackControlsTestLogPath, 
      "\\logs\\testframework\\VideoPlaybackViewControlsTest\\" );
#else 
_LIT( KVideoPlaybackControlsTestLogPath, 
      "e:\\logs\\testframework\\VideoPlaybackViewControlsTest\\" );
#endif

// Log file
_LIT( KVideoPlaybackControlsTestLogFile, 
                    "VideoPlaybackControlsTest.txt" ); 
_LIT( KVideoPlaybackControlsTestLogFileWithTitle, 
                    "VideoPlaybackControlsTest_[%S].txt" );

// FORWARD DECLARATIONS
class CMPXVideoPlaybackControlsController;

// CLASS DECLARATION

/**
*  CVideoPlaybackControlsTest test class for STIF Test Framework TestScripter.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CVideoPlaybackControlsTest) : public CScriptBase
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CVideoPlaybackControlsTest* NewL( CTestModuleIf& aTestModuleIf );

    /**
     * Destructor.
     */
    virtual ~CVideoPlaybackControlsTest();

public: // New functions

    /**
     * ?member_description.
     * @param ?arg1 ?description
     * @return ?description
     */
    //?type ?member_function( ?type ?arg1 );

public: // Functions from base classes

    /**
     * From CScriptBase Runs a script line.
     * @param aItem Script line containing method name and parameters
     * @return Symbian OS error code
     */
    virtual TInt RunMethodL( CStifItemParser& aItem );

protected:  // New functions

    /**
     * ?member_description.
     * @param ?arg1 ?description
     * @return ?description
     */
    //?type ?member_function( ?type ?arg1 );

protected:  // Functions from base classes

    /**
     * From ?base_class ?member_description
     */
    //?type ?member_function();

private:

    /**
     * C++ default constructor.
     */
    CVideoPlaybackControlsTest( CTestModuleIf& aTestModuleIf );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    // Prohibit copy constructor if not deriving from CBase.
    // ?classname( const ?classname& );
    // Prohibit assigment operator if not deriving from CBase.
    // ?classname& operator=( const ?classname& );

    /**
    * Frees all resources allocated from test methods.
    */
    void Delete();

    /**
    * Method used to log version of test class
    */
    void SendTestClassVersion();

    /**
    * Create controls
    * @return Symbian OS error code.
    */
    virtual TInt CreateControls( CStifItemParser&  aItem );

    /**
    * Delete controls
    * @return Symbian OS error code.
    */
    virtual TInt DeleteControls();
    /**
    * Test HandleEventL(EMPXControlCmdSetAspectRatio)
    * @return Symbian OS error code.
    */
    virtual TInt SetAspectRatio( CStifItemParser&  aItem );

    /**
    * Test HandleEventL(EMPXControlCmdSetVolume)
    * @return Symbian OS error code.
    */
    virtual TInt SetVolume( CStifItemParser&  aItem );

    /**
    * Test HandleEventL(EMPXControlCmdSetDuration)
    * @return Symbian OS error code.
    */
    virtual TInt SetDuration( CStifItemParser&  aItem );

    /**
    * Issue pointer event
    * @return Symbian OS error code.
    */
    virtual TInt PointerEvent( CStifItemParser&  aItem );
    
    /**
    * Issue pointer event
    * @return Symbian OS error code.
    */
    virtual TInt KeyEvent( CStifItemParser&  aItem );    

    /**
    * Check return value is expected value or not
    * @return Symbian OS error code.
    */
    virtual TInt ExpectedResult( CStifItemParser&  aItem );

    /**
    * Set state
    * @return Symbian OS error code.
    */
    virtual TInt SetState( CStifItemParser&  aItem );

    /**
    * Add default file details
    * @return Symbian OS error code.
    */
    virtual TInt AddDefaultFileDetails();

    /**
    * Add audio only file details
    * @return Symbian OS error code.
    */
    virtual TInt AddAudioOnlyFileDetails();

    /**
    * Create controller
    */
    void CreateController( const TDesC& aFileName, TInt aMode );

    void TapOnScreen( TPointerEvent::TType aType, TPoint aPosition );

    TInt ParseType( TDesC& aTypeName, TPointerEvent::TType& aType );

    TFileName GetFileNameAndPath( TDesC& aFileName );

    virtual TInt SetDownLoadSize(CStifItemParser&  aItem );
    
    virtual TInt SetDownLoadPostion(CStifItemParser&  aItem );

    virtual TInt UpdateDownloadPosition( CStifItemParser&  aItem );

    virtual TInt SetDownLoadPaused();
    
    virtual TInt ClearDownLoadPaused();
    
    virtual TInt TvOutConnected();
    
    virtual TInt TvOutDisconnected();
    
    virtual TInt HandleErrors();

    virtual TInt SoftkeyPressed( CStifItemParser&  aItem );

    virtual TInt ShowControls();

    virtual TInt ToggleVisibility();

    virtual TInt HandleBackgroundEvnet();
    
    virtual TInt ShowMediaDetailsViewer();
    
    virtual TInt CloseMediaDetailsViewer();

    //[TestMethods] - Do not remove

public:     
    // Data
    // ?one_line_short_description_of_data
    //?data_declaration;

protected:  
    // Data
    // ?one_line_short_description_of_data
    //?data_declaration;

private:    
    // Data    
    // ?one_line_short_description_of_data
    //?data_declaration;

    // Reserved pointer for future extension
    //TAny* iReserved;

public:     
    // Friend classes
    //?friend_class_declaration;
    CMPXVideoPlaybackControlsController* iController;    

protected:  
    // Friend classes
    //?friend_class_declaration;

private:    
    // Friend classes
    //?friend_class_declaration;
    CMPXVideoPlaybackContainer*          iContainer;
    RWsSession                           iWsSession;
};

#endif      // VIDEOPLAYBACKCONTROLSTEST_H

// End of File
