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
* Description: 
*
*/




#ifndef IPTVPROVISIONINGAPPUI_H
#define IPTVPROVISIONINGAPPUI_H

// INCLUDES
#include <aknappui.h>

#include "MIptvServiceManagementClientObserver.h"

const TUid KRssPluginInterfaceUid = { 0x102750E0 }; // RSS Plugin interface UID
const TUid KXmlTvPluginInterfaceUid = { 0x1028242B }; // XML TV plugin interface UID
static const TInt KMaxNumberOfServices = 20;

class CIptvServiceManagementClient;
class CIptvServices;

//CLASS DECLARATION
/**
*	CIptvProvisioningAppUi class.
*	This class handles the operation betweed provisioning processor,
*	notifier framework (asks if user want to install parsed service)
*	and service api (used to add service to the database if user has
*	decided so)
*/
class CIptvProvisioningAppUi : public CAknAppUi,
							   public MIptvServiceManagementClientObserver
	{
    public:
        /**
        * C++ default constructor.
        * @param None
        * @return void
        */
        CIptvProvisioningAppUi();

        /**
        * Destructor
        * @param None
        * @return void
        */
        virtual ~CIptvProvisioningAppUi();

        // New methods
        /**
        *	Method to be called for CIptvProvisioningDocument when
        *	that classes OpenFileL method has been called and file
        *	containing the data to be parsed is received.
        *	@param	aFile	A handle to the file opened by framework
        *					to be used to parse contents of the service by¨
        *					provisioning processor.
        *	@return	None
        */
        void HandleFileL( RFile& aFile );

		/**
		*	Overwritten method from CEikAppUi. This method gets called
		*	when application is started by framework when file, which has
		*	mime type this applicaion knows, has been selected to be
		*	opened.
		*	@param	aCommand		Type of the command
		*	@param	aDocumentName	Name of the document to be opened
		*							When starting up application this value
		*							is empty
		*	@param	aTail			Command line's tail. Not used in this
		*							implementation.
		*	@return	ETrue if aDocument represents an existing file, otherwise
		*			EFalse.
		*/
    	TBool ProcessCommandParametersL( TApaCommand aCommand,
                                         TFileName& aDocumentName,
                                         const TDesC8& aTail );


		// From inherited MIptvServiceManagementClientObserver class
		// These methods are here because CIptvServiceManagementClient::NewL
		// requires a MIptvServiceManagementClientObserver type class to be
		// passed as a parameter. These methods are callbacks for the async
		// versions of the methods in the service management client and since
		// we use only sync versions, these are not used in here.

		/**
		*	See functionality of this method from the
		*	MIptvServiceManagementClientObserver class.
		*/
        void AddServiceResp(TRespStatus aRespStatus);

		/**
		*	See functionality of this method from the
		*	MIptvServiceManagementClientObserver class.
		*/
        void UpdateServiceResp(TRespStatus aRespStatus);


		/**
		*	See functionality of this method from the
		*	MIptvServiceManagementClientObserver class.
		*/
        void DeleteServiceResp(TRespStatus aRespStatus);

		/**
		*	See functionality of this method from the
		*	MIptvServiceManagementClientObserver class.
		*/
        void GetServicesResp(TRespStatus aRespStatus,
                                     CDesC8ArraySeg* aServicesArray);

		/**
		*	See functionality of this method from the
		*	MIptvServiceManagementClientObserver class.
		*/
        void GetUsedIapResp( TUint32 aIapId,
                             const TDesC& aIapName,
                             CIptvNetworkSelection::TConnectionPermission aConnectionPermission,
                             TBool aWlanWhenGPRS,
                             CIptvNetworkSelection::TRespStatus aRespStatus );

		/**
		*	See functionality of this method from the
		*	MIptvServiceManagementClientObserver class.
		*/
        void ServerShutdownResp(TRespStatus aRespStatus);

        /**
        * Not used yet.
        */
        void HandleSmEvent(CIptvSmEvent& /*aEvent*/) { };

    private:
        /**
        * EPOC default constructor.
        * @param None
        * @return void
        */
        void ConstructL();

	// New private methods
	private:

	    /**
	    *   Utility method that goes through existing services and checks
	    *   what IAPs they are using. These IAPs can be used for new service
	    *   if it doesn't define any IAPs of its own.
	    *
	    *   @param aClient  Client session for Service Manager.
	    *   @param aIapList On return, list of IAPs from other services.
	    *   @return KErrNone if successfull.
	    */
	    TInt GetIapListForNewServiceL(
	        CIptvServiceManagementClient* aClient,
	        CIptvIapList& aIapList ) const;

		/**
		*	Utility method to be used to ask from user is parsed service
		*	is to be installed to the service database or not.
		*	This method launches the notifier on top of everything to ask
		*	user wish considering the installation.
		*	@param	aService	Reference to the object containing data
		*						for the parsed service
		*	@return	ETrue if user has selected service to be installed,
		*			otherwise EFalse
		*/
		TBool DoesUserWantToInstallServiceL( CIptvService& aService ) const;

		/**
		*	Utility method to be used to ask from user if service that
		*	has already exists in service database is to be overwritten.
		*	This method launches the notifier on top of everything to ask
		*	user wish considering the installation.
		*   @param aServiceName Name of the service to install
		*
		*	@return	ETrue if user has selected service to be installed,
		*			otherwise EFalse
		*/
        TBool OverwriteExistingServiceL( const TDesC& aServiceName ) const;

		/**
		*	Method to handle logics of the service installation after prosessor
		*	has done it's parsing and we are making a decision whether service
		*	is valid for installation and user wants to install service.
		*	@param	aServices	Array of services prosessor found
		*	@return	None
		*/
		void HandleServicesAdditionL( CIptvServices* aServices );

		/**
		*	Method to check if a single service is valid. This checks the
		*	service type and plugin availability in the ECom. Also Live tv
		*	compilation flag is checked.
		*	@param	aService Service which validity is checked
		*	@return	ETrue if service is valid, otherwise EFalse
		*/
		TBool IsServiceValidL( CIptvService& aService );

        /**
        *   Method to find existing service group items.
        *   @param aClient Client session for Service Manager.
        *   @param aExistingGroupIds a array of existing ids.
        *   @return none
        */
        void FindExistingGroupedIdsL( CIptvServiceManagementClient* aClient,
                                      RArray<TUint32>& aExistingGroupIds );

        /**
        *   Method to delete existing left over service group items.
        *   @param aClient Client session for Service Manager.
        *   @param aExistingGroupIds a array of existing ids.
        *   @param aReceivedGroup a received group id.
        *   @return none
        */
        void DeleteExistingGroupedIdsL( CIptvServiceManagementClient* aClient,
                                        RArray<TUint32>& aExistingGroupIds,
                                        const TUint32 aReceivedGroup );

        /**
        *   Method to sort existing service group items.
        *   @param aClient Client session for Service Manager.
        *   @param aServices Array of services prosessor found.
        *   @param aGroupItemsOrder a array of order in incoming vcfg.
        *   @return none
        */
        void ForceOrderOfGroupedServicesL( CIptvServiceManagementClient* aClient,
                                           RArray<TUint32>& aGroupItemsOrder );

        /**
        *   Method to check if a service group belongs to existing feed.
        *   @param aClient  Client session for Service Manager.
        *   @param aService Service which validity is checked.
        *   @param aGroupId Group id where this service belongs.
        *   @param aLauchId Servce group id for lauch.
        *   @return ETrue if service is valid, otherwise EFalse
        */
        TBool VerifyServiceGroupL( CIptvServiceManagementClient* aClient,
                                   CIptvService& aService,
                                   TUint32& aGroupId,
                                   TUint32& aLauchId );

        /**
        *   Sets 'Received' flag if group items received.
        *   @param aClient  Client session for Service Manager.
        *   @param aGroupId Group id where this service belongs.
        *   @return None
        */
        void SetGroupReceivedL( CIptvServiceManagementClient* aClient,
                                const TUint32& aGroupId );

        /**
		*	Show invalid data note.
		*	@param	None
		*	@return	None
		*/
        void ShowInvalidDataNoteL() const;

        /**
		*	Show invalid service type note.
		*	@param	None
		*	@return	None
		*/
        void ShowInvalidServiceTypeNoteL() const;

        /**
		*	Show done note.
		*	@param	aFeed Indicates service was installed to Video Feeds folder
		*	@return	None
		*/
        void ShowDoneNoteL( TBool aFeed ) const;

        /**
        * Query added/updated service id by provider id.
        * @param aProviderId a provider id of wanted service.
        * @return service id of found service
        */
        TUint32 GetServiceIdByProviderIdL( CIptvServiceManagementClient* aClient,
                                           const TDesC& aProviderId );

        /**
        * Query added/updated service to open.
        * @param aServiceId Service id.
        * @param aGroupId Service group id.
        * @return None
        */
        void QueryAddedServiceOpenL( TUint32 aServiceId, TUint32 aGroupId );

    private:
        /**
        * Takes care of command handling.
        * @param aCommand command to be handled
        * @return void
        */
        void HandleCommandL( TInt aCommand);

        /**
        * handles key events
        * @param TKeyEvent& aKeyEvent
        * @param TEventCode aType
        * @return TKeyResponse
        */
        TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    private:

    	/** Handle to EComSession */
    	REComSession iEcomSession;

    };

#endif      // IPTVPROVISIONINGAPPUI_H

// End of File
