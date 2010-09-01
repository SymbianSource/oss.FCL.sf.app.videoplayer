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
* Description:    Observer interface for phone registration.*
*/




#ifndef CIPTVPHONEREGISTRATIONOBSERVER_H
#define CIPTVPHONEREGISTRATIONOBSERVER_H

/**
 *  Observer interface for battery watcher.
 *
 *  @lib iptvscheduleddownloadplugin.dll
 */
class MPhoneRegistrationWatcherObserver
    {
public:

    /**
     * This callback will be called when the home network state of the phone
     * registration changes.
     */
    virtual void OnHomeNetwork( TBool aOnHomeNetwork ) = 0;
    };

#endif // CIPTVPHONEREGISTRATIONOBSERVER_H
