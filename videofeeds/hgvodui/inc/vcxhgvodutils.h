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
* Description:    HG VOD utility class*
*/




#ifndef VCXHGVODUTILS_H
#define VCXHGVODUTILS_H

#include <coemain.h>
#include "vcxnscontent.h"

class CVcxNsCategory;

//CONSTS
//Max length of date string 
const TInt KVcxNsMaxDateBufSize = 32;
//Max length of time string 
const TInt KVcxNsMaxTimeBufSize = 32;
//Empty space string
_LIT( KVcxNsSpaceString, " " );
_LIT( KVcxNsDownloadProgressFormatString, "%d" );
//search category's custom id. Dont want to link this to iptvutil.h
const TUint32 KVcxNsSearchCategoryId = KMaxTUint32 - 1;
//The size of the buffer
const TUint KScrollerBufferSize = 150;
//The minimum change needed for buffer to be modified
const TUint KScrollerBufferTreshold = 30;

// CLASS DECLARATION
/**
 * Utility class for VOD UI common use
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib vcxhgvodui.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CVcxHgVodUtils ): public CBase
    {
public:

    /**
     * Screen resolutions.
     */
    enum TVcxScreenResolution
        {
        EVcxScreenResolutionUnknown = 0,
        EVcxScreenResolutionQVGA,
        EVcxScreenResolutionQHD,
        EVcxScreenResolutionVGA
        };

public:
    
// New functions
    
    /**
     * Returns current screen resolution (QVGA,QHD,VGA).
     * 
     * @param aCoeEnv Pointer to Coe Env.
     * @return Screen resolution.
     */
    static TVcxScreenResolution GetScreenResolution( CCoeEnv* aCoeEnv );    

     /**
      * Load a resource string 
      * 
      * @param aResourceId Resource id
      */
    static HBufC* LoadStringFromResourceLC( TInt aResourceId );

     /**
      * Load a resource string 
      * @param aResourceId Resource id
      * @param aDesc Descriptor to replace the variable from resource string
      */
    static HBufC* LoadStringFromResourceLC( TInt aResourceId, const TDesC& aDesc );
    
    /**
     * Load a resource string 
     * @param aResourceId Resource id
     * @param aNum Number to replace the variable from resource string
     */
    static HBufC* LoadStringFromResourceLC( TInt aResourceId, const TInt aNum );
   
   /**
     * Format the given time to a text
     */
    static void FormatDateAndTimeL( TTime& aTime, TDes& aFormattedString );
    
    /**
     * Format given size to required UI format
     * @param aLength Content length in time
     * @param aSize Content size
     * @param aFormattedLength On return, formatted string  
     */    
    static void FormatLengthAndSizeL( TUint32 aLength, 
                                      TUint32 aSize, 
                                      TDes& aFormattedString );

    /**
     * Format given download state to required UI format
     * @param aStatus Status of the content's download
     * @param aFormattedLength On return, formatted string  
     */    
    static void FormatDownloadStateL( CVcxNsContent::TVcxContentDlStatus aStatus,
                                      TInt aProgress,
                                      TDes& aFormattedString );
    
    /**
     * Format Category's name to a text
     */
    static HBufC* ResolveCategoryNameLC( CVcxNsCategory& aCategory );
    
    /**
     * Format Category's info to a text
     */
    static HBufC* ResolveCategoryInfoLC( CVcxNsCategory& aCategory );
    
    /**
     * Is the given category a search category
     */
    static TBool IsSearchCategory( CVcxNsCategory& aCategory );
    
    /**
     * Get number of email accounts defined
     *
     */
    static TBool IsDefaultSmtpAccountDefinedL();

    /**
     * Send link using SMS or Email service.
     *
     * @param aLink The link to share
     * @param aMessageTypeModule MTM service Uid
     * @param aEikonEnv Eikon env
     *
     */
    static void ShareLinkL( 
            const TDesC& aLink, 
            const TUid aMessageTypeModule,
            CEikonEnv& aEikonEnv );    

    static CGulIcon* DuplicateGulIconL( CGulIcon* aIcon );
    
private:    
    
    /**
     * Default constructor
     *
     */
    CVcxHgVodUtils( );

    /**
    * Destructor.
    */
    virtual ~CVcxHgVodUtils();

    };

#endif // VCXHGVODCATEGORYLISTIMPL_H
