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

my $option = "";
$option = shift(@ARGV) if( scalar(@ARGV) > 0 );

if( $option =~ m/\-h/i )
{
    print(
    "tsrcpkgtool.pl\n" . 
    " Toggles comments in all pkgs under tsrc folders for lines \n" .
    " which have testframework.ini or cccccc00.cre cenrep.\n" . 
    " Options:\n" . 
    "  -h help\n  -r remove only\n  -a add only\n" );    
    exit;
} 

my @files;
FindFiles(".", ".pkg", 1, \@files);
FindFiles(".", ".inf", 1, \@files);

my $comment = ";TEMPCOMMENT ";
my $comment_inf = "//TEMPCOMMENT ";


foreach my $file ( @files )
{
    if( $file =~ m/\/tsrc\//i )
    {
        open(FILE_HANDLE, $file) or die ("Could not read file '$file'\n");
        my @lines = <FILE_HANDLE>;
	    close(FILE_HANDLE);
	    
	    open(FILE_HANDLE, ">$file") or die ("Could not write file '$file'\n");
	    foreach my $line ( @lines )
	    {
	        if( $line =~ /\.\.\/init\/testframework\.ini/i or $line =~ /\/cccccc00\.cre/i or
	            $line =~ /\.\.\\init\\testframework\.ini/i or $line =~ /\\cccccc00\.cre/i )
	        {
	            my $filename = $file;
	            $filename =~ s/\//\\/g;
	            $filename = substr( $filename, rindex( $filename, "\\" )+1 );
	            	            
	            if( $line =~ /$comment/ )
	            {
	                if( $option ne "-a" )
	                {
	                    # Remove comment.
	                    $line = substr( $line, length( $comment ) );
	                    print("Removed comment: $filename.\n");
	                }
	            }
	            elsif( $line =~ /$comment_inf/ )
	            {
	                if( $option ne "-a" )
	                {
	                    # Remove comment.
	                    $line = substr( $line, length( $comment_inf ) );
	                    print("Removed comment: $filename.\n");
	                }
	            }
	            else
	            {
	                if( $option ne "-r" )
	                {
    	                # Add comment.
    	                if( $filename =~ /\.inf/ ) {
												$line = $comment_inf . $line;
											}
											else {
												$line = $comment . $line;
											}
	                    print("Added comment: $filename.\n");
	                }
	            }
	        }
	        print FILE_HANDLE $line;
	    }	    
	    close(FILE_HANDLE);
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
	my ($godir, $fileSearch, $searchType, $refIncfiles) = @_;

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
		 	FindFiles( $file, $fileSearch, $searchType, $refIncfiles);
		} else
		{
			if( ($file =~ m/$fileSearch/i and $searchType == 0 ) or ($file =~ m/$fileSearch$/i and $searchType == 1 ) )
			{
                $file = cwd . "/" . $file;
				push @$refIncfiles, $file;
				#print("$file\n");
			}
		}
	}

	chdir ($startDir);
}
