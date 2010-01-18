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



#ifndef CVCXNOTIFIER_H
#define CVCXNOTIFIER_H

//  INCLUDES

#include <e32base.h>

#include <eiknotapi.h>
#include <AknProgressDialog.h>
#include <agentdialog.h> 
#include <ipvideo/tvcxconnutilnotifierparams.h>

class CCoeEnv;
class CIptvResourceLoader;
class CAknWaitDialog;

// CLASS DECLARATION
class CVcXNotifier : public CBase, 
                     public MEikSrvNotifierBase2, 
                     public MProgressDialogCallback
    {
    public:

        /**
        * Creates instance of the CVcXNotifier class.
        *
        * @return Returns the MEikSrvNotifierBase2 instance just created.
        */
        static MEikSrvNotifierBase2* NewL();

        /**
        * Destructor.
        */
        virtual ~CVcXNotifier();
          
    public: // from MEikSrvNotifierBase2

        /**
        * Notifier capabilities.
        */                                        	    
        TInt NotifierCapabilites();

        /**
        * Called when all resources allocated by notifiers should be freed.
        */
        void Release();

        /**
        * From MEikSrvNotifierBase2 - method for registering the EikSrv plugin
        *
        * @return TNotifierInfo - struct containing the plugin priorities etc.
        */
        TNotifierInfo RegisterL();

        /**
        * From MEikSrvNotifierBase2 - method for getting the notifier info
        *
        * @return TNotifierInfo - struct containing the plugin priorities etc.
        */
        TNotifierInfo Info() const;

        /**
        * From MEikSrvNotifierBase2 - method for starting the plugin. Synchronous version
        * @param aBuffer - buffer containing the parameters for the plugin, packaged in TPckg<SAknSoftNotificationParams>
        * @return TPtrC8 - return value to the client, this method will return KNullDesC
        */
        TPtrC8 StartL(const TDesC8& aBuffer);

        /**
        * From MEikSrvNotifierBase2 - method for starting the plugin. Asynchronous version.
        * @param aBuffer - buffer containing parameters for the plugin, packaged in TPckg<SAknSoftNotificationParams>
        * @param aReturnVal - return value of any type. Not used in this case.
        * @param aMessage - RMessage to notify that the execution has ended
        */
        void StartL(const TDesC8& aBuffer,TInt aReturnVal, const RMessagePtr2& aMessage);

        /**
        * From MEikSrvNotifierBase2 - method for cancelling the plugin execution.
        */
        void Cancel();

        /**
        * From MEikSrvNotifierBase2 - method for updating the plugin while execution is in progress
        * @param aBuffer - buffer containing parameters for the plugin, packaged in TPckg<SAknSoftNotificationParams>
        * @return not used, return KNullDesC
        */
        TPtrC8 UpdateL(const TDesC8& /*aBuffer*/);
            
    private:// From MProgressDialogCallback
         
        /**
         * Get's called when a dialog is dismissed.
         */
        void DialogDismissedL( TInt aButtonId ); 
        
    private:

        /**
        * Displays a wait note
        *         
        */ 
       void DisplayWaitNoteL();
       
       /**
        * closes a wait note
        *           
        */
       void CloseWaitNote();

    private:

        /**
        * C++ default constructor.
        */
        CVcXNotifier();              
        
        /**
        * Check if our application is running.
        */
        TBool IsOurApplication();

       
        /**
        * Load resources.
        */
        void LoadResourcesL();

                
    private:

        /**
        * Notifier info.
        */
        TNotifierInfo iInfo;

        /**
        * Parameters.
        */
        TVcxNotifierParams iParams;         

        /**
        * Resource loader.
        */
        CIptvResourceLoader* iResourceLoader;
    
        /**
         * Connection wait dialog
         */
        CAknWaitDialog*  iWaitDialog;  
        
        /**
         * MEssage object to be completed if user cancels dialog
         */
        RMessagePtr2 iAsyncResp;
    };

#endif // CVCXNOTIFIER_H

// end of file
