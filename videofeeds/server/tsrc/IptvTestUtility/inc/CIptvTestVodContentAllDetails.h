/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Holds all ECG data for one VOD content item.*
*/



#ifndef CIPTVTESTVODCONTENTALLDETAILS_H
#define CIPTVTESTVODCONTENTALLDETAILS_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvVodContentContentFullDetails;
class CIptvVodContentContentBriefDetails;
class CIptvMediaContent;

// CLASS DECLARATION

/**
* class CIptvTestVodContentAllDetails
*  @since
*/
class CIptvTestVodContentAllDetails : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CIptvTestVodContentAllDetails* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CIptvTestVodContentAllDetails();

    private:

        /**
        * C++ default constructor.
        */
        CIptvTestVodContentAllDetails();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:    // Data
        CIptvVodContentContentFullDetails*      iFullDetails;
        CIptvVodContentContentBriefDetails*     iBriefDetails;
        RPointerArray<CIptvMediaContent>        iMediaContents;

        TInt                                    iCategoryId;
    };


#endif      // CIPTVTESTVODCONTENTALLDETAILS_H

// End of File
