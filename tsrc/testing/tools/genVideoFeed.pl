#
# Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#------------------------------------------------------------------------------------
# Includes
#------------------------------------------------------------------------------------
#use strict;
use warnings;
use Cwd; # for cwd
use FindBin; # for FindBin:Bin
use File::Path; # for mkpath
use Date::Calc;

#------------------------------------------------------------------------------------
# GLOBAL CODE
#------------------------------------------------------------------------------------


my $xmlStart = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" .
"<rss version=\"2.0\" xmlns:media=\"http://search.yahoo.com/mrss/\">\n" .
"\t<channel>\n";

my $xmlItem =
"\t<item>\n" .
"\t\t<enclosure url=\"XMLURL\" length=\"39567360\" type=\"video/3gpp\"/>\n" .
"\t\t<title>XMLTITLE</title>\n" .
"\t\t<guid>XMLURL</guid>\n" .
"\t</item>\n";

my $xmlEnd =
"\t</channel>\n" .
"</rss>\n";

my $argcount = scalar(@ARGV);
my $paramFileName = "";
my $paramOutFileName = "";

if(scalar(@ARGV) < 2)
{
    print("Usage: perl genVideoFeed.pl <infile> <outfile>\n");
    print("Infile is list of videos with url and title are separated by comma\n");
    exit();
}

while(scalar(@ARGV) >= 1)
{
	$paramFileName = shift(@ARGV);
	$paramOutFileName = shift(@ARGV);
}

# Read TXT

if( !open(FILE_HANDLE, $paramFileName) )
{
	print("ERROR! Could not open file '" . $paramFileName . "'\n");
	return 0;
}
my @array = <FILE_HANDLE>;
close(FILE_HANDLE);

# Write XML

if(!open(FILE_HANDLE, ">" . "$paramOutFileName") )
{
    print("ERROR! Could not open file for writing '" . "$paramOutFileName" . "'\n");
    return 0;
}

print FILE_HANDLE ("$xmlStart");

my $line;
$line = $array[0];
RemoveWhiteSpaces(\$line);
print FILE_HANDLE ("<title>$line</title>\n");

for(my $i=1; $i<scalar(@array); $i++)
{
    my $pos = index($array[$i], ",");
    my $url = substr( $array[$i], 0, $pos);
    my $title = substr( $array[$i], $pos+1);

    RemoveWhiteSpaces(\$url);
    RemoveWhiteSpaces(\$title);

    my $data = $xmlItem;
    XmlReadyText(\$title);
    XmlReadyText(\$url);
    $data =~ s/XMLTITLE/$title/;
    $data =~ s/XMLURL/$url/g;

    if($url ne "" and $title ne "")
    {
        print FILE_HANDLE ("$data");
    }
}

print FILE_HANDLE ("$xmlEnd");

close(FILE_HANDLE);

exit();

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

	$txtlt = "&lt;";
	$txtgt = "&gt;";
	${$text} =~ s/</$txtlt/g;
	${$text} =~ s/>/$txtgt/g;

	${$text} =~ s/Ä/&Auml/g;
	${$text} =~ s/ä/&auml/g;
	${$text} =~ s/Ö/&Ouml/g;
	${$text} =~ s/ö/&ouml/g;

	#ä &auml;

#	Ö &Ouml;

#	ö &ouml;

}
