#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

my $argcount = scalar(@ARGV);

if(scalar(@ARGV) > 0)
{
    ShowHelp();
    exit();
}

print("This script deletes mobilecrashes and result directory from ./Selge.\nPress CTRL-C to quit, enter to continue.\n");
$input = <>;
exit if($input eq "q");

my @dirs;
my $startDir = cwd;

FindFiles(".", 1, "", \@dirs, "case_*");

foreach my $dir(@dirs)
{
    $dir =~ s/\//\\/g; #whitespaces in the end
    system("del selge\\mobilecrash_*.bin\n");

    print("copy $dir\\mobilecrash_*.bin selge\\\n");
    system("copy $dir\\mobilecrash_*.bin selge\\");

    print("cd selge\n");
    chdir("selge");

    print("selge.exe -m\n");
    system("selge.exe -m");

    print("copy results\\ $dir\\\n");
    system("copy results\\ $dir\\");


    print("del mobilecrash_*.bin\n");
    system("del mobilecrash_*.bin");

    print("rmdir /s /q results\\\n");
    system("rmdir /s /q results\\");

    chdir("..");
}

chdir($startDir);

exit();

#------------------------------------------------------------------------------------
# ShowHelp
#------------------------------------------------------------------------------------
sub ShowHelp {

print <<USAGE_EOF;

selgesubdirs.pl

Copies the mobilecrashes from sub directories into Selge directory and runs Selge there.
Result files are copied back to the sub directory.

Only directories starting with case_ are processed. Selge and case_* must exist in the current directory.

USAGE_EOF

	exit();

};

#------------------------------------------------------------------------------------
# RemoveWhiteSpaces
#
# Parameters:
# 	$text
#------------------------------------------------------------------------------------
sub RemoveWhiteSpaces()
{
	my ($text) = @_;
	${$text} =~ s/\s+$//; #whitespaces in the end
	${$text} =~ s/^\s+//; #whitespaces at the start
}

#------------------------------------------------------------------------------------
# FindFiles
# Parameters:
#	$goDir, where to start finding
#   $fileType 0 = files, 1 = directories
#	$fileSearch, filename search
#	$refIncfiles, reference to array which will hold found files
#------------------------------------------------------------------------------------
sub FindFiles
{
	my ($godir, $fileType, $fileSearch, $refIncfiles) = @_;

	my $startDir = cwd;

	chdir($godir);

	#print("Now in: " . cwd . "\n");

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	foreach my $file(@filelist)
	{

		if($file eq "." or $file eq "..") {next};

		if($file =~ m/$fileSearch/i)
		{

    		if (!(-d $file) and $fileType == 0)
    		{
				push @$refIncfiles, ( cwd . "/" . $file );
    		}
    		elsif (-d $file and $fileType == 1)
    		{
				push @$refIncfiles, ( cwd . "/" . $file );
    		}
        }
	}

	chdir ($startDir);
}

