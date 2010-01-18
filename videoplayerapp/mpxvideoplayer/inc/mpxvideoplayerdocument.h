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
* Description:  Document class
*
*/

// Version : %version: da1mmcf#5 %

#ifndef CMPXVideoPlayerDOCUMENT_H
#define CMPXVideoPlayerDOCUMENT_H

#include <GenericParamConsumer.h>

class CEikAppUi;
class MMPXViewUtility;

/**
*  Document class of MpxVideoPlayer application.
*
*  @lib mpxvideoplayer.exe
*  @since MpxVideoPlayer 0.1
*/
class CMpxVideoPlayerDocument : public CAiwGenericParamConsumer
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aApp Handle to Eikon's application class
        * @return A new instance of CMacFwDocument
        */
        static CMpxVideoPlayerDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CMpxVideoPlayerDocument();

    private:    // Functions from base classes   
        /**
        * From CEikDocument
        * @return A new instance of application's AppUi class
        */
        CEikAppUi* CreateAppUiL();

        /**
        * Default constructor.
        * @param aApp Handle to Eikon's application class
        */
        CMpxVideoPlayerDocument( CEikApplication& aApp );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
         * From CEikDocument.
         * Called by the framework to open a file.
         *
         * @param aDoOpen ETrue to open an existing file, 
                          EFalse to create a new default file.
         * @param aFileName THe path and name of the file to open or create.
         * @param aFs FIle server session to use.
         * @return Pointer to newly created object.
         */
        CFileStore* OpenFileL( 
            TBool aDoOpen,
            const TDesC& aFilename,
            RFs& aFs );

        /**
         * From CEikDocument.
         * Called by the framework to open a file.
         *
         * @param aFileStore This is set by the function to the file 
                             store that stores the main document, 
                             if the file is a "native" Symbian OS file, 
                             otherwise it is set to NULL.
         * @param aFile The path and name of the file to read from
         */
        void OpenFileL(
            CFileStore*& aFileStore, 
            RFile& aFile );

    private:    // data
    
        MMPXViewUtility* iViewUtility;
    };

#endif         // CMPXVideoPlayerDOCUMENT_H

