/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Contains Video Center's last watched item's data.*
*/




#ifndef C_IPTVLASTWATCHEDDATA_H
#define C_IPTVLASTWATCHEDDATA_H

#include <e32base.h>
#include "CIptvUtil.h"

// Consts
const TInt KIptvLastWatchedStartparamMaxLength = 16384; // 16kB
const TInt KIptvLastWatchedUriMaxLength = 2048;
const TInt KIdUndefined = KMaxTInt32 - 1;

/**
 *  Last watched data class
 *
 *  VideoCenter Main UI has the last watched item, data of that item can be set
 *  and get via the API by this class. This is used also when there is
 *  Space UI support available, this class contains MainUI's last watched
 *  item's launch data.
 *
 *  @lib IptvUtil.lib
 *  @since S60 v3.1
 */
class CIptvLastWatchedData : public CBase
    {

public: // structs

    /**
     * Type of the launch of last watched item.
     */
    enum TLaunchType
        {
        /**
         * Not defined.
         */
        EUndefined,

        /**
         * Activate VideoCenter's local view by view server id and pass parameters for the view.
         *
         * Last watched command handling uses CAknViewAppUi::ActivateLocalViewL()
         */
        EView,

        /**
         * Start application by app and view uid and pass parameters for the
         * application.
         *
         * Last watched command handling uses CCoeAppUi::ActivateViewL();
         */
        EApplication,

        /**
         * Send message to an application by app and view uid and pass
         * parameters for the application. Application should be running if
         * using this method.
         *
         * Last watched command handling uses TApaTask::SendMessageL
         */
        EHandleMessage,

        /**
         * Try to send a message to the application by app and view uid
         * and pass parameters for the application. If application is not
         * running it is started via apparc.
         *
         * Last watched command handling uses RApaLsSession::StartApp
         */
        EApaMsgWithTail
        };

public: // member functions

    IMPORT_C static CIptvLastWatchedData* NewL();
    virtual ~CIptvLastWatchedData();

    /**
     * Read data stream and construct this class from it.
     * @param aStream Stream to read data from.
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

    /**
     * Write class data to binary format which can be stored in a file.
     * @param aStream The Stream where data is written.
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream );

    /**
     * Sets the last watched item's name
     * Max length = KIptvMyVideosVideoNameMaxLength = 100
     * @since S60 v3.1
     * @param aName item's name
     * @return System-wide error code
     */
    IMPORT_C TInt SetNameL( const TDesC& aName );

    /**
     * Sets the last watched item's URI
     * Max length = KIptvLastWatchedUriMaxLength = 2048
     * @since S60 v3.1
     * @param aUri Item's uri, works as unique identifier.
     * @return System-wide error code
     */
    IMPORT_C TInt SetUriL( const TDesC& aUri );

    /**
     * Sets the last watched item's icon path
     * Max length = KIptvMaxPath = 255
     * @since S60 v3.1
     * @param aIconPath item's icon path
     * @return System-wide error code
     */
    IMPORT_C TInt SetIconPathL( const TDesC& aIconPath );

    /**
     * Sets the last watched item's mime type
     * Max length = KIptvMyVideosVideoMaxMimeTypeLength
     * @since S60 v3.2
     * @param aMimeType item's mime type
     * @return System-wide error code
     */
    IMPORT_C TInt SetMimeTypeL( const TDesC& aMimeType );

    /**
     * Sets the last watched item's service id
     * @since S60 v3.2
     * @param aServiceId item's service id
     * @return System-wide error code
     */
    IMPORT_C TInt SetServiceId( const TInt32 aServiceId );

    /**
     * Sets the last watched item's content id
     * @since S60 v3.2
     * @param aContentId item's content id
     * @return System-wide error code
     */
    IMPORT_C TInt SetContentId( const TIptvContentId aContentId );

    /**
     * Sets the last watched item's content index
     * @since S60 v3.2
     * @param aContentIndex item's content index
     * @return System-wide error code
     */
    IMPORT_C TInt SetContentIndex( const TInt32 aContentIndex );

    /**
     * Sets the last watched item's last play point
     * @since S60 v3.2
     * @param aLastVideoPlayPoint item's last play point
     * @return System-wide error code
     */
    IMPORT_C TInt SetLastVideoPlayPoint( const TUint32 aLastVideoPlayPoint );

    /**
     * Sets the last watched item's parental control limit
     * @since S60 v3.2
     * @param aParentalControl item's parental control limit
     * @return System-wide error code
     */
    IMPORT_C TInt SetParentalControl( const TUint32 aParentalControl );

    /**
     * Sets the last watched item's launch parameters. This can be command
     * line, TPckg<?Datatype?>, or anything that the launched application
     * takes as start / activation parameter.
     * Max length = KIptvLastWatchedStartparamMaxLength = 16k
     * @since S60 v3.1
     * @param aParameters item's launch parameters
     * @return System-wide error code
     */
    IMPORT_C TInt SetParametersL( const TDesC8& aParameters );

    /**
     * Sets the last watched item's launched application's Uid.
     * Needed only if item's launch type is EApplication.
     * @since S60 v3.1
     * @param aAppUid launched application's Uid
     * @return System-wide error code
     */
    IMPORT_C void SetAppUid( const TUid& aAppUid );

    /**
     * Sets the last watched item's launched application view's Uid.
     * @since S60 v3.1
     * @param aViewUid activated window server view's uid.
     * @return System-wide error code
     */
    IMPORT_C void SetViewUid( const TUid& aViewUid );

    /**
     * Sets the last watched item's parameter uid.
     * @since S60 v3.1
     * @param aParameterId
     * @return System-wide error code
     */
    IMPORT_C void SetParameterId( const TUid& aParameterId );

    /**
     * Sets the last watched item's launch type, see TLaunchType enum.
     * Type of launching of the last watched item. Launcing can be handled
     * by activating VideoCenter's local view, send mssage to self or some
     * other application, or launch any application with any given parameters.
     *
     * @since S60 v3.1
     * @param aLaunchType last watched item's launch type
     * @return System-wide error code
     */
    IMPORT_C void SetLaunchType( TLaunchType aLaunchType );

    /**
     * Gets the last watched item's name
     * Max length = KIptvMyVideosVideoNameMaxLength = 100
     * @since S60 v3.1
     * @return aName item's name
     */
    IMPORT_C const TDesC& Name() const;

    /**
     * Gets the last watched item's URI
     * Max length = KIptvLastWatchedUriMaxLength = 2048
     * @since S60 v3.1
     * @return aUri item's uri
     */
    IMPORT_C const TDesC& Uri() const;

    /**
     * Gets the last watched item's icon path
     * Max length = KIptvMaxPath = 255
     * @since S60 v3.1
     * @return aIconPath item's icon path
     */
    IMPORT_C const TDesC& IconPath() const;

    /**
     * Gets the last watched item's mime type
     * Max length = KIptvMyVideosVideoMaxMimeTypeLength
     * @since S60 v3.2
     * @return aIconPath item's mine type
     */
    IMPORT_C const TDesC& MimeType() const;

    /**
     * Gets the last watched item's service id
     * @since S60 v3.2
     * @return item's service id
     */
    IMPORT_C TInt32 ServiceId() const;

    /**
     * Gets the last watched item's content id
     * @since S60 v3.2
     * @return item's content id
     */
    IMPORT_C TIptvContentId ContentId() const;

    /**
     * Gets the last watched item's content index
     * @since S60 v3.2
     * @return item's content index
     */
    IMPORT_C TInt32 ContentIndex() const;

    /**
     * Gets the last watched item's last play point
     * @since S60 v3.2
     * @return item's last play point
     */
    IMPORT_C TUint32 LastVideoPlayPoint() const;

    /**
     * Gets the last watched item's parental control limit
     * @since S60 v3.2
     * @return item's parental control limit.
     */
    IMPORT_C TUint32 ParentalControl() const;

    /**
     * Gets the last watched item's launch parameters. This can be command
     * line, TPckg<?Datatype?>, or anything that the launched application
     * takes as start / activation parameter.
     * Max length = KIptvLastWatchedStartparamMaxLength = 16k
     * @since S60 v3.1
     * @return item's launch parameters
     */
    IMPORT_C const TDesC8& Parameters() const;

    /**
     * Gets the last watched item's launched application's Uid.
     * Needed only if item's launch type is EApplication.
     * @since S60 v3.1
     * @return launched application's Uid
     */
    IMPORT_C TUid AppUid() const;

    /**
     * Gets the last watched item's launched application view's Uid.
     * @since S60 v3.1
     * @return activated window server view's uid.
     */
    IMPORT_C TUid ViewUid() const;

    /**
     * Gets the last watched item's parameter uid.
     * @since S60 v3.1
     * @return parameter id
     */
    IMPORT_C TUid ParameterId() const;

    /**
     * Sets the last watched item's launch type, see TLaunchType enum.
     * Type of launching of the last watched item. Launching can be handled
     * by activating VideoCenter's local view, send mssage to self or some
     * other application, or launch any application with any given parameters.
     *
     * @since S60 v3.1
     * @return last watched item's launch type
     */
    IMPORT_C TLaunchType LaunchType() const;

    /**
     * Counts size of the externalize result. User can call this to find out
     * how much data will be received from ExternalizeL.
     * @return Size in 8-bit bytes
     */
    IMPORT_C TUint32 CountExternalizeSize() const;

    /**
     * Gets the last watched item's content type.
     * 
     * @since S60 v5.0
     * @param aContentType Item's content type.
     * @return System-wide error code.
     */
    IMPORT_C TInt SetContentType( const TInt aContentType );    
    
    /**
     * Gets the last watched item's content type.
     * 
     * @since S60 v5.0
     * @return Item's content type.
     */
    IMPORT_C TInt ContentType() const;    

    /**
     * Gets the last watched item's mpx id.
     * 
     * @since S60 v5.0
     * @param aMpxId Item's mpx id.
     * @return System-wide error code.
     */
    IMPORT_C TInt SetMpxId( const TInt aMpxId );    
    
    /**
     * Gets the last watched item's mpx id.
     * 
     * @since S60 v5.0
     * @return Item's mpx id.
     */
    IMPORT_C TInt MpxId() const; 
    
private:

    CIptvLastWatchedData();

private: // Data

    /**
     * Name that is displayed in UI for the last watched item.
     * Own.
     */
    HBufC* iName;

    /**
     * Uri that identifies the last watched item.
     * Own.
     */
    HBufC* iUri;

    /**
     * Icon's path that is shown to user for the last watched item.
     * Own.
     */
    HBufC* iIconPath;

    /**
     * Mime type for the last watched item.
     * Own.
     */
    HBufC* iMimeType;

    /**
     * Service where from this video was started.
     */
    TInt32 iServiceId;

    /**
     * Content id of started video.
     */
    TIptvContentId iContentId;

    /**
     * Content index of started video to specify correct url.
     */
    TUint32 iContentIndex;

    /**
     * Position where play was stopped when finnished watching the clip.
     */
    TUint32 iLastVideoPlayPoint;

    /**
     * Parental control age limit for this video.
     */
    TUint32 iParentalControl;

    /**
     * Launch parameter descriptor for launching the last watched item's
     * associated application.
     * Own.
     */
    HBufC8* iParameters;

    /**
     * Last watched item's associated application's Uid.
     */
    TUid iAppUid;

    /**
     * Last watched item's associated view server view's Uid.
     */
    TUid iViewUid;

    /**
     * Launch parameter id of the last watched item's associated application.
     */
    TUid iParameterId;

    /**
     * Last watched item's launch type, see TLaunchType enum. Launching can
     * be handled by activating VideoCenter's local view, send mssage to self
     * or some other application, or launch any application with any given
     * parameters.
     */
    TLaunchType iLaunchType;
    
    /**
     * Content type when last watched is storing either stream or local clip.
     * Same values are used as in Via Player Default View's iContentType.
     */
    TInt iContentType;
    
    /**
     * iMpxId.
     */
    TInt iMpxId;
    
    };

#endif // C_IPTVLASTWATCHEDDATA_H
