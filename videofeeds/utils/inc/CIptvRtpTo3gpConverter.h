/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    .rtp to .3gp file converter*
*/




#ifndef CIPTVRTPTO3GPCONVERTER_H
#define CIPTVRTPTO3GPCONVERTER_H

#include <e32base.h>

class RFs;
class MIptvRtpTo3gpObserver;

/**
 * .rtp to .3gp file converter
 *
 * @lib   IptvUtil.lib
 * @since Video Center 1.0
 */
class CIptvRtpTo3gpConverter : public CBase
    {
public: // Public constructors and destructors

    /**
     * Two-phased constructor
     *
     * @param aFs
     * @param aObserver  Observer to receive conversion notifications
     * @param aPriority
     */
    static CIptvRtpTo3gpConverter* NewL( RFs& aFs, MIptvRtpTo3gpObserver& aObserver, TInt aPriority );

    /**
     * C++ destructor
     */
    virtual ~CIptvRtpTo3gpConverter();

private: // private constructors

    /**
     * C++ default constructor
     *
     * @param aFs
     * @param aObserver
     */
    CIptvRtpTo3gpConverter( RFs& aFs, MIptvRtpTo3gpObserver& aObserver );

    /**
     * 2nd phase constructor
     *
     * @param aPriority
     */
    void ConstructL( TInt aPriority );

public: // public user API

    /**
     * Requests conversion generation
     *  
     * Conversion completation and failures are notified by the registered observer
     *  
     * @param aSrcRtpFilename  Filename for source .rtp clip
     * @param aDst3gpFilename  Filename for destination .3gp clip
     */
    void ConvertL( const TDesC& aSrcRtpFilename, const TDesC& aDst3gpFilename );

    /**
     * Cancels ongoing file conversion
     */
    void CancelConverstion();

private: // implementation, own methods

    /**
     * Actual cleanup for internal data/state
     *
     * @param aStatus status for conversion operation. If different than KErrNone, output
     *                file is removed
     */
    void DoCleanup( TInt aStatus );

    /**
     * Launches idle cleanup and notification
     *
     * @param aStatus convertion status, passed to DoCleanup
     */
    void DoIdleCleanupAndNotify( TInt aStatus );

    /**
     * Callback to cleanup up and notify
     *
     * @param aSelf wrapped 'this' pointer
     */
    static TInt CleanupAndNotifyCallback( TAny* aSelf );

private: // implementation, own data

    /**
     * Reference to file system server.
     */
    RFs& iFs;

    /**
     * Conversion observer
     */
    MIptvRtpTo3gpObserver& iObserver;

    /** 
     * Cache for 3gp file name
     * Own.
     */
    HBufC* i3gpFilename;

    /**
     * Idle callback cleanup and notify
     * Own.
     */
    CIdle* iIdle;

    /**
     * Status for converstion, passed through idle cleanup
     */
    TInt iIdleStatus;

    /**
     * Signals that a first keyframe has been written to file
     */
    TBool iKeyframeWritten;
    };

#endif // CIPTVRTPTO3GPCONVERTER_H
