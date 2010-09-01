/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for selecting best video type supported by terminal.*
*/




#ifndef CIPTVTERMINALVIDEOSUPPORT_H
#define CIPTVTERMINALVIDEOSUPPORT_H

#include <e32std.h>
#include "CIptvUtil.h"

class TIptvMimeType;
class CIptvMediaContent;

/**
 * Class for selecting best video type supported by terminal.
 *
 * @lib IptvUtil.lib
 * @since Video Center 2.0
 */
class CIptvTerminalVideoSupport : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CIptvTerminalVideoSupport* NewL();

        /**
         * Select best mime-type from array.
         *
         * @param aMediaContents Array of media contents.
         * @return Index of best content.
         */
        IMPORT_C TInt SelectMimeTypeL(RPointerArray<CIptvMediaContent>& aMediaContents);

        /**
         * Destructor.
         */
        virtual ~CIptvTerminalVideoSupport();
                                        
    private: // New functions

        /**
         * C++ default constructor.
         */
        CIptvTerminalVideoSupport();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Load Mime-types from resource file.
         *
         * @param aBuffer The mime-type list from resource file as buffer.
         */
        void LoadMimeTypesL( const HBufC8* aBuffer );

        /**
         * Loads the class resources from resource file.
         */
        void LoadResourceL();

    private: // Data members

        /**
         * Array of Mime-Types.
         */
        RArray<TIptvMimeType> iMimeTypes;
    };

#endif // CIPTVTERMINALVIDEOSUPPORT_H
