/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef MIPTVEPGMANAGERUPDATEOBSERVER_H
#define MIPTVEPGMANAGERUPDATEOBSERVER_H

// INCLUDES

// FORWARD DECLARATIONS

// CLASS DECLARATION

class MIptvEpgManagerUpdateObserver
    {
    public:
      
        /**        
         * Check is plugin running.
         * 
         * @param   None.         
         */
        virtual void CheckIsPluginRunning() = 0;
    };

#endif  // MIPTVEPGMANAGERUPDATEOBSERVER_H

// End of File
