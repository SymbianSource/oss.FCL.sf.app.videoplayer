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
* Description:   App settings handles storing and loading of application settings*
*/



#ifndef _CVCXNSSETTINGS_H_
#define _CVCXNSSETTINGS_H_

// INCLUDES
#include <e32base.h>   
#include <f32file.h>
#include <etelmm.h>


// CONSTANTS 

// FORWARD DECLARATIONS
class CRepository;


// CLASS DECLARATION

/**
* Class CVcxNsSettingsEngine handles application setting related saving and loading.
*
* @lib vcxnssettingsengine.lib
*/
class CVcxNsSettingsEngine : public CBase
    { 

    public:

        /**
         * Two-phased constructor.
         *
         * @return New CIptvAppSettings object.
         */
        IMPORT_C static CVcxNsSettingsEngine* NewL();

        /**
         * Destructor.
         */
        virtual ~CVcxNsSettingsEngine();

   
    public: // Exported methods that handle application settings
				
		
        /**
         * Get used memory from CenRep. Leaves with system-wide error code
         * on error.
         *
         * @param aUsedMemory Reference to variable where value is stored.
         */
        IMPORT_C void GetUsedMemoryL( TInt& aUsedMemory );

        /**
         * Set used memory to CenRep.
         *
         * @param aUsedMemory New value.        
         * @return ETrue if successfull, EFalse otherwise.
         */
        IMPORT_C TBool SetUsedMemory( TInt aUsedMemory );

        /**
         * Get parental control setting from central repository.
         *
         * @param aParentControl On return, parental control setting value.
         *                       0 if parental control is off, otherwise the age set. 
         * @return ETrue if successfull, EFalse otherwise.
         */
        IMPORT_C TBool GetParentControl( TInt& aParentControl );

        /**
         * Set parental control setting to central repository.
         *
         * @param aParentControl Parental control value to set.
         * @return ETrue if successfull, EFalse otherwise.
         */
        IMPORT_C TBool SetParentControl( TInt aParentControl );

        /**
         * Get VoD thumnbails from CenRep.
         *
         * @param aVodThumbnails Reference to variable where value is stored.
         * @return ETrue if successfull, EFalse otherwise.
         */
        IMPORT_C TBool GetVideoOnDemandThumbnails( TInt& aVodThumbnails );
        
        /**
         * Set VoD thumbnails to CenRep.
         *
         * @param aVodThumbnails New value.
         * @return ETrue if successfull, EFalse otherwise.
         */
        IMPORT_C TBool SetVideoOnDemandThumbnails( TInt aVodThumbnails );
        
        /**
         * Get UsedSnapId from CenRep.
         *
         * @param aUsedSnapId Reference to variable where value is stored.
         * @return ETrue if successful, EFalse otherwise.
         */
        IMPORT_C TBool GetUsedSnapId( TInt& aUsedSnapId );

        /**
         * Set UsedSnapId to CenRep.
         *
         * @param aUsedSnapId New value.
         * @return ETrue if successful, EFalse otherwise.
         */
        IMPORT_C TBool SetUsedSnapId( TInt aUsedSnapId );
        
        /**
         * Get AllowUseGprs from CenRep.
         *
         * @param aAllowUseGprs Reference to variable where value is stored.
         * @return ETrue if successful, EFalse otherwise.
         */
        IMPORT_C TBool GetAllowUseGprs( TInt& aAllowUseGprs );

        /**
         * Set AllowUseGprs to CenRep.
         *
         * @param aAllowUseGprs New value.
         * @return ETrue if successful, EFalse otherwise.
         */
        IMPORT_C TBool SetAllowUseGprs( TInt aAllowUseGprs );
        
        /**
         * Get AllowRoaming from CenRep.
         *
         * @param aAllowRoaming Reference to variable where value is stored.
         * @return ETrue if successful, EFalse otherwise.
         */
        IMPORT_C TBool GetAllowRoaming( TInt& aAllowRoaming );

        /**
         * Set AllowRoaming to CenRep.
         *
         * @param aAllowRoaming New value.
         * @return ETrue if successful, EFalse otherwise.
         */
        IMPORT_C TBool SetAllowRoaming( TInt aAllowRoaming );
 
        /**
         * Check the lock code 
         *
         * @return ETrue if the user enters the correct lock code
         */
        IMPORT_C TBool CheckLockCodeL();
  
        /**
         * Get player volume setting fron CenRep
         *
         * @param aVolume Reference to variable where value is stored.
         * @return ETrue if successful, EFalse otherwise.
         */
        IMPORT_C TBool GetPlayerVolume( TInt& aVolume );

        /**
         * Store volume setting to CenRep
         *
         * @param aVolume New value.
         * @return ETrue if successful, EFalse otherwise.
         */
        IMPORT_C TBool SetPlayerVolume( TInt aVolume );
        
    private:
         
        /**
         * C++ Constructor.
         *
         */
        CVcxNsSettingsEngine( );
        
        /**
         * EPOC constructor.
         */
        void ConstructL( );

               
        /**
         * Read integer value from CenRep.
         *
         * @param aCategory CenRep category uid of the requested key
         * @param aKey   CenRep key to read.
         * @param aValue Reference to variable where value is stored.
         * @return ETrue if successfull, EFalse otherwise.
         */
        TBool GetCenRepIntKey( TUid aCategory, TInt aKey, TInt& aValue );
        
        /**
         * Write integer value to CenRep.
         *
         * @param aCategory CenRep category uid of the requested key
         * @param aKey   CenRep key to write.
         * @param aValue New value.
         * @return ETrue if successfull, EFalse otherwise.
         */
        TBool SetCenRepIntKey(  TUid aCategory, TInt aKey, TInt aValue );
              
    private: // Data
    
        CRepository*  iCenRep;
        CRepository*  iAlrCenRep;

        TBool           iPhoneConnected;
        RTelServer		iServer;
        RMobilePhone	iPhone;

    };

#endif // _CVCXNSSETTINGS_H_

// End of File
