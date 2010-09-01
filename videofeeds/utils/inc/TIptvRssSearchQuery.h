/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Data class for RSS search query.*
*/




#ifndef TIPTVRSSSEARCHQUERY_H
#define TIPTVRSSSEARCHQUERY_H

#include <e32std.h>
#include "TIptvRssSearchQuery.h"
#include "CIptvUtil.h"

/**
 * Data class for RSS search query.
 *
 * @lib IptvUtil.lib
 * @since Video Center 3.0
 */
class TIptvRssSearchQuery
    {
    public:

    /**
     * Constructor
     */
    IMPORT_C TIptvRssSearchQuery();
	
    /**
     * Destructor
     */
    IMPORT_C ~TIptvRssSearchQuery();
	
    /**
     * Getter for search string
     *	 
     * @return	Reference to the search string
     */
    IMPORT_C TDesC& SearchString();
	
    /**
     * Setter for search string
     *
     * @param aSearchString Search string to be assigned for this class.
     */
	IMPORT_C void SetSearchString( TDesC& aSearchString );

	private: // Instance variables

	/**
     * Instance variable holding the search string for the query
     */
    TBuf16<KIptvRssSearchMaxSearchStringLength> iSearchString;
    };

#endif // TIPTVRSSSEARCHQUERY_H
