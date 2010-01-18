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




#include <coemain.h>

#include "IptvDebug.h"

#include <aknViewAppUi.h> 
#include <AknWaitDialog.h>
#include <StringLoader.h> 
#include <apgwgnam.h>
#include <aknnotewrappers.h> 
#include <e32property.h>
#include "CIptvResourceLoader.h"
#include <vcxnotifier.rsg>
#include "vcxnotifier.h"

// CONSTANTS
_LIT( KvcNotifierResFile, "\\Resource\\Plugins\\vcxnotifier.");

//
// ----------------------------------------------------------
// CVcXNotifier::NewL()
// ----------------------------------------------------------
//
MEikSrvNotifierBase2* CVcXNotifier::NewL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodNotifier::NewL");

    CVcXNotifier* self = new(ELeave) CVcXNotifier(); 
    return self;
    }



//
// ----------------------------------------------------------
//  CVcXNotifier::CVcXNotifier()
// ----------------------------------------------------------
//
CVcXNotifier::CVcXNotifier()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::CVcXNotifier");  
    }

//
// ----------------------------------------------------------
// CVcXNotifier::~CVcXNotifier()
// ----------------------------------------------------------
//
CVcXNotifier::~CVcXNotifier()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::~CVcXNotifier");        

    if ( iWaitDialog )
        {
        iWaitDialog->SetCallback( NULL );
        TRAPD( err, iWaitDialog->ProcessFinishedL() );
        if( err != KErrNone )
            {
            delete iWaitDialog;
            }
        iWaitDialog = NULL;
        }
    
    delete iResourceLoader;

    }

//
// ----------------------------------------------------------
// CVcXNotifier::Release()
// ----------------------------------------------------------
//
void CVcXNotifier::Release()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::Release");
    delete this;
    }

//
// ----------------------------------------------------------
// CVcXNotifier::RegisterL()
// ----------------------------------------------------------
//
MEikSrvNotifierBase2::TNotifierInfo CVcXNotifier::RegisterL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodNotifier::RegisterL");
        
    iInfo.iUid = KVcxNotifierImplUid;        
    iInfo.iChannel = KVcxNotifierImplUid;
    iInfo.iPriority = MEikSrvNotifierBase2::ENotifierPriorityHigh;
    return iInfo;
    }

//
// ----------------------------------------------------------
// CVcXNotifier::Info()
// ----------------------------------------------------------
//
MEikSrvNotifierBase2::TNotifierInfo CVcXNotifier::Info() const
    {
    return iInfo;
    }

//
// ----------------------------------------------------------
// CVcXNotifier::StartL()
// ----------------------------------------------------------
//
TPtrC8 CVcXNotifier::StartL(const TDesC8& /*aBuffer*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::StartL");
    // Synch usage not allowde
    TPtrC8 ret(KNullDesC8);
    return (ret);
    }

//
// ----------------------------------------------------------
// CVcXNotifier::StartL()
// ----------------------------------------------------------
//
void CVcXNotifier::StartL(const TDesC8&  aBuffer, 
                          TInt /*aReturnVal*/, 
                          const RMessagePtr2& aMessage )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::StartL");
    TPckg<TVcxNotifierParams> params( iParams );
    params.Copy( aBuffer );
    if( !iAsyncResp.IsNull() )
        {
        // "gracefully" close waiting active objects
        iAsyncResp.Complete( KErrNone );
        }
    
    if( !iResourceLoader )
        {
        LoadResourcesL();
        }
    
    switch (iParams.iEvent)
         {
         case TVcxNotifierParams::EConnecting:
             // New completed download, get new counter value
             iAsyncResp = aMessage;
             DisplayWaitNoteL();
             // message will be completed after 
             // dialog have been dismissed or
             // when close request is received from client
             break;        
         
         case TVcxNotifierParams::EConnected: 
             CloseWaitNote( );
             aMessage.Complete( KErrNone );
             break;
         default:
             break;
         }     
    }
    
//
// ----------------------------------------------------------
//  CVcXNotifier::Cancel()
// ----------------------------------------------------------
//
void CVcXNotifier::Cancel()
    {
    if( !iAsyncResp.IsNull() )
        {
        iAsyncResp.Complete( KErrCancel );
        }
    CloseWaitNote();
    
    delete iResourceLoader;
    iResourceLoader = NULL;
    }

//
// ----------------------------------------------------------
//  CVcXNotifier::UpdateL()
// ----------------------------------------------------------
//
TPtrC8 CVcXNotifier::UpdateL(const TDesC8& /*aBuffer*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::UpdateL");

    return TPtrC8();
    }

//
// ----------------------------------------------------------
// CVcXNotifier::NotifierCapabilites()
// ----------------------------------------------------------
//
TInt CVcXNotifier::NotifierCapabilites()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::NotifierCapabilites");

    return EScreenDeviceChangeSupported;
    }

//
// ----------------------------------------------------------
// CVcXNotifier::DialogDismissedL(
// ----------------------------------------------------------
//
void CVcXNotifier::DialogDismissedL( TInt /* aButtonId */ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::DialogDismissedL");
    if( !iAsyncResp.IsNull() )
        {
        iAsyncResp.Complete( KErrCancel );
        }
    }

//
// ----------------------------------------------------------
// CVcXNotifier::DisplayWaitNoteL
// ----------------------------------------------------------
//
void CVcXNotifier::DisplayWaitNoteL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::DisplayWaitNoteL");
    
    CloseWaitNote();
    
    if( iResourceLoader )
        {
        iWaitDialog = new (ELeave) CAknWaitDialog( 
                                 (REINTERPRET_CAST( CEikDialog**, &iWaitDialog )),
                                 ETrue );
        HBufC* text(NULL);
        if( iParams.iMsg.Length() )
           {
           text = StringLoader::LoadLC( R_IPTV_CON_UTIL_CONNECTING_VIA_NOTE, iParams.iMsg );
           }
       else
           {
           text = StringLoader::LoadLC( R_IPTV_CON_UTIL_CONNECTING_NOTE );
           }
        iWaitDialog->SetCallback( this );
        iWaitDialog->SetTextL( *text );
        CleanupStack::PopAndDestroy( text );
        iWaitDialog->ExecuteLD( R_IPTV_CON_UTIL_COMMON_WAIT_NOTE );
        }
    }

//
// ----------------------------------------------------------
// CVcXNotifier::CloseWaitNote
// ----------------------------------------------------------
//
void CVcXNotifier::CloseWaitNote()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::CloseWaitNote");
    if ( iWaitDialog )
        {
        TRAPD(error, iWaitDialog->ProcessFinishedL());
        if ( error != KErrNone )
            {
            delete iWaitDialog;
            }
        iWaitDialog = NULL;
        }    
    }

//
// ----------------------------------------------------------
// CVcXNotifier::LoadResourcesL
// ----------------------------------------------------------
//
void CVcXNotifier::LoadResourcesL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcXNotifier::LoadResourcesL");

    CCoeEnv* env = CCoeEnv::Static();
    if( env )
        {
        iResourceLoader = CIptvResourceLoader::NewL( *env );   
        if (iResourceLoader)
            {
            iResourceLoader->AddResourceL( KvcNotifierResFile );
            }
        else
            {
            User::Leave( KErrGeneral );
            }
        } 
    }
// end of file
