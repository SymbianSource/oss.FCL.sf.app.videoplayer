#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

my $utils_PL_Included = 1; 

#------------------------------------------------------------------------------------
# GetPathFileName
#
# Parameters:
# 	$str
#------------------------------------------------------------------------------------
sub GetPathFileName
{
    my ($str) = @_;

    my $startPos = rindex($str, "\\");
    if($startPos == -1)
    {
        $startPos = rindex($str, "/");
        return $str if($startPos == -1);
    }

    my $filename = substr($str, $startPos+1);

    return $filename;
}

#------------------------------------------------------------------------------------
# GetPathDir
#
# Parameters:
# 	$str
#------------------------------------------------------------------------------------
sub GetPathDir
{
    my ($str) = @_;

    my $startPos = rindex($str, "\\");
    if($startPos == -1)
    {
        $startPos = rindex($str, "/");
        return $str if($startPos == -1);
    }

    my $filename = substr($str, 0, $startPos+1);

    return $filename;
}

#------------------------------------------------------------------------------------
# RemoveWhiteSpaces
#
# Parameters:
# 	$text
#------------------------------------------------------------------------------------
sub RemoveWhiteSpaces
{
	my ($text) = @_;
	${$text} =~ s/\s+$//; #whitespaces in the end
	${$text} =~ s/^\s+//; #whitespaces at the start
}

#------------------------------------------------------------------------------------
# XmlReadyText
#
# Parameters:
# 	$text
#------------------------------------------------------------------------------------
sub XmlReadyText
{
	my ($text) = @_;

	my $txtlt = "&lt;";
	my $txtgt = "&gt;";
	${$text} =~ s/</$txtlt/g;
	${$text} =~ s/>/$txtgt/g;

	${$text} =~ s/Ä/&Auml/g;
	${$text} =~ s/ä/&auml/g;
	${$text} =~ s/Ö/&Ouml/g;
	${$text} =~ s/ö/&ouml/g;
}

#------------------------------------------------------------------------------------
# XmlToText
#
# Parameters:
# 	$text
#------------------------------------------------------------------------------------
sub XmlToText
{
	my ($text) = @_;

	$txtlt = "&lt;";
	$txtgt = "&gt;";
	${$text} =~ s/$txtlt/</g;
	${$text} =~ s/$txtgt/>/g;

	${$text} =~ s/&Auml/Ä/g;
	${$text} =~ s/&auml/ä/g;
	${$text} =~ s/&Ouml/Ö/g;
	${$text} =~ s/&ouml/ö/g;
}