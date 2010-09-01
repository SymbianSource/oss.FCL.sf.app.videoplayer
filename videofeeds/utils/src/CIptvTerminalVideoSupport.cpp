/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include <e32base.h>
#include "IptvDebug.h"
#include <barsread.h>
#include <barsc.h>
#include <bautils.h>
#include <iptvmimesupport.rsg>
#include "CIptvMediaContent.h"
#include "CIptvTerminalVideoSupport.h"
#include "TIptvMimeType.h"

// CONSTANTS
_LIT(KIptvMimeResourceFile, "\\Resource\\Apps\\Iptvmimesupport.RSC");
_LIT(KIptvMimeRootC, "C:");
_LIT(KIptvMimeRootE, "E:");
_LIT(KIptvMimeRootF, "F:");
_LIT(KIptvMimeRootZ, "Z:");
const TInt KIptvMaxDrives = 4;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvTerminalVideoSupport::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvTerminalVideoSupport::ConstructL()
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvTerminalVideoSupport::ConstructL");

    LoadResourceL();    
    }

// ---------------------------------------------------------
// CIptvTerminalVideoSupport::LoadResourceL
// 
// ---------------------------------------------------------
//
void CIptvTerminalVideoSupport::LoadResourceL()
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvTerminalVideoSupport::LoadResourceL");

    RFs fs;
    User::LeaveIfError(fs.Connect());
    RResourceFile file;
    TFileName filename;
    TBool open = EFalse;

    for (TInt i = 0; i < KIptvMaxDrives; i++)
        {
        switch (i)
            {
            case 0:
                {
                filename.Copy(KIptvMimeRootC);
                filename.Append(KIptvMimeResourceFile);
                }
                break;
            case 1:
                {
                filename.Copy(KIptvMimeRootE);
                filename.Append(KIptvMimeResourceFile);
                }
                break;
            case 2:
                {
                filename.Copy(KIptvMimeRootF);
                filename.Append(KIptvMimeResourceFile);
                }
                break;
            case 3:
                {
                filename.Copy(KIptvMimeRootZ);
                filename.Append(KIptvMimeResourceFile);
                }
                break;
            default:
                {
                }
                break;
            } 

        TRAPD(error, file.OpenL(fs, filename));
        if (error == KErrNone)
            {
            open = ETrue;
            break;
            }
        }
        
    if (open)
        {    
        CleanupClosePushL(file);    
        file.ConfirmSignatureL();
        HBufC8* buffer = file.AllocReadLC(R_SUP_MIME_TYPE_LIST);
        LoadMimeTypesL(buffer);
        CleanupStack::PopAndDestroy(); // buffer
        CleanupStack::PopAndDestroy(); // file     
        }
    fs.Close();
    }

// ---------------------------------------------------------
// CIptvTerminalVideoSupport::LoadMimeTypesL
// 
// ---------------------------------------------------------
//
void CIptvTerminalVideoSupport::LoadMimeTypesL(const HBufC8* aBuffer)
    {    
    IPTVLOGSTRING_LOW_LEVEL("CIptvTerminalVideoSupport::LoadMimeTypesL");
                        
    if (aBuffer)
        {                
        TResourceReader reader;
        reader.SetBuffer(aBuffer);

        TUint32 version = reader.ReadUint32();
        TInt count = reader.ReadInt16();
                    
        for (TInt i = 0; i < count; i++)
            {
            TIptvMimeType type;
            HBufC* mimeType = reader.ReadHBufCL();
            CleanupStack::PushL(mimeType);
            
            if (mimeType)
                {
                TInt length = mimeType->Length();                
                TInt dataToCopy = Min(length, KIptvCAMimeTypeLength);                
                type.iMimeType.Copy(mimeType->Mid(0, dataToCopy));
                }

            type.iMaxBitrate = reader.ReadUint32();
            type.iMaxScreensizeH = reader.ReadUint32();
            type.iMaxScreensizeV = reader.ReadUint32();
            type.iMaxFramerate = reader.ReadUint32();

            iMimeTypes.AppendL(type);
            CleanupStack::PopAndDestroy(mimeType);             
            }                    
        }    
    }

// ---------------------------------------------------------
// CIptvTerminalVideoSupport::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CIptvTerminalVideoSupport* CIptvTerminalVideoSupport::NewL()
    {
    CIptvTerminalVideoSupport* self = new(ELeave) CIptvTerminalVideoSupport();
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvTerminalVideoSupport::~CIptvTerminalVideoSupport
// Destructor
// -----------------------------------------------------------------------------
//
CIptvTerminalVideoSupport::~CIptvTerminalVideoSupport()
    {        
    iMimeTypes.Close();
    }

// ---------------------------------------------------------
// CIptvTerminalVideoSupport::CIptvTerminalVideoSupport
// C++ default constructor
// ---------------------------------------------------------
//
CIptvTerminalVideoSupport::CIptvTerminalVideoSupport()     
    {
    }

// ---------------------------------------------------------
// CIptvTerminalVideoSupport::SelectMimeTypeL
// 
// ---------------------------------------------------------
//
EXPORT_C TInt CIptvTerminalVideoSupport::SelectMimeTypeL(RPointerArray<CIptvMediaContent>& aMediaContents)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvTerminalVideoSupport::SelectMimeTypeL");
        
    CIptvMediaContent* bestContent = NULL;
    TInt priority = 0;
    TInt index = 0;                

    TInt count = aMediaContents.Count();
    for (TInt i = 0; i < count; i++)
        {        
        TInt mimeTypeCount = iMimeTypes.Count();
        for (TInt j = 0; j < mimeTypeCount; j++)
            {
            // Compare is current media content's mime-type supported.
            // Mime-types are in priorized order in iMimeTypes array, best is first.
            if (aMediaContents[i]->iMimeType.Compare(iMimeTypes[j].iMimeType) == 0)
                {
                // No best content yet. No comparison needed to previous best content.
                if (!bestContent)
                    {
                    if (aMediaContents[i]->iBitrate <= iMimeTypes[j].iMaxBitrate &&
                        aMediaContents[i]->iHeight <= iMimeTypes[j].iMaxScreensizeV &&
                        aMediaContents[i]->iWidth <= iMimeTypes[j].iMaxScreensizeH &&
                        aMediaContents[i]->iFramerate <= iMimeTypes[j].iMaxFramerate)
                        {
                        index = i;
                        bestContent = aMediaContents[i];
                        IPTVLOGSTRING_LOW_LEVEL("");
                        IPTVLOGSTRING_LOW_LEVEL("First best content selected!:");
                        IPTVLOGSTRING_LOW_LEVEL("");
                        bestContent->PrintL();
                        priority = j;
                        break;
                        }
                    }
                else
                    {
                    // We already have some content as a best content. Let's compare
                    // is new content better than previous. If is -> we got new best content.
                    // Notice!! j = 0 is best priority because best mime-type is fist in array!
                    if (j < priority)
                        {
                        if (aMediaContents[i]->iBitrate <= iMimeTypes[j].iMaxBitrate &&
                            aMediaContents[i]->iHeight <= iMimeTypes[j].iMaxScreensizeV &&
                            aMediaContents[i]->iWidth <= iMimeTypes[j].iMaxScreensizeH &&
                            aMediaContents[i]->iFramerate <= iMimeTypes[j].iMaxFramerate)
                            {
                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("Best content changed from:");
                            bestContent->PrintL();                                
                            priority = j;
                            index = i;
                            bestContent = aMediaContents[i];
                            
                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("To:");
                            bestContent->PrintL();
                            }
                        }
                    // Same priority ex. same mime-type. Let's use another parameters.                                                        
                    else if (j == priority)
                        {                                                    
                        // New contents bitrate is better that previous best content's.
                        if ((aMediaContents[i]->iBitrate > bestContent->iBitrate && aMediaContents[i]->iBitrate <= iMimeTypes[j].iMaxBitrate) &&
                            aMediaContents[i]->iHeight <= iMimeTypes[j].iMaxScreensizeV &&
                            aMediaContents[i]->iWidth <= iMimeTypes[j].iMaxScreensizeH &&
                            aMediaContents[i]->iFramerate <= iMimeTypes[j].iMaxFramerate)                                                        
                            {
                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("Best content changed from:");
                            bestContent->PrintL();      

                            index = i;
                            bestContent = aMediaContents[i];
                
                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("To:");
                            bestContent->PrintL();
                            }                       
                        // New contens horizontal screensize is better than previous best content's
                        else if (aMediaContents[i]->iBitrate <= iMimeTypes[j].iMaxBitrate &&
                            (aMediaContents[i]->iHeight > bestContent->iHeight && aMediaContents[i]->iHeight <= iMimeTypes[j].iMaxScreensizeH) &&
                            aMediaContents[i]->iWidth <= iMimeTypes[j].iMaxScreensizeH &&
                            aMediaContents[i]->iFramerate <= iMimeTypes[j].iMaxFramerate)                                                        
                            {
                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("Best content changed from:");
                            bestContent->PrintL();      

                            index = i;
                            bestContent = aMediaContents[i];
    
                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("To:");
                            bestContent->PrintL();
                            }                    
                        // New contents vertical screensize is better that previous best content's
                        else if (aMediaContents[i]->iBitrate <= iMimeTypes[j].iMaxBitrate &&
                            aMediaContents[i]->iHeight <= iMimeTypes[j].iMaxScreensizeV &&
                            (aMediaContents[i]->iWidth > bestContent->iWidth && aMediaContents[i]->iWidth <= iMimeTypes[j].iMaxScreensizeV) &&
                            aMediaContents[i]->iFramerate <= iMimeTypes[j].iMaxFramerate)                                                        
                            {
                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("Best content changed from:");
                            bestContent->PrintL();      
                            index = i;
                            bestContent = aMediaContents[i];

                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("To:");
                            bestContent->PrintL();
                            }
                        // New contents framerate is better than previous best content's
                        else if (aMediaContents[i]->iBitrate <= iMimeTypes[j].iMaxBitrate &&
                            aMediaContents[i]->iHeight <= iMimeTypes[j].iMaxScreensizeV &&
                            aMediaContents[i]->iWidth <= iMimeTypes[j].iMaxScreensizeH &&
                            (aMediaContents[i]->iFramerate > bestContent->iFramerate && aMediaContents[i]->iFramerate <= iMimeTypes[j].iMaxFramerate))
                            {
                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("Best content changed from:");
                            bestContent->PrintL();      
                            index = i;
                            bestContent = aMediaContents[i];

                            IPTVLOGSTRING_LOW_LEVEL("");
                            IPTVLOGSTRING_LOW_LEVEL("To:");
                            bestContent->PrintL();
                            }                                                  
                        }
                    }
                }
            }                                             
        }
       
    if (bestContent)
        {
        return index;
        }
    else
        {
        return KErrNotFound;
        }
    }

// End of file
