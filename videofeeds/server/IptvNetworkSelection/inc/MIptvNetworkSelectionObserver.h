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
* Description:    callback functions for Network Selection class*
*/





#ifndef MIPTVNETWORKSELECTIONOBSERVER_H
#define MIPTVNETWORKSELECTIONOBSERVER_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class TIptvGetUsedIapReq;

/**
*  Callback function interface for CIptvNetworkSelection.
*
*  @lib
*  @since
*/
class MIptvNetworkSelectionObserver
    {
        
    public: // New functions
        
        /**
        * Callback function for GetUsedIapReqL()
        * @since
        * @param aRequestOb is pointer to the TGetUsedIapReq object which
        *        was given in GetUsedIapReqL(). Observer can use it to
        *        identify which request was completed.
        * @return
        */
        virtual void GetUsedIapResp(TIptvGetUsedIapReq* aRequestOb) = 0;

    };

#endif      // MIPTVNETWORKSELECTIONOBSERVER_H  
            
// End of File
