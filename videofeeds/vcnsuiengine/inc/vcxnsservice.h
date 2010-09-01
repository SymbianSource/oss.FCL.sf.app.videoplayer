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
* Description: 
*
*/




#ifndef C_VCXNSVODSERVICE_H
#define C_VCXNSVODSERVICE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

_LIT( KVcxNsVideoDirectory, "qtn_iptv_video_directory_list" );

// CLASS DECLARATION

/**
 *  CVcxNsService
 * 
 * @lib vcxnsuiengine.lib
 */
class CVcxNsService : public CBase
    {
    
public:

    /**
     * Service's update status enum.
     */
    typedef enum
        {
        ENotUpdating = 0,
        EUpdatePending,
        EStartingPending,
        EUpdateOngoing,
        EUpdateFailed,
        } TServiceUpdateStatus;
    
    /**
    * VOD feed type information 
    */
    typedef enum 
        {
        EVcxNsReadOnly = 0,
        EVcxNsEditable,
        EVcxNsMainGroup,
        EVcxNsVideoDirectory,
        EVcxNsGroupedVod,
        EVcxNsGroupedBrowser,
        EVcxNsGroupedSearch,
        EVcxNsGroupedUpload
        } TVcxNsVodServiceType;

     /**
     * Constructor
     */
    static CVcxNsService* NewL();

    /**
     * Destructor
     */
    virtual ~CVcxNsService();

    /**
     * GetName
     * @return Name
     */
    IMPORT_C const TDesC& GetName();
    
    /**
     * GetDesc
     * @return Description
     */
    IMPORT_C const TDesC& GetDesc();
    
    /**
     * GetIconPath
     * @return IconPath
     */
    IMPORT_C const TDesC& GetIconPath();
    
    /**
     * GetUserName
     * @return User name
     */
    IMPORT_C const TDesC& GetUserName();
        
    /**
     * GetPassword
     * @return Password
     */
    IMPORT_C const TDesC& GetPassword();
    
    /**
     * GetUpdateTime 
     * @return update time
     */
    IMPORT_C TTime GetUpdateTime();
        
    /**
     * GetUpdateStatus
     * @return Update status
     */
    IMPORT_C TServiceUpdateStatus GetUpdateStatus();

    /**
     * GetIap
     * @return Iap
     */
    IMPORT_C TUint32 GetIap();
    
    /**
     * GetIsReadOnlyIap
     * @return Is read only iap
     */
    IMPORT_C TBool GetIsReadOnlyIap();
    
    /**
     * Get simplified service type.
     * @return Service type
     */
    IMPORT_C TVcxNsVodServiceType Type() const;

    /**
     * Get account management uri.
     * @return Account management uri. 
     * KNullDesC() if not available.
     */
    IMPORT_C const TDesC& AccountMgmtUri();
    
    /**
     * Get service custom URI.
     * @return Custom uri
     */
    const TDesC& CustomUri();
    
    /**
     * SetNameL
     * @param aName Name
     */
    void SetNameL( const TDesC& aName );

    /**
     * SetDescL
     * @param aDesc Description
     */
    void SetDescL( const TDesC& aDesc );
    
    /**
     * SetIconPathL
     * @param aPath icon path
     */
    void SetIconPathL(  const TDesC& aPath );
    
    /**
     * SetUserNameL
     * @param aUserName Username
     */
    void SetUserNameL( const TDesC& aUserName );
    
    /**
     * SetPasswordL
     * @param aPassword Password
     */
    void SetPasswordL( const TDesC& aPassword );
    
    /**
     * SetServiceId
     * @param iServiceId Service Id 
     */
    void SetServiceId( const TUint32 iServiceId );

    /**
     * GetServiceId
     * @return Service Id 
     */
    IMPORT_C TUint32 GetServiceId();
    
    /**
     * SetUpdateTime
     * @param aTime Update time
     */
    void SetUpdateTime( const TTime aTime );

    /**
     * SetUpdateStatus
     * @param aUpdateStatus Update status
     */
    void SetUpdateStatus( const TServiceUpdateStatus aUpdateStatus );
    
    /**
     * SetIap
     * @param aIap Iap id
     */
    void SetIap( const TUint32 aIap );
    
    /**
     * SetIsReadOnlyIap
     * @param aIsReadOnlyIap Is read only iap
     */
    void SetIsReadOnlyIap( const TBool aIsReadOnlyIap );
    
    /**
     * Set browser url for browser type service
     * @param aCustomUri Custom uri
     */
    void SetCustomUriL( const TDesC& aCustomUri ); 
    
    /**
     * Set type of the service
     * @param aType Service type
     */
    void SetType( const CVcxNsService::TVcxNsVodServiceType aType );
    
    /**
     * Set account management url for service
     * @param aAccountMgmtUri Account management uri.
     */
    void SetAccountMgmtUriL( const TDesC& aAccountMgmtUri );

    /**
     * Set group id.
     * @param aGroupId Group id
     */
    void SetGroupId( const TUint32 aGroupId );
    
    /**
     * Get group id.
     * @return Service group id.
     */
    TUint32 GroupId();
    
    /**
     * Add sub service to service group.
     * @param aSubService Groupped service. Ownership transferred here.
     * Leaves with KErrArgument if aSubService is NULL or if service type
     * is something else than EVcxNsMainGroup.
     */
    void AddSubServiceL( CVcxNsService* aSubService );
    
    /**
     * Return service group.
     * @return Array of groupped services.
     */
    RPointerArray<CVcxNsService>& ServiceGroup();

    /**
     * Set highlighted category index.
     * @param aIndex Highlighted index.
     */
    void SetHighlightCategoryIndex( TInt aIndex );

    /**
     * Return service group.
     * @return Highlighted index.
     */
    TInt HighlightCategoryIndex();

private:

    /**
     * Constructor
     */
    CVcxNsService();

private: //Data

    /**
     * Name.
     * Own.
     */
    HBufC* iName;

    /**
     * Icon path.
     * Own.
     */
    HBufC* iIconPath;

    /**
     * User name
     * Own.
     */
    HBufC* iUserName;

    /**
     * Password
     * Own.
     */
    HBufC* iPassword;
    
    /**
     * Service id.
     */
    TUint32 iServiceId;
    
    /**
     * Last updated time
     */
    TTime iUpdateTime;
    
    /**
     * Current update status.
     */    
    TServiceUpdateStatus iUpdateStatus;
    
    /**
     * Access point.
     */   
    TUint32 iIap;
    
    /**
     * Flag for read only iap.
     */
    TBool iIsReadOnlyIap;
    
    /**
     * Optional URI for browser service
     * Own.
     */
    HBufC* iCustomUri;
    
    /**
     * Service type. 
     */
    TVcxNsVodServiceType iType;
    
    /**
     * Service description
     * Own.
     */
    HBufC* iDescription;
    
    /**
     * Account management uri
     * Own.
     */
    HBufC* iAccountMgmtUri;
    
    /**
     * Service group id.
     */
    TUint32 iGroupId;
    
    /**        
    * Groupped services.
    * Own.
    */
    RPointerArray<CVcxNsService> iServiceGroup;
    
    /**        
    * Highlighted category index for service.
    * By default, always 0.
    */
    TInt iHighlightIndex;

    };

#endif // C_VCXNSVODSERVICE_H
