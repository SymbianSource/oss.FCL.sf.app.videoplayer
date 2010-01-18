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
* Description: 
*
*/




#ifndef CIPTVEPGPLUGININTERFACE_H
#define CIPTVEPGPLUGININTERFACE_H

#include <e32base.h>
#include <ecom/ecom.h>
#include "MIptvEpgVodCallback.h"
#include "MIptvEpgLiveTvCallback.h"

#include "CIptvEpgService.h"

class CIptvEpgPluginInterface : public CBase
	{
	public:

	    /**
	     * RSS Parser states.
	     */
	    typedef enum
	        {
	        EIptvRssNone,
	        EIptvRssService,
	        EIptvRssSearch,
	        EIptvRssServiceGroup,
	        EIptvRssAddServiceThumbnailToDlQueue,
	        EIptvRssAddItemThumbnailToDlQueue
	        } TIptvRSSAction;

	    /**
         * Two-phased constructor.
         *
         * @param aImplementationUid Plugin UID.
         */
		static CIptvEpgPluginInterface* NewL(const TUid aImplementationUid);
		
		/**
         * Destructor.
         */
		virtual ~CIptvEpgPluginInterface();
		
		/**
         * Run plugin.
         */
		virtual void RunPlugin() = 0;

        /**
         * Set callback pointer to MIptvEpgVodCallback. Plugin uses 
         * MIptvEpgVodCallback interface to supply Epg data to system.
         *
         * @param aVodCallback Pointer to callback interface.
         */
        virtual void SetVodCallback(MIptvEpgVodCallback* aVodCallback) = 0;

        /**
         * Set callback pointer to MIptvEpgLiveTvCallback.
         *
         * @param aLiveTvCallback Pointer to callback interface.
         */
        virtual void SetLiveTvCallback(MIptvEpgLiveTvCallback* aLiveTvCallback) = 0;

        /**
         * Set service information to plugin.
         */
        virtual void SetServiceInformation(
            CIptvEpgService* aService,
            TBool aUseSearchAddress = EFalse ) = 0;

        /**
         * Set service information to plugin.
         */
        virtual void SetServiceInformationL(
            CIptvEpgService* aService,
            TIptvRSSAction aAction ) = 0;

        /**
         * Set first update.
         *
         * @param aFirstUpdate aFirstUpdate is ETrue if plugin in running first time
         *                     after EPG database is created.
         */
        virtual void SetFirstUpdate( TBool aFirstUpdate ) = 0;
		
	private:
		TUid iDestructorIDKey;
	};

// This includes the implementation of the ECOM instantiation functions and destructor
#include "CIptvEpgPluginInterface.inl"

#endif  // CIPTVEPGPLUGININTERFACE_H
