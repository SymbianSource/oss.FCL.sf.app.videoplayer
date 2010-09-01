/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Common data structures shared by IP protocol livetv*
*/




#ifndef __IPTV_SMARTVISION_DATA_STRUCTURES
#define __IPTV_SMARTVISION_DATA_STRUCTURES
enum TIptvSmartVisionServiceType
    {
    ELiveTv = 0,
    EVod,
    EServiceDiscovery
    };
    
struct SIptvSmartVisionServiceDiscovery
	{
	TBuf8<255> iAddress;
	TIptvSmartVisionServiceType iServiceType;
	};

/**
 * This enum identifies different kind of messages
 * in message queue that UI listens to
 */
enum TIptvQueueMsg
	{
	EEPGUpdateCompleted,   /**< LiveTV metadata update finished, possibly with error */
	EEPGUpdateStarted,	   /**< LiveTV metadata update started */
	EChannelDataModified,  /**< LiveTV channel object has been changed */
	EChannelIconDownloaded,
	EIapUp,                /**< Internet access point is up in CR engine */
	EIapDown,              /**< Internet access point is down in CR engine */
	EStartingConnection,
	EBothAudioVideoExists,
	EAudioOnly,            /**< LiveTV stream contains only audio, no video */
	EVideoOnly,            /**< LiveTV stream contains only video, no audio */
	ENoAudioOrVideo,       /**< LiveTV stream contains both audio and video */
	EAuthenticationFailed, /**< RTSP server said 401 to common recording engine */
	EScheduleCompleted,
	ECRROPSDPReady,         /**< Common recording engine has SDP file ready for ROP */
	EConnectionError	    /**< Unable to open connection in RTP engine & CR engine */
	} ;
/**
 * This structure is sent over message queue where
 * ui sits listening
 */
struct SIptvQueueEntry
	{
	TIptvQueueMsg iMsg ;        /**< Identifies the message */
	TUint iServiceId ;          /**< Service provider that this message is about */
	TInt64 iModifiedChannelId ; /**< In case of EChannelDataModified the channel id */
	TInt iErr ;                 /**< May be used to pass error value related to message */
	};

/**
 * This structure is used when downloading channel icons
 */
struct SIptvChannelIconEntry
	{
	TUint32 iServiceId ;          /**< Service provider that this message is about */
	TInt64 iChannelId ; 
	TBuf8<256> iLogoPath;
	};

/**  
* Version of CleanupXxxxPushL() that can be used to 'ResetAndDestroy'
* the supplied object if the clean-up stack unwinds.
*/

template <class T>
class CleanupResetAndDestroy
    {
    public:
        inline static void PushL( T& aRef );
    private:
        static void ResetAndDestroy( TAny *aPtr );
    };
    
template <class T>
    inline void CleanupResetAndDestroyPushL( T& aRef );

template <class T>
inline void CleanupResetAndDestroy<T>::PushL( T& aRef )
     {
     CleanupStack::PushL( TCleanupItem( &ResetAndDestroy, &aRef ) );
     }

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy( TAny *aPtr )
    {
    ( static_cast<T*>( aPtr ) )->ResetAndDestroy();
    }
    
template <class T>
inline void CleanupResetAndDestroyPushL( T& aRef )
    {
    CleanupResetAndDestroy<T>::PushL( aRef );
    }
    
/**
 * Name of the queue that is used in creating and opening the queue
 */
_LIT(KIptvUiMsgQueueName, "IPTV_MSG_Q_UI") ; 

_LIT(KIptvUiEPGMsgQueueName, "IPTV_EPG_MSG_QUEUE" );

#endif //__IPTV_SMARTVISION_DATA_STRUCTURES
