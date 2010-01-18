/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for "service selection list" in Settings UI.*
*/



#ifndef VCXNSSETTINGSVIEWSERVICESELECTION_H
#define VCXNSSETTINGSVIEWSERVICESELECTION_H

// INCLUDES
#include <AknServerApp.h>
#include "feedsettingsviewcontainer.h"
#include "CIptvService.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CAknDoubleStyleListBox;
class CIptvService;
class CVcxConnectionUtility;
class CBrowserLauncher;

class CIptvLiveUIScheduledProgram;

// CLASS DECLARATION

/**
 * Class for "service selection list" in Settings UI.
 *
 *  @lib vcsettingsview.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CVcxNsSettingsViewServiceSelection ) : public CBase,
                                                          public MAknServerAppExitObserver
    {
public: // Constructors and desctructor.

    /**
     * @param aParent      Pointer to parent container.
     * @param aServiceSettings Reference to Service Settings in UI Engine.
     */
    static CVcxNsSettingsViewServiceSelection* NewL(CVcxNsSettingsViewContainer* aParent,
            CVcxNsServiceSettings& aServiceSettings);

    virtual ~CVcxNsSettingsViewServiceSelection();

    /**
     * Checks if msk needs tp be updated and does so if it is needed
     */
    void CheckMsk( );
    
protected: //From MAknServerAppExitObserver

    /**
     * Handle the exit of a connected server app.
     * This implementation provides Series 60 default behavior
     * for handling of the EAknCmdExit exit code. Derived classes
     * should base-call this implementation if they override this
     * function.
     * @param aReason The reason that the server application exited.
     * This will either be an error code, or the command id that caused
     * the server app to exit.
     * ( empty implementation here )
     */
     void HandleServerAppExit( TInt /*aReason*/) { ; }

private:
    /**
     * Returns true if currently active list item is marked.
     *
     * @return ETrue if currently active list item is marked.
     */
    TBool IsCurrentListItemMarked();

public:

    /**
     * Activates "service selection list".
     */
    void ActivateL();

    /**
     * Deactivates "service selection list".
     */
    void Deactivate();

    /**
     * Sets service type used in "service selection list".
     *
     * @param aServiceType Service type as CIptvService::TServiceType 
     *                     enumeration.
     */
    void SetServiceType(CIptvService::TServiceType aServiceType);

    /**
     * Returns service type in use.
     *
     * @return Service type as CIptvService::TServiceType enumeration.
     */
    CIptvService::TServiceType GetServiceType();

    /**
     * Returns true if currently active service is read-only.
     *
     * @return ETrue if currently active service is read-only.
     */
    TBool IsCurrentServiceReadOnlyL();

    /**
     * Returns currently active service. Ownership of object is given
     * to caller.
     *
     * @return Pointer to currently active service.
     */
    CIptvService* GetCurrentServiceL();

    /**
     * Deletes currently active service.
     */
    void DeleteCurrentServiceL();

    /**
     * Loads data to "service selection list".
     */
    void LoadL();

    /**
     * Saves data in "service selection list".
     */
    void SaveL();

    /**
     * Returns pointer to "service selection item" listbox. No ownership transfer.
     *
     * @return Pointer to listbox.
     */
    CAknSingleGraphicStyleListBox* ListBox();

    /**
     * Shows message query dialog for managing an account before removing service
     *
     * @return Dialog's result, what user has replied to query.
     */
    TInt ShowAccountMgmtDialogL();

    /**
     * Used as a callback function in message query.
     * @return System-wide error code.
     */
    static TInt ShowLinkL( TAny* ptr );

    /**
     * Opens account management Url with embedded browser.
     */ 
    void OpenEmbeddedBrowserL( const TDesC& aUri );
    
private: // New methods.

    /**
     * Gets current service id.
     *
     * @param aServiceId service id
     */
    void GetServiceIdL( TUint32& aServiceId );

    /**
     * Removes commands from CBA
     * @param aCba Button group container
     */
    void RemoveCommandsFromCba( CEikButtonGroupContainer& aCba );

private: // Construction.

    /**
     * @param aParent      Pointer to parent container.
     * @param aServiceSettings Reference to Service Settings in UI Engine.
     */
    CVcxNsSettingsViewServiceSelection(CVcxNsSettingsViewContainer* aParent,
            CVcxNsServiceSettings& aServiceSettings);

    void ConstructL();

private: // Data

    /**
     * Parent control container
     */
    CVcxNsSettingsViewContainer* iParent;
    
    /**
     * UI engine's application settings provider
     */
    CVcxNsServiceSettings& iServiceSettings;
    
    /**
     * List box control
     */
    CAknSingleGraphicStyleListBox* iListbox;

    /**
     * Is own command set used
     */ 
    TBool iOwnButtons;

    /**
     * Service array
     */
    CDesC8ArraySeg* iServicesArray;

    /**
     * Service array for vod casts
     */
    CDesC8ArraySeg* iServicesArrayVodCast;
      
    /**
     * Service type
     */
    CIptvService::TServiceType iServiceType;
    
    /**
     * Current Msk resource id.
     */
    TInt iCurrentMskResource;
    
    /*
    * Launcher that handles opening of web link in embedded browser
    * Own.
    */
    CBrowserLauncher* iLauncher;

    /**
     * Connection utility.
     * Not own.
     */
    CVcxConnectionUtility* iConnUtil;
    
    };

#endif // VCXNSSETTINGSVIEWSERVICESELECTION_H
