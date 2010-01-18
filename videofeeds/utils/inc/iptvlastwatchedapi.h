/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Interface for getting and setting the Video Center's last*
*/




#ifndef C_IPTVLASTWATCHEDAPI_H
#define C_IPTVLASTWATCHEDAPI_H


#include <e32base.h>

class CIptvLastWatchedData;

/**
 *  Last watched API
 *
 *  VideoCenter Main UI has the last watched item, data of that item can be set 
 *  and get via this API. This needs to be used only when there is no Space UI 
 *  support available, but last watched item's data should be accessible.
 *
 *  @lib IptvUtil.lib
 *  @since S60 v3.1
 */
class CIptvLastWatchedApi : public CBase
    {

public:

    IMPORT_C static CIptvLastWatchedApi* NewL();
    IMPORT_C static CIptvLastWatchedApi* NewLC();

    /**
     * Destructor.
     */
    virtual ~CIptvLastWatchedApi();

    /**
     * Overwrites the last watched data to local datafile with given data.
     * If given data is invalid, previous data will be lost anyway.
     * Creates the datafile if necessary.
     *
     * @since S60 v3.1
     * @param aData data to be set
     * @return System-wide error code
     */
    IMPORT_C TInt SetLastWatchedDataL( CIptvLastWatchedData& aData );

    /**
     * Gets the last watched data from local datafile.
     *
     * @since S60 v3.1
     * @param aData data to be get
     * @return System-wide error code
     */
    IMPORT_C TInt GetLastWatchedDataL( CIptvLastWatchedData& aData );

    /**
     * Updates last play point to last watched local datafile.
     *
     * @since S60 v3.2
     * @param aLastVideoPlayPoint New last play point.
     * @return System-wide error code
     */
    IMPORT_C TInt UpdateLastVideoPlayPointL(
        const TUint32 aLastVideoPlayPoint );

private:

    CIptvLastWatchedApi();

    void ConstructL();
    
private: // data

    /**
     * File server session.
     */
    RFs iFsSession;
    
    };

#endif // C_IPTVLASTWATCHEDAPI_H
