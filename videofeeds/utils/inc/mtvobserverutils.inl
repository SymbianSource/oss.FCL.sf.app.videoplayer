/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Helper functions and macros for handling multiple observers.*
*/




#ifndef _MTV_OBSERVER_UTILS
#define _MTV_OBSERVER_UTILS

namespace MtvObserverUtils
{
    
/**
 *  Struct for holding a bunch of observers and a semaphore for controlling observer removal.
 *
 *  @since Live TV UI v1.0
 */
template <class ObserverType, template<class> class ContainerType = RPointerArray, class PointerType = ObserverType>
struct RObserverData
    {
    // constructor for initializing the semaphore
    RObserverData() : iSemaphore(0), iShutdown(EFalse)
        {
        }
    /**
     * Initializes the container.
     * This function is currently just a placeholder, later
     * it can be used to add support for CBase-derived containers,
     * assuming that all clients call OpenL and Close when using
     * this class.
     */
    void OpenL()
        {
        }
    /**
     * Closes the container.
     */
    void Close()
        {
        iContainer.Close();
        iShutdown = ETrue;
        }
    // Container can be RPointerArray, RArray.. 
    // At the moment CBase-derived containers are not supported.
    ContainerType<ObserverType> iContainer;
    // the semaphore is used so that when it has a value greater than zero, observers
    // are not removed, but nulled in the container
    TInt iSemaphore;
    // pointer type member helps code using this object, the using code does not
    // need to know the actual observer type
    PointerType* iObserver;
    // Set to ETrue when the provider of notifications is shut down.
    TBool iShutdown;
    };

/**
 * Removes NULL observers from a container.
 * @param aExistsNull Tells whether there are any NULL observers.
 * Checking this parameter inside this function makes invoking code simpler.
 * @param aObserverData Observer data object (usually a RObserverData).
 * This data object must contain a container of observers and a semaphore.
 */
template <class ObserverDataType>
static void RemoveNullObservers(TBool aExistsNull, ObserverDataType& aObserverData)
    {
    // semaphore must always be decremented 
    --aObserverData.iSemaphore;    
    if ( !aObserverData.iSemaphore && aExistsNull )
        {
        for (TInt i = aObserverData.iContainer.Count() - 1; i >= 0; --i)
            {
            if (aObserverData.iContainer[i] == NULL)
                {
                aObserverData.iContainer.Remove(i);
                }
            }
        }
    }
    
/**
 * Macro that invokes an observer function for all registered observers.
 * @param aNotification Observer function to call.
 * @param aObserverData Observer data object (usually a RObserverData).
 * This data object must contain a container of observers and a semaphore.
 */
#define RUN_NOTIFY_LOOP(aNotification, aObserverData) \
do \
    { \
    if ( !aObserverData.iShutdown )\
        {\
        ++aObserverData.iSemaphore; \
        TBool existsNull = EFalse; \
        const TInt count = aObserverData.iContainer.Count(); \
        for ( TInt i = 0; i < count; ++i ) \
            { \
            aObserverData.iObserver = aObserverData.iContainer[ i ]; \
            if ( aObserverData.iObserver ) \
                { \
                TRAP_IGNORE( aObserverData.iObserver->aNotification ); \
                } \
            else \
                { \
                existsNull = ETrue; \
                } \
            } \
        RemoveNullObservers(existsNull, \
            aObserverData); \
        }\
    } while (EFalse) \

/**
 * Macro that invokes an observer function for all registered observers.
 * @param aNotification Observer function to call.
 * @param aObserverData Observer data object (usually a RObserverData).
 * @param aError On return, contains the possible first error code that happened.
 * This data object must contain a container of observers and a semaphore.
 */
#define RUN_NOTIFY_LOOP_L(aNotification, aObserverData, aError) \
do \
    { \
    aError = KErrNone; \
    TInt ___mtvObserverUtilsNotifyLoopError = KErrNone; \
    if ( !aObserverData.iShutdown )\
        {\
        ++aObserverData.iSemaphore; \
        TBool existsNull = EFalse; \
        const TInt count = aObserverData.iContainer.Count(); \
        for ( TInt i = 0; i < count; ++i ) \
            { \
            aObserverData.iObserver = aObserverData.iContainer[ i ]; \
            if ( aObserverData.iObserver ) \
                { \
                TRAP( ___mtvObserverUtilsNotifyLoopError, aObserverData.iObserver->aNotification ); \
                if ( aError == KErrNone ) \
                    { \
                    aError = ___mtvObserverUtilsNotifyLoopError; \
                    } \
                } \
            else \
                { \
                existsNull = ETrue; \
                } \
            } \
        RemoveNullObservers(existsNull, \
            aObserverData); \
        }\
    } while (EFalse) \

/**
 * Adds an observer to an observer data object.
 * @param aObserverData Data object to which an observer is added.
 * @param aObserver Observer to add.
 * @param aError Error code to use for Leave if adding fails.
 */
template <class ObserverType, class ContainerType>
static void AddObserverL(ContainerType& aObserverData, ObserverType& aObserver, TInt aError)
    {
    if (aObserverData.iShutdown)
        {
        return;
        }
    TInt pos = aObserverData.iContainer.Find( &aObserver );
    if (KErrNotFound != pos)
        {
        return;
        }
    TInt error = aObserverData.iContainer.Append( &aObserver );
    if (KErrNone != error)
        {
        User::Leave(aError); 
        }
    }

/**
 * Removes an observer from an observer data object.
 * @param aObserverData Data object from which an observer is removed.
 * @param aObserver Observer to remove.
 */
template <class ObserverType, class ContainerType>
static void RemoveObserver( ContainerType& aObserverData, ObserverType& aObserver )
    {
    TInt pos = aObserverData.iContainer.Find( &aObserver );
    if ( KErrNotFound == pos  )
        {
        return;
        }
    if ( !aObserverData.iSemaphore )
        {
        // NotifyEventL is not part of call stack. Just remove.
        aObserverData.iContainer.Remove( pos );
        }
    else
        {
        // Simply set to NULL. NotifyEventL will remove observers.
        aObserverData.iContainer[ pos ] = NULL;
        }
    }

} // end namespace MtvObserverUtils

#endif //_MTV_OBSERVER_UTILS
