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




#ifndef CIPTVOMAPROVISIONINGADAPTER_H
#define CIPTVOMAPROVISIONINGADAPTER_H

//  INCLUDES
#include <CWPAdapter.h>
#include <MWPContextExtension.h>

#include "CIptvService.h"
#include "CIptvServices.h"
#include "CIptvServiceManagementClient.h"

// CONSTANTS
const TInt KIptvOcpMaxSummaryTitleLength = 100;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CWPCharacteristic;

// CLASS DECLARATION

/**
*  Realizes IPTV OMA provisioning adapter.
*
*  @lib IptvOmaProvisioningAdapter.lib
*/
class CIptvOmaProvisioningAdapter :  public CWPAdapter,
                                     public MWPContextExtension,
                                     public MIptvServiceManagementClientObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIptvOmaProvisioningAdapter* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CIptvOmaProvisioningAdapter();
       
    public: // Methods from base classes 
  
        // From CWPAdapter
        /**
        * Returns the number of the summary lines to be shown in 
        * BIO control.
        * @since Series60 3.0.
        * @return TInt. Quantity of items.
        */
        TInt  ItemCount() const;

       /**
        * Getter for the summary text of the service item. 
        * located in the array. Text is shown to the user in 
        * opened configuration message.
        * @since Series60 3.0.
        * @param TInt aIndex. Location of the service item.
        * @return TDesC16&. Summary title.
        */
        const TDesC16& SummaryTitle( TInt aIndex ) const;

       /**
        * Getter for the summary text of the service item. 
        * located in the array. Text is shown to the user in 
        * opened configuration message.
        * @since Series60 3.0.
        * @param TInt aIndex. Location of the service item.
        * @return TDesC16&. Summary text.
        */
        const TDesC16& SummaryText( TInt aIndex ) const;

       /**
        * Saves service item located in array.
        * @since Series60 3.0.
        * @param TInt aIndex. Location of the service item to be saved.
        */
        void SaveL( TInt aIndex );

       /**
        * Query if the service item on place pointed by parameter can 
        * be set as default.
        * @since Series60 3.0.
        * @param TInt aIndex. Location of the service item to be queried.
        * @return TBool.
        */
        TBool CanSetAsDefault( TInt aIndex ) const;
    
       /**
        * Sets the service item on place pointed by parameter as default.
        * @since Series60 3.0.
        * @param TInt aIndex. Location of the VoIP item to be queried.
        */
        void  SetAsDefaultL( TInt aIndex );

       /**
        * Query for the detail information about the service settings. 
        * MWPPairVisitor is used for retrieving the details of a single 
        * setting entry. This is not supported feature as in the other 
        * adapters in the framework.
        * @since Series60 3.0.
        * @param TInt aItem.
        * @param MWPPairVisitor aVisitor.
        * @return TInt. 
        */
        TInt  DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

       /**
        * Visit method. Visits the adapter. Adapter then checks from the 
        * aCharacteristic if the data is targetet to it and acts according to 
        * that.
        * @since Series60 3.0.
        * @param CWPCharacteristic& aCharacteristic.
        */
        void  VisitL( CWPCharacteristic& aCharacteristic );		

       /**
        * Visit method. Visits the adapter. Method set the iAppId value.
        * @since Series60.
        * @param CWPParameter& aParameter.
        */
        void  VisitL( CWPParameter& aParameter );		

       /**
        * Visit method. Not needed, but here because of inheritation.
        * @since Series60.
        * @param CWPCharacteristic& aLink.
        */
        void  VisitLinkL( CWPCharacteristic& /*aLink*/ )
            {
            };
            
        /**
        * Informs the adapters of the saved settings.
        * @since 3.0.
        * @param TDesC8& aAppIdOfSavingItem. UID of the adapter component. 
        * @param HBufC8& aAppRef. APPREF of the saved settings.
        * @param TAny& aStorageIdValue. Value that identifies the settings in 
        *        its storage.
        * @param TBool aIsLastOfItsType. If the settings saved is the last of 
        *        its kind, the value of the parameter is ETrue (or 1, because 
        *        the TBool type does not work correctly via interface class 
        *        definition (based on information from MVC documentation).
        */
        void SettingsSavedL (const TDesC8& aAppIdOfSavingItem,
        const TDesC8& aAppRef, const TDesC8& aStorageIdValue );

        /**
        * Lets the adapters know when the saving has been finalized in 
        * viewpoint of the provisioning framework. The adapters can make 
        * finalization of saving the settings after receiving this call.
        * @since Series60 3.0
        */
        void SavingFinalizedL();

    // From MWPContextExtension.

        /**
        * Returns a pointer to a context extension.
        * @param aExtension Contains a pointer to MWPContextExtension if supported
        * @return KErrNotSupported if not supported, KErrNone otherwise
        */
        TInt ContextExtension(MWPContextExtension*& aExtension);

        /**
        * Returns the data used for saving.
        * @param aIndex The index of the data
        * @return The data. Ownership is transferred.
        */
        const TDesC8& SaveDataL(TInt aIndex) const;

        /**
        * Deletes a saved item.
        * @param aSaveData The data used for saving
        */
        void DeleteL(const TDesC8& aSaveData);

        /**
        * Returns the UID of the adapter.
        * @return UID
        */
        TUint32 Uid() const;
    
    public:
        /**
        * From MServiceManagementClientObserver.h see comments from there.
        * These implementations are empty.
        */
        void AddServiceResp( TRespStatus aRespStatus );
        void UpdateServiceResp( TRespStatus aRespStatus );
        void DeleteServiceResp( TRespStatus aRespStatus );
        void GetServicesResp( TRespStatus aRespStatus, 
                              CDesC8ArraySeg* aServicesArray );
        void GetUsedIapResp( TUint32 aIapId,
                             const TDesC& aIapName,
                             CIptvNetworkSelection::TConnectionPermission aConnectionPermission,
                             TBool aWlanWhenGPRS,
                             CIptvNetworkSelection::TRespStatus aRespStatus );
        void ServerShutdownResp( TRespStatus aRespStatus );
        void HandleSmEvent( CIptvSmEvent& aEvent );
           
    private:

        /**
        * C++ default constructor.
        */
        CIptvOmaProvisioningAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Set given icon path. Whether it is drive independent filepath,
        * searches correct drive and fix it
        *
        * @param aIconPath Icon path.
        */
        void SetIconPathL( const TDesC& aIconPath );

    private: // Data
        
        /**
        * Stores application ID when receiving provisioning data.
        */
        TPtrC                               iAppId;
        
        /**
        * iCurrentService is added to this when it has fully constructed.
        * iServices will contain all provisioned services once the processing is finished.
        */
        CIptvServices*                      iServices;
        
        /**
        * The current provision item being parsed is stored here.
        */
        CIptvService*                       iCurrentService;
                
        /**
        * Localized summary title is stored here.
        */
        TBuf<KIptvOcpMaxSummaryTitleLength> iSummaryTitle;

        /**
	    * File server session
	    */
	    RFs iFs;

    };

#endif      // CIPTVOMAPROVISIONINGADAPTER_H
            
// End of File
