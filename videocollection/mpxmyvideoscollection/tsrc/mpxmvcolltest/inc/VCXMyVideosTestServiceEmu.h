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
* Description:   Class to emulate IPTV services and ECG content.*
*/



#ifndef VCXMYVIDEOSTESTSERVICEEMU_H
#define VCXMYVIDEOSTESTSERVICEEMU_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

_LIT( KVcxTestServiceName1, "OOPS" ); // http://193.65.182.78/oops.xml
_LIT( KVcxTestServiceName2, "NokiaVideos" ); // http://193.65.182.78/nokia_vod.xml
_LIT( KVcxTestServiceName3, "Enginetestingfeed" ); // http://193.65.182.78/testingfeed_iptvengine.xml
_LIT( KVcxTestServiceName4, "SmallVideos" ); // http://193.65.182.78/misc/smallvideos/smallvideos.xml
_LIT( KVcxTestServiceName5, "Miscvideos" ); // http://193.65.182.78/miscvideos.xml

// MACROS
//#define ?macro ?macro_def

// FORWARD DECLARATIONS

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

class CVCXMyVideosTestContent : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CVCXMyVideosTestContent* NewL( TInt aServiceId, TInt aContentId, const TPtrC& aUrl, const TPtrC& aTitle );

        /**
        * Destructor.
        */
        virtual ~CVCXMyVideosTestContent();

    private: // New functions


    private:

        /**
        * C++ default constructor.
        */
        CVCXMyVideosTestContent();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TInt aServiceId, TInt aContentId, const TPtrC& aUrl, const TPtrC& aTitle );

    public:    // Data

        HBufC* iUrl;
        HBufC* iTitle;
        TInt iServiceId;
        TInt iContentId;
    };

/**
*  CVCXMyVideosTestServiceEmu test class
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class CVCXMyVideosTestServiceEmu : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVCXMyVideosTestServiceEmu* NewL();

        /**
        * Destructor.
        */
        virtual ~CVCXMyVideosTestServiceEmu();

    public: // New functions

        TInt GetServiceIdL( const TDesC& aServiceName, TInt& aServiceId );

        const TPtrC GetContentUrlL( TInt aServiceId, TInt aContentId );

        const TPtrC GetContentTitleL( TInt aServiceId, TInt aContentId );

    public: // Functions from base classes

    private: // New functions

        void AddServiceContentL( const TDesC& aServiceName, TInt aContentId, const TDesC& aUrl, const TDesC& aTitle );

    private:

        /**
        * C++ default constructor.
        */
        CVCXMyVideosTestServiceEmu( );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

    private:    // Data

        RPointerArray<CVCXMyVideosTestContent> iContents;
    };

#endif      // VCXMYVIDEOSTESTSERVICEEMU_H

// End of File
