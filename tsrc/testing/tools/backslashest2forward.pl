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

my @files;

FindFiles(".", "cpp", 1, \@files, "");
FindFiles(".", "h", 1, \@files, "");

foreach $file ( @files )
{
    my $found = 0;

    if(!open(FILE_HANDLE, "$file") )
    {
        print("ERROR! Could not open file '$file'\n");
        die;
    }

	my @array = <FILE_HANDLE>;
	close(FILE_HANDLE);

    foreach $line ( @array )
    {
        if( $line =~ m/#include/ && $line =~ m/\\/ )
        {
            print("\n\nCHANGING: $file\n");
            $found = 1;
            if(!open(FILE_HANDLE2, ">$file") )
            {
                print("ERROR! Could not open file for writing '$file'\n");
                die;
            }

            rename $file,"$file.OLD";
#            unlink $file;
        }

    }

    if( $found )
    {
        foreach $line ( @array )
        {
            if( $line =~ m/#include/ && $line =~ m/\\/ )
            {
                print(" - old: $line");
                $line =~ s/\\/\//g;
                print(" - new: $line");
            }
            print FILE_HANDLE2 ($line);
        }

        close(FILE_HANDLE2);
        $found = 0;
    }

}



#------------------------------------------------------------------------------------
# FindFiles
# Parameters:
#	$goDir, where to start finding
#	$fileSearch, filename search
#	$searchType, 0 = fullname search, 1 = filetype search
#	$refIncfiles, reference to array which will hold found files
#------------------------------------------------------------------------------------
sub FindFiles
{
	my ($godir, $fileSearch, $searchType, $refIncfiles, $fileFilter) = @_;

	my $startDir = cwd;

	chdir($godir);

	#print("Now in: " . cwd . "\n");

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

		if (-d $file)
		{
		 	FindFiles( $file, $fileSearch, $searchType, $refIncfiles, $fileFilter );
		} else
		{
			if( ($file =~ m/$fileSearch/i and $searchType == 0 and $file =~m/$fileFilter/i) or ($file =~ m/$fileSearch$/i and $searchType == 1 and $file =~m/$fileFilter/i) )
			{
                $file = cwd . "/" . $file;
				push @$refIncfiles, $file;
				#print("$file\n");
			}
		}
	}

	chdir ($startDir);
}
