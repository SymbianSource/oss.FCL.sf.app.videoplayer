/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef CIPTVRESOURCELOADER_H
#define CIPTVRESOURCELOADER_H

// INCLUDES
#include <e32std.h>

class CCoeEnv;
class RFs;

// CLASS DECLARATION
class CIptvResourceLoader : public CBase
    {
    public:
        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvResourceLoader* NewL( CCoeEnv& aCoeEnv );

        /**
         * Destructor.
         */
        virtual ~CIptvResourceLoader();

        /**
         * Add resource.
         *
         * @param aResourceFile Descriptor containing the resource file to be loaded
         */
        IMPORT_C void AddResourceL( const TDesC& aResourceFile );
                                
    private: // New functions           

        /**
        * C++ default constructor.
        */
        CIptvResourceLoader( CCoeEnv& aCoeEnv );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();

        /**
        * Search resource file.
        *
        * @param aFile Buffer descriptor to the resource file
        * @return System wide error code
        */
        TInt SearchResourceFile( const TDesC& aFile );
                  
        /**
        * Add resource file.
        *
        * @param aFile Buffer descriptor to the resource file
        * @return System wide error code
        */
        TInt AddResourceFile( const TDesC& aFile );
                  
    private:     // Data members

        /**
        * Resource file offset.
        */      
        TInt        iOffset;

        /**
        * Is resource added
        */ 
        TBool       iAdded;

        /**
        * Reference to CCoeEnv.
        */ 
        CCoeEnv&    iCoeEnv;

    };

#endif  // CIPTVRESOURCELOADER_H

// End of File
