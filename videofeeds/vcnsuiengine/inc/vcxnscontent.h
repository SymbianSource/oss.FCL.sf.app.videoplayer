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




#ifndef C_VCXNSVODCONTENT_H
#define C_VCXNSVODCONTENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <vcxmyvideosdefs.h>

// FORWARD DECLARATIONS
class CMPXMedia;
class CVcxNsContentAccess;

// CLASS DECLARATION

/**
 *  CVcxNsContent
 * 
 * @lib vcxnsuiengine.lib
 */
class CVcxNsContent : public CBase
    {

public:

    /**
     * TVcxContentDlStatus
     * Typedef that links the content dl status to mpx dl status
     */ 
    typedef enum
        {
        EVcxContentDlStateNone         =   EVcxMyVideosDlStateNone,
        EVcxContentDlStateDownloading  =   EVcxMyVideosDlStateDownloading,
        EVcxContentDlStateFailed       =   EVcxMyVideosDlStateFailed,
        EVcxContentDlStatePaused       =   EVcxMyVideosDlStatePaused,
        EVcxContentDlStateDownloaded   =   EVcxMyVideosDlStateDownloaded,
        EVcxContentDlStateNotFound     =   1000,  // Mpx item not found
        EVcxContentDlStateNotDownloadable,        // Content has no downloadable media content
        EVcxContentDlStateCanceling // Dl cancel command has been sent to collection, dl start cmd is not offered for user
        }  TVcxContentDlStatus;

    /**
     * TVcxNsContentAccessType
     * 
     * Defines the mime / handling type of given content media acess.
     */ 
    typedef enum
        {
        EVcxContentTypeNoType          = 0x00,
        EVcxContentTypeVideo           = 0x01,
        EVcxContentTypeStream          = 0x02,
        EVcxContentTypePreviewVideo    = 0x04,
        EVcxContentTypePreviewStream   = 0x08
        } TVcxNsContentAccessType;

    /**
     * TVcxNsMpxContentCommand
     */    
    typedef enum 
        {
        EVcxNsMpxContentCommandNone = 0,
        EVcxNsMpxContentCommandStartDownload,
        EVcxNsMpxContentCommandCancelDownload,
        EVcxNsMpxContentCommandPauseDownload,
        EVcxNsMpxContentCommandResumeDownload,
        EVcxNsMpxContentCommandWaitingForMpxItem
        } TVcxNsMpxContentCommand;

    static CVcxNsContent* NewL();

	/**
	 * Copy ctor
	 *
	 * @param aContent Content to be made a copy
	 */
    static CVcxNsContent* NewL( CVcxNsContent& aContent );

    virtual ~CVcxNsContent();

    /**
     * GetName
     * @return Name
     */
    IMPORT_C const TDesC& GetName();

    /**
     * GetLanguage
     * @return Language
     */
    IMPORT_C const TDesC& GetLanguage();
    
    /**
     * GetDescription
     * @return Description
     */
    IMPORT_C const TDesC& GetDescription();
    
    /**
     * GetAuthor
     * @return Author
     */
    IMPORT_C const TDesC& GetAuthor();  
  
    /**
     * GetCopyright
     * @return GetCopyright
     */
    IMPORT_C const TDesC& GetCopyright();  
  
     /**
     * GetIconPath
     * @return IconPath
     */
    IMPORT_C const TDesC& GetIconPath();
    
    /**
     * GetServiceId
     * @return ServiceId
     */
    IMPORT_C TUint32 GetServiceId();
     
    /**
     * GetContentId
     * @return ContentId
     */
    IMPORT_C TUint32 GetContentId();
    
    /**
     * Get size
     * @return Size
     */
    IMPORT_C TUint32 GetSize();
    
    /**
     * Get length in time
     * @return Length
     */
    IMPORT_C TUint32 GetLength();
    
    /**
     * Get browser url
     * @return Browser url
     */
    IMPORT_C const TDesC& GetBrowserUrl();
    
    /**
     * Get DlStatus
     * @return Dl Status
     */
    IMPORT_C TVcxContentDlStatus GetDlStatus();
    
    /**
     * Get DlProgress in time
     * @return Dl Progress
     */
    IMPORT_C TInt8 GetDlProgress();    

    /**
     * Get content access types found from this content.
     * Content type flag values are masked to return value.
     * @return  Content types
     */
    IMPORT_C TUint32 GetContentTypes();
   
    /**
     * GetUrl
     * @return Url
     */
    IMPORT_C const TDesC& GetUrl( TVcxNsContentAccessType aContentType );
    
    /**
     * Get age profile for this content.
     * @return Age profile.
     */
    IMPORT_C TUint16 GetAgeProfile();

    /**
     * Set user name
     * @param aUsername
     */
    IMPORT_C void SetUsernameL( const TDesC& aUsername );
    
    /**
     * Set password
     * @param aPassword
     */
    IMPORT_C void SetPasswordL( const TDesC& aPassword );
    
    /**
     * Get user name
     * @return Username
     */
    const TDesC& GetUsername();
    
    /**
     * Get password
     * @return Password
     */
    const TDesC& GetPassword();
    
    /**
     * Get local filepath
     * @param aType Type of the content access, which path to get
     * @return path
     */
    const TDesC& GetLocalFilepath( TVcxNsContentAccessType aType );
    
    /**
     * Get Last Playback Position
     * 
     * @param aType Type of the content access, which position to get
     * @return Last playback position
     */
    TInt32 GetLastPlaybackPosition( TVcxNsContentAccessType aType );
    
    /**
     * Set Last Playback Position
     * @param aPos Last playback position
     * @param aType Type of the content access, which position to get
     */
    void SetLastPlaybackPosition( const TReal32 aPos, TVcxNsContentAccessType aType );

    /**
     * SetNameL
     * @param aName Name
     */
    void SetNameL( const TDesC& aName );
    
    /**
     * SetLanguageL
     * @param aLanguage Language
     */
    void SetLanguageL( const TDesC& aLanguage );
    
    /**
     * SetDescriptionL
     * @param aDescription Description
     */
    void SetDescriptionL( const TDesC& aDescription );
    
    /**
     * SetAuthorL
     * @param aAuthor Author
     */
    void SetAuthorL( const TDesC& aAuthor );    

    /**
     * SetCopyrightL
     * @param aCopyright Copyright
     */
    void SetCopyrightL( const TDesC& aCopyright );
    
    /**
     * SetIconPathL
     * @param aPath Path
     */
    void SetIconPathL( const TDesC& aPath );

    /**
     * SetServiceId
     * @param aServiceId ServiceId
     */
    void SetServiceId( const TUint32 aServiceId );
    
    /**
     * SetContentId
     * @param aContentId ContentId
     */
    void SetContentId( const TUint32 aContentId );

    /**
     * Set size
     * @param aSize Size
     */
    void SetSize( const TUint32 aSize );
    
    /**
     * Set length
     * @param aLength Length
     */
    void SetLength( const TUint32 aLength );

    /**
     * Set browser url
     * @param aBrowserUrl Browser Url
     */
    void SetBrowserUrlL( const TDesC& aBrowserUrl );

    /**
     * Set age profile.
     * @param aAgeProfile Age profile.
     */
    void SetAgeProfile( const TUint16 aAgeProfile );

    /**
     * Set mpx media
     * @param aMpxMedia Mpx Media object
     * @param aType Content access type
     */
    void SetMpxMediaL( CMPXMedia* aMpxMedia,
                       TVcxNsContentAccessType aType = EVcxContentTypeVideo );
    
    /**
     * Get mpx media
     * @param aType Content access type
     * @return Media object
     */
    CMPXMedia* GetMpxMedia( TVcxNsContentAccessType aType = EVcxContentTypeVideo );
    
    /**
     * Get mpx id
     * @param aType Content access type
     * @return 
     */    
    TUint32 GetMpxId( TVcxNsContentAccessType aType = EVcxContentTypeVideo );
    
     /**
      * Set mpx id
      * @param aMpxId Mpx Media id
      * @param aType Content access type
      */   
    void SetMpxId( const TUint32 aMpxId, TVcxNsContentAccessType aType = EVcxContentTypeVideo );

    /**
     * Adds content access. Transfers ownership to this class.
     * @param aContentAccess Media content access
     */    
    void AddContentAccess( CVcxNsContentAccess* aContentAccess );

     /**
      * Get media content.
      * @param aType Content access type
      * @return Media content access
      */   
    CVcxNsContentAccess* GetContentAccess( TVcxNsContentAccessType aType );

    /**
     * Pending collection command. Cmd has been sent, but not yet processed.
     * see TVcxNsMpxContentCommand
     * @param aCmd Issued command
     */
    void SetPendingCommand( TVcxNsMpxContentCommand aCmd );

    /**
     * Pending collection command. Cmd has been sent, but not yet processed.
     * see TVcxNsMpxContentCommand
     * @return Pending command
     */
    TVcxNsMpxContentCommand PendingCommand();

    /**
     * Pending collection command's transaction id.
     * Needed to identify when the command is completed.
     * @param aTrId Current command's transaction id
     */
    void SetCmdTransactionId( TUint32 aTrId );

    /**
     * Pending collection command's transaction id.
     * Needed to identify when the command is completed.
     * @return Current command's transaction id
     */
    TUint32 CmdTransactionId();

    /**
     * DlFailNotified
     * @return Is the dl failure notified to ui engine observers.
     */
    TBool DlFailNotified();
    
    /**
     * SetDlFailNotified
     * @param aDlFailNotified Is the dl fail notified.
     */
    void SetDlFailNotified( TBool aDlFailNotified );

    /**
     * FullDetailsFetched
     * @return Has the full details been fetched to the content item.
     */
    TBool FullDetailsFetched();
    
    /**
     * SetFullDetailsFetched
     * @param aFullDetailsFetched Is the full details fetched.
     */
    void SetFullDetailsFetched( TBool aFullDetailsFetched );
    
private:

    CVcxNsContent( );
    
// Data

    /**
     * Content name
     * Own.
     */
    HBufC* iName;

    /**
     * iLanguage.
     * Own.
     */
    HBufC* iLanguage;
    
    /**
     * iDescription.
     * Own.
     */
    HBufC* iDescription;
    
    /**
     * iAuthor.
     * Own.
     */
    HBufC* iAuthor;
    
    /**
     * iCopyright.
     * Own.
     */
    HBufC* iCopyright;
    
    /**
     * Icon path
     * Own.
     */
    HBufC* iIconPath;

    /**
     * Service Id
     */
    TUint32 iServiceId;
    
    /**
     * Content Id
     */
    TUint32 iContentId;
    
    /**
     * Size
     */
    TUint32 iSize; 
    
    /**        
    * Length in time
    */
    TUint32 iLength;
    
    /**        
    * Browser url
    * Own.
    */
    HBufC* iBrowserUrl;
    
    /**        
    * Video age profile.
    */
    TUint16 iAgeProfile;
    
    /**        
    * Username
    * Own.
    */
    HBufC* iUserName;
    
    /**        
    * Password
    * Own.
    */
    HBufC* iPasswd;
    
    /**        
    * Content acess array.
    */
    RPointerArray<CVcxNsContentAccess> iContentAccesses;

    /**
     * Pending collection command. Cmd has been sent, but not yet processed.
     * 
     * see typedef TVcxNsMpxContentCommand
     */
    TVcxNsMpxContentCommand iPendingCommand;

    /**
     * Pending collection command's transaction id.
     * 
     * Needed to identify when the command is completed.
     */
    TUint32 iCmdTransactionId;

    /**
     * Is error note show to user.
     */
    TBool iDlFailNotified;

    /**
     * Has the full details been fetched to the content item.
     */
    TBool iFullDetailsFetched;
    
    };

#endif // C_VCXNSVODCONTENT_H
