/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef XQSERVICEUTILXTRA_H_
#define XQSERVICEUTILXTRA_H_

#include <QObject>

class XQServiceUtilXtra;

class XQServiceUtilXtra : public QObject
{
    Q_OBJECT

public:

    static XQServiceUtilXtra *instance();

    void decreaseReferenceCount();

    bool currentService();

    void setCurrentService(bool aservice);

private:

    XQServiceUtilXtra();
    
    static XQServiceUtilXtra* mInstance;
 
    virtual ~XQServiceUtilXtra();


    Q_DISABLE_COPY(XQServiceUtilXtra)

public:

    /**
     * Reference count.
     */
    static int mReferenceCount;

    static bool service;

};
#endif
