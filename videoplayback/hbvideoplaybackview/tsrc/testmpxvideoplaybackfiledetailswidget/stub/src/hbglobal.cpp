/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  class for HbGlobal
*
*/

// Version : %version:   1 %



#include "hbglobal.h"
#include <qstring>


QString hbTrId( QString string, int n )
{
    Q_UNUSED( n );

    QString loc = "";

    if ( string == "txt_videos_list_description" )
    {
        loc = "Description";
    }
    else if ( string == "txt_videos_list_duration" )
    {
        loc = "Duration";
    }
    else if ( string == "txt_videos_list_date" )
    {
        loc = "Date";
    }
    else if ( string == "txt_videos_list_modified" )
    {
        loc = "Modified";
    }
    else if ( string == "txt_videos_list_location" )
    {
        loc = "Location";
    }
    else if ( string == "txt_videos_list_author" )
    {
        loc = "Author";
    }
    else if ( string == "txt_videos_list_copyright" )
    {
        loc = "Copyright";
    }
    else if ( string == "txt_videos_list_language" )
    {
        loc = "Language";
    }
    else if ( string == "txt_videos_list_keywords" )
    {
        loc = "Keywords";
    }
    else if ( string == "txt_videos_list_resolution" )
    {
        loc = "Resolution";
    }
    else if ( string == "txt_videos_list_format" )
    {
        loc = "Format";
    }
    else if ( string == "txt_videos_list_collection_name" )
    {
        loc = "Collection";
    }
    else if ( string == "txt_videos_title_videos" )
    {
        loc = "Title";
    }
    else if ( string == "txt_videos_list_file_size" )
    {
        loc = "Size";
    }
    else if ( string == "txt_videos_list_bitrate" )
    {
        loc = "Bitrate";
    }
    return loc;
}

//End of file
