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

while(scalar(@ARGV) >= 1)
{
	my $argument = shift(@ARGV);

	if($argument eq "-release")
	{
		#$paramRelease = shift(@ARGV);
	}
	else
	{
		print "arg: $argument\n";
		Help();
	}
}

sub Help
{
    print <<USAGE_EOF;

USAGE_EOF

	exit();
}

#------------------------------------------------------------------------------------
# Main code
#------------------------------------------------------------------------------------

my $startDir = cwd;

print("here: $startDir\n");

opendir(DIR, ".");
my @dirs = sort( readdir(DIR) );
closedir(DIR);

foreach my $dir(@dirs)
{
	print("search $dir\n");
	if($dir eq "." or $dir eq "..") {next};

	if (-d $dir)
	{
	    if($file eq "." or $file eq "..") {next};

	 	my $log = FindFile($dir, "iptvlog.txt", 0);
	 	my $report = FindFile($dir, "testreport.txt", 0);
	 	my $ctcdata = FindFile($dir, "ctcdata.txt", 0);
	 	$log =~ s/\//\\/;
	 	$report =~ s/\//\\/;
	 	$ctcdata =~ s/\//\\/;

	 	print("found $log\n");
	 	print("found $report\n");
	 	if($report ne "" and $log ne "")
	 	{
            my $file = GetFilename($log);
            my $pos = rindex($file, ".");
            my $ext = substr($file, $pos);
            my $file = substr($file, 0, $pos);
            $file = $file . "_" . $dir . $ext;
            my $logdest = $startDir . "\\" . $file;
	     	$logdest =~ s/\//\\/;
	     	system("copy \"$log\" \"$logdest\"\n\n");

            $file = GetFilename($report);
            $pos = rindex($file, ".");
            $ext = substr($file, $pos);
            $file = substr($file, 0, $pos);
            $file = $file . "_" . $dir . $ext;
            my $reportdest = $startDir . "\\" . $file;
	     	$reportdest =~ s/\//\\/;
	     	system("copy \"$report\" \"$reportdest\"\n\n");

            if( $ctcdata ne "" )
            {
                $file = GetFilename($ctcdata);
                $pos = rindex($file, ".");
                $ext = substr($file, $pos);
                $file = substr($file, 0, $pos);
                $file = $file . "_" . $dir . $ext;
                my $ctcdest = $startDir . "\\" . $file;
    	     	$ctcdest =~ s/\//\\/;
    	     	system("copy \"$ctcdata\" \"$ctcdest\"\n\n");
            }
	 	}
	}
}

chdir ($startDir);

exit();


#------------------------------------------------------------------------------------
# FindFiles
# Parameters:
#	$goDir, where to start finding
#	$fileSearch, filename search
#	$searchType, 0 = fullname search, 1 = filetype search
#	$refFiles, reference to array which will hold found files
#------------------------------------------------------------------------------------
sub FindFiles
{
	my ($godir, $fileSearch, $searchType, $refFiles) = @_;

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
		 	FindFiles( $file, $fileSearch, $searchType, $refFiles );
		} else
		{
			if( ($file =~ m/$fileSearch/i and $searchType == 0) or ($file =~ m/$fileSearch$/i and $searchType == 1) )
			{
                $file = cwd . "/" . $file;
				push @$refFiles, $file;
				print("$file\n");
			}
		}
	}

	chdir ($startDir);
}



#------------------------------------------------------------------------------------
# FindDirectory
#
# Parameters:
#	$goDir, where to start finding
#	$fileSearch, filename search
#	$minSize, minimum size for the directory
#
# Returns the directory found.
#------------------------------------------------------------------------------------
sub FindDirectory
{
	my ($godir, $fileSearch, $dontCheckSubFolders, $minSize) = @_;

	my $startDir = cwd;

    #print("Now in $startDir\n");
    print("minsize: $minSize for $fileSearch\n") if($minSize);

	chdir($godir);

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

		if (-d $file)
		{
			print("($file - $fileSearch)\n");
			if( $file =~ m/$fileSearch/i  )
			{
                if(!$minSize or DirSize($file) > $minSize)
                {
                    $file = cwd . "/" . $file;
    				$file =~ s/\//\\/g;
    				chdir ($startDir);
    				return $file;
    			}
			}
		}
	}

    return "" if($dontCheckSubFolders);

    # Find subfolders
	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

		if (-d $file)
		{
		 	my $found = FindDirectory( $file, $fileSearch );
		 	if($found ne "")
		    {
		        chdir ($startDir);
		        $found =~ s/\//\\/g;
		        return $found;
		    }
		}
	}

	chdir ($startDir);
	return "";
}


#------------------------------------------------------------------------------------
# FindFile
#
# Parameters:
#	$goDir, where to start finding
#	$fileSearch, filename search
#   $minSize, minimum size for the file
#
# Returns the file found.
#------------------------------------------------------------------------------------
sub FindFile
{
	my ($godir, $fileSearch, $dontCheckSubFolders, $minSize) = @_;

	my $startDir = cwd;

    #print("Now in $startDir\n");

	chdir($godir);

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

		if( $file =~ m/$fileSearch/i and !(-d $file) )
		{
            if(!$minSize or -s $file > $minSize)
            {
                $file = cwd . "/" . $file;
    			$file =~ s/\//\\/g;
    			chdir ($startDir);
    			return $file;
    		}
		}
	}

    return "" if($dontCheckSubFolders);

    # Find subfolders
	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

		if (-d $file)
		{
		 	my $found = FindFile( $file, $fileSearch );
		 	if($found ne "")
		    {
		        chdir ($startDir);
		        $found =~ s/\//\\/g;
		        return $found;
		    }
		}
	}

	chdir ($startDir);
	return "";
}

#------------------------------------------------------------------------------------
# DirSize
#
# Parameters:
#	$goDir, directory
#
# Returns the size of directory
#------------------------------------------------------------------------------------
sub DirSize
{
	my ($godir) = @_;

	my $startDir = cwd;

	chdir($godir);

	opendir(DIR, ".");
	my @filelist = sort(readdir(DIR));
	closedir(DIR);

    my $size = 0;

	foreach my $file(@filelist)
	{
		if($file eq "." or $file eq "..") {next};

		if (-d $file)
		{
			$size += DirSize($file);
		}
		else
		{
		    $size += -s $file;
		}
	}

	chdir ($startDir);
	return $size;
}

#------------------------------------------------------------------------------------
# GetFilename
#
# Parameters:
# 	$text
#------------------------------------------------------------------------------------
sub GetFilename
{
	my ($file) = @_;
	$file =~ s/\//\\/g; # / -> \
    my $pos = rindex($file, "\\");
    die("Invalid path: $file\n") if($pos == -1);
    return substr($file, $pos+1);
}

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
# Timestamp
#
# Parameters:
#------------------------------------------------------------------------------------
sub Timestamp
{
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);

    return "" . ($year+1900) . "-" . ($mon+1) . "-" . $mday . "-" . $hour . "-" . $min;
}
