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
* Description:    CVcxNsChPublisher class declaration*
*/




#ifndef VCXNSCHPUBLISHER_H
#define VCXNSCHPUBLISHER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "videocontentharvesterplugin.hrh"

class MLiwInterface;
class CLiwGenericParamList;
class CVcxNsChPublishableData;

// CLASS DECLARATION

/**
 * Class for handling data content for matrix menu's
 * tvvideo -suite.
 * 
 * Data to show is saved into Content Publishing
 * database using Liw -framework. After publishing, it is
 * up to menu definition to update appropriate fields.  
 *
 * Class has also a methods for backing up wanted content.
 * Backing up in this sence means saving the data to the 
 * content publishing database using different content id.
 * 
 *
 *  @lib vcxnscontentharverterplugin.dll
 */
class CVcxNsChPublisher : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CVcxNsChPublisher();

    /**
     * Two-phased constructor.
     * 
     * @param MLiwInterface* a pointer to sapi's Liw -inteface to publish data to     
     * @param CVcxNsChPublishableData* a pointer to data to publish
     * 
     * @return CVcxNsChPublisher*
     */
    static CVcxNsChPublisher* NewL( MLiwInterface* aCPInterface, 
                                    CVcxNsChPublishableData* aData );

    /**
     * Two-phased constructor.
     * 
     * @param MLiwInterface* a pointer to sapi's Liw -inteface to publish data to    
     * @param CVcxNsChPublishableData* a pointer to data to publish
     * 
     * @return  CVcxNsChPublisher*
     */
    static CVcxNsChPublisher* NewLC( MLiwInterface* aCPInterface,
                                     CVcxNsChPublishableData* aData );

private:

    /**
     * Constructor for performing 1st stage construction
     * 
     * @param MLiwInterface* a pointer to sapi's Liw -inteface to publish data to    
     * @param CVcxNsChPublishableData* a pointer to data to publish
     * 
     */
    CVcxNsChPublisher( MLiwInterface* aCPInterface,  
                       CVcxNsChPublishableData* aData );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
public: // new methods
    
    /**
     * Method calls PublishContentL for all content -types defined
     * 
     */
    void PublishAllL();
    
    /**
     * Method publishes content for given content type.
     * Data is fetched from the iPublishableData.
     * 
     * @param TVCxNsCPContentType content type to publish 
     * 
     */
    void PublishContentL( TVCxNsCPContentType aContentType );
    
    
    /**
     * Method deletes all defined content from the CP database
     * 
     * @param TVCxNsCPContentType content type to delete 
     * 
     */
    void DeleteContentL( TVCxNsCPContentType aContentType );
    
    /**
     * Method reads data content from the CP database.
     * 
     * Data is saved to iPublishableData
     * 
     * @param TVCxNsCPContentType content type to read
     * 
     */
    void ReadContentL( TVCxNsCPContentType aContentType );
    
    /**
     * Method backs up data content from the CP database.
     * Basically backing up means that same data is saved back
     * to the CP using different content id value than for the 
     * values used for the content to be shown in matrix menu.
     * 
     * if aRefreshFrDB is ETrue, local data content that is used to 
     * backup is updated from the CP DB before backing up.
     * 
     * if aRefreshFrDB is EFalse, local data content that is used to 
     * backup is not updated, but writtendirectly to backup db. 
     * 
     * @param TVCxNsCPContentType content type to backup
     * @param aReadFrDB see definition above
     * 
     */
    void BackupContentL( TVCxNsCPContentType aContentType, TBool aRefreshFrDB = ETrue );
    
    /**
     * Method reads backed up data content from the CP and
     * saves it locally to the data containers in iPublishableData.
     * After data is restored succesfully, backup content is removed 
     * from the CP.
     * 
     * In case backed up data needs to be shown in the menu,
     * PublishContentL -needs to be called after restore. 
     * 
     * @param TVCxNsCPContentType content type restore.
     * 
     */
    void RestoreBackupL( TVCxNsCPContentType aContentType );
       
private:
    
    /**
     * Helper method to extract data from CLiwDefaultMap for 
     * tvvideosuite needs
     * 
     * @param aContentType type 
     * @param aDataRoot a map containing result data gotten from CP 
     * 
     * @return KErrNone if data gotten ok
     */
    void ReadCPResultDataRootL( TVCxNsCPContentType& aContentType, CLiwDefaultMap* aDataRoot );
    
    /**
     * Method executes given command to CP database concerning
     * given contenttype
     * 
     * @param aContentType type 
     * @param aCmd command to execute
     * @param aInParam parameters in
     * @param aOutParam parameters out
     * 
     */
    void ExecCPCommandL(  TVCxNsCPContentType& aContentType,   
                          const TDesC8& aCmd,
                          CLiwGenericParamList* aInParam,
                          CLiwGenericParamList* aOutParam);
    
private: // DATA
    
    /**
     * Instance of CPS interface used for update with CPS.
     * Not owned
     */
    MLiwInterface* iCPSInterface;
    
    /**
     * pointer to actual data to be published
     * Not owned
     */
    CVcxNsChPublishableData* iPublishableData;
    
    /**
     * contenttype id string used to fetch content.
     * KContentDefaultId == "menucontentid" for normal data to be shown in matrix
     * KContentBackupId == "backucontentpid" for backed up data content
     */
    TPtrC iContentId;

    };

#endif // VCXNSCHPUBLISHER_H
