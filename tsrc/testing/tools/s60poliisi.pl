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
use File::Copy;

my @files;

FindDirs(".", "", \@dirs);

my $option = "";

$option = shift(@ARGV) if( scalar(@ARGV) > 0 );

if( $option eq "-check" )
{
 	# Check for missing policy files.
 	print("Policy file missing from directories:\n");
	my $count = 0;
	foreach $dir ( @dirs )
	{
    if( !-e "$dir/distribution.policy.s60" )
    	{
    	print("$dir\n");
    	$count++;
    	}
	}
	print("None!\n") if( $count == 0 );
}
elsif( $option eq "-len" )
{
	print("Too long filenames:\n");
	foreach $dir ( @dirs )
	{
		my @files;
		GetAllFiles( $dir, \@files );
	    foreach $file ( @files )
	   	{
	    	if( length( $dir . "/" . $file ) > 140 )
	    	{
	    		print("$dir/$file\n");
	    	}
	    }
	}
}
elsif( $option eq "-list" )
{
	foreach $dir ( @dirs )
	{
    	print("$dir\n");
    	CheckDistri( $dir );
	}
}
elsif( $option eq "" )
{
	print("This copies distribution.policy.s60 file from current directory to all subdirectories.\n\n");
	print("Press CTRL-C to cancel, ENTER key to continue.\n\n");
	 <>;
	
	die("policy file does not exist in current directory!") if( ! -e "distribution.policy.s60" );
	
	foreach $dir ( @dirs )
		{
	    unlink("$dir/distribution.policy.s60");
	    copy("distribution.policy.s60", "$dir/distribution.policy.s60");
		}
}
else
{
	print("Options:\n");
	print("  -check checks all directories for policy files.\n");
	print("  -len checks that any path doesn't go over 140 characters.\n");
	exit();
}

# returns if this is internal folder
sub CheckDistri
{
	my ($dir) = @_;
	
	#
	# Check all subdirs but internals
	#
	
	my $isInternal = 0;
	
	if( substr($dir, rindex( $dir, "/" )+1 ) =~m/internal$/i )
	{
		$isInternal = 1;
	}
	return $isInternal;
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


#------------------------------------------------------------------------------------
# GetAllFiles
# Parameters:
#	$goDir, where to start finding
#	$refIncfiles, reference to array which will hold found files
#------------------------------------------------------------------------------------
sub GetAllFiles
{
	my ($godir, $refIncfiles) = @_;

	my $startDir = cwd;

	chdir($godir);

	#print("Now in: " . cwd . "\n");

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};
		
		if (! (-d $file) )
		{
		 	push @$refIncfiles, $file;
		}
	}

	chdir ($startDir);
}


#------------------------------------------------------------------------------------
# FindDirs
# Parameters:
#	$goDir, where to start finding
#	$fileSearch, filename search
#	$refIncfiles, reference to array which will hold found files
#------------------------------------------------------------------------------------
sub FindDirs
{
	my ($godir, $fileSearch, $refIncfiles) = @_;

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
		    if( $file =~ m/$fileSearch/i )
		    {
		        push @$refIncfiles, (cwd . "/" . $file);
		    }
		 	FindDirs( $file, $fileSearch, $refIncfiles );
		}
	}

	chdir ($startDir);
}
