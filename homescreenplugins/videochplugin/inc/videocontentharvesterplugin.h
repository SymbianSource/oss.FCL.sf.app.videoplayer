/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    CVcxNsContentHarvesterPlugin class declaration*
*/




#ifndef _VIDEOCONTENTHARVESTERPLUGIN_H
#define _VIDEOCONTENTHARVESTERPLUGIN_H 

// INCLUDES
#include <e32std.h>
#include <e32cmn.h>
#include <contentharvesterplugin.h>
#include <contentharvesterpluginuids.hrh>

// FORWARD DECLARATIONS

class CVcxNsChMCategoryListener;
class MLiwInterface;
class CLiwDefaultMap;
class CVcxNsContentHarvesterMpxClient;
class CVcxNsContentHarvesterServiceListener;
class CVcxNsNotificationLauncher;
class CVcxNsChPublishableData;
class CVcxNsChPublisher;
class CVcxNsChLastWatchedUpdater;
class CVcxNsChIptvServerClient;

// CLASS DECLARATION
/**
 *  Ns Content Harvester plugin
 *
 *  @lib vcxnscontentharvesterplugin.dll
 *  @since S60 v5.0
 */

class CVcxNsContentHarvesterPlugin : public CContentHarvesterPlugin
    {
    
public:

    static CVcxNsContentHarvesterPlugin* NewL( MLiwInterface* aCPSInterface );
    
    /**
     * Destructor
     * 
     */
    virtual ~CVcxNsContentHarvesterPlugin();

    /**
     * NotificationLauncher getter
     * 
     * @return CVcxNsNotificationLauncher* pointer to notification launcher
     */
    CVcxNsNotificationLauncher* NotificationLauncher();

    /**
     * From CContentHarvesterPlugin.
     *This function updates information to CP database during the phone bootup
     * 
     * 
     */
    void UpdateL();

private:
    
    /**
     * Symbian 2nd phase constructor
     * 
     */
    void ConstructL();

    /**
     * C++ constructor
     * 
     * @param aCPSInterface interface to service api (content publishing)
     */
    CVcxNsContentHarvesterPlugin( MLiwInterface* aCPSInterface );
    

private: // new methods
    
    /**
     * Internal helper method to update last watched data content 
     *  during the phone bootup.
     * 
     */
    void UpdateLastWatchedL();
    
    /**
     * Internal helper method to update preloaded video data content 
     * during the phone bootup.
     * 
     */
    void UpdatePreloadedVideoL();
    
    /**
    * Internal helper method to update predefined service data content 
    * during the phone bootup.
    * 
    */
    void UpdatePredefinedServiceL();
    
    
private: // DATA
       
    /**
     * CVcxNsContentHarvesterMpxClientOwned.
     */    
    CVcxNsContentHarvesterMpxClient* iMpxClient;
    
    /**
     * CVcxNsContentHarvesterServiceListener. Owned.
     */    
    CVcxNsContentHarvesterServiceListener* iServiceListener;
    
    /**
     * CVcxNsNotificationLauncher. Owned.
     */    
    CVcxNsNotificationLauncher* iNotificationLauncer;

    /**
     * Interface to content publishing gotten from the 
     * ContentHarvester server, not owned
     */
    MLiwInterface* iCPSInterface;
    
    /**
     * Publishable data container. Owned. 
     */
    CVcxNsChPublishableData* iPublishableData;
    
    /**
     * Publisher object used to publish data from CVcxNsChPublishableData
     * to service api. Owned. 
     */
    CVcxNsChPublisher* iPublisher;
  
    /**
     * Updater class for the last watched item. Owned.
     */
    CVcxNsChLastWatchedUpdater* iLwUpdater;
    
    /**
     * IPTV server interface. Owned.
     */
    CVcxNsChIptvServerClient* iIptvServerClient;
    
    };

#endif // _VIDEOCONTENTHARVESTERPLUGIN_H 
