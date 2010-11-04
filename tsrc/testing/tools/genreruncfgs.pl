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

{ # No globals.
    # Solve where the script is located.
    $0=~/^(.+[\\\/])[^\\\/]+[\\\/]*$/;
    my $cgidir= $1 || "./";
    
    # And try to solve relative path.
    if( index( $cgidir, "..\\" ) != -1 )
    {
    	my $p = cwd;
    	$p =~ s/\//\\/g;
    	$cgidir =~ s/\//\\/g;
    	while(1) 
    	{
    		my $pos = index( $cgidir, "..\\" );
    		last if( $pos == -1 );
    		$cgidir = substr( $cgidir, $pos+3 );
    		
    		$pos = rindex( $p, "\\" );
    		last if( $pos == -1 );
    		$p = substr( $p, 0, $pos);
    	}
    	$cgidir = $p . "\\" . $cgidir;
    }
    
    if ( ! -e( $cgidir . "rerunsubs.pl" ) )
    {
        $cgidir = cwd;
        my $domain = "VideoApp_Domain";
        my $pos = index( $cgidir, $domain );
        if( $pos != -1 )
        {
            $cgidir = substr( $cgidir, 0, $pos + length( $domain ) ) . "\\videoplayer\\tsrc\\testing\\tools\\";
            
        }
    }
    require( $cgidir . "rerunsubs.pl" );
    require( $cgidir . "utils.pl" );
}

#------------------------------------------------------------------------------------
# GLOBAL CODE
#------------------------------------------------------------------------------------

my $lineFeed = "\n";

my $regexpCaseName = '[<>\x3C\x3E\(\)\w.,\/:_\-&\' ]+';

my $argcount = scalar(@ARGV);
my $paramStartDir = "..";
my $paramQuiet = 0;
my $paramFilename = "";
my $paramMakeSis = 1;
my $paramGenCaseList = 0;

if(scalar(@ARGV) <= 0)
{
    my @xmlFiles;
    FindFiles(".", "xml", 1, \@xmlFiles, 0);
    
    if( scalar(@xmlFiles) > 1 )
    {
    	print("Multiple xml files. Can run without arguments only if there's one testreport xml.\n");
    	foreach my $file ( @xmlFiles ) 
    	{
    	    print("$file\n");
    	}
    	ShowHelp();
    }

    if( scalar(@xmlFiles) <= 0 )
    {
    	print("No testreport xml in current directory, cannot run without arguments.\n");
    	ShowHelp();
    }
    
    $paramFilename = $xmlFiles[0];
    print("No arguments given. Picked file $paramFilename.\n");
}
else
	{
	while(scalar(@ARGV) >= 1)
	{
		my $argument = shift(@ARGV);
	
		if($argument eq "-f")
		{
			$paramFilename = shift(@ARGV);
		}
		elsif($argument eq "-h")
		{
			ShowHelp();
		}
		elsif($argument eq "-q")
		{
			$paramQuiet = 1;
		}
		elsif($argument eq "-sis")
		{
			$paramMakeSis = 1;
		}
		elsif($argument eq "-caselist")
		{
			$paramGenCaseList = 1;
		}
		else  {
			print "arg: $argument\n";
			ShowHelp();
		}
	}
}

my @incfiles;
my $startDir = cwd;
my @failedCfgs;

if( ! -e ".\\init\\testframework.ini" )
{
	die("Must be run from tsrc and there must be .\\init\\\testframework.ini!\n");
}

if( Rerun_ReadFailedCases( $paramFilename, \@failedCfgs ) )
{
    die("Reading the Excel file failed!\n");
}

if( $paramGenCaseList )
{
    if(!open(FILE_HANDLE, ">" . "caselist.txt") )
    {
        print("ERROR! Could not open file for writing '" . "caselist.txt" . "'\n");
        return 0;
    }

    my $currCfg = "";
    foreach my $cfg( @failedCfgs )
    {
        $refCases = @$cfg[1];
        $file = @$cfg[0];

        if($file ne $currCfg)
        {
            print FILE_HANDLE ("cfg $file\n");
        }

        foreach my $failedCaseName (@$refCases)
        {
            print FILE_HANDLE ("case $failedCaseName\n");
        }
    }

    close(FILE_HANDLE);

    exit();
}
else
{
    if(scalar(@failedCfgs) <= 0)
    {
    	print("No fails!\n");
    	exit();
    }
    
    system("rmdir /s /q 1rerun\\\n") if( -e "1rerun" );
    mkdir("1rerun");

    foreach my $cfg( @failedCfgs )
    {
        my $file = @$cfg[0];
        Rerun_GenerateCfg( ".\\conf\\" . $file, $file, \@failedCfgs );
    }

    MakeSISX( \@failedCfgs ) if( $paramMakeSis );

    foreach my $cfg( @failedCfgs )
    {
        my $file = @$cfg[0];
        if( -e ".\\conf\\$file" ) 
        {
            AddCfgToRerunIni( ".\\init\\testframework.ini", $file );
            system("move $file 1rerun\\$file");
        }
    }

}
chdir($startDir);

chdir("1rerun");
system("dir");

exit();

#------------------------------------------------------------------------------------
# ShowHelp
#------------------------------------------------------------------------------------
sub ShowHelp {

print <<USAGE_EOF;
genreruncfgs.pl

parameter -f <Excel xml testreport> (-sis)

Creates cfg files into the run directory, the cfgs will have cases which have failed or are for setup.

The current directory must be tsrc, there must be testframework.ini in ./init and STIF cfg files in ./cfg.

The testreport can be in current directory and then no parameters are needed.

-sis Creates STIF PU sis of the rerun CFGs.
-caselist Creates caselist.txt which lists the failed cases

USAGE_EOF

	exit();

};

#------------------------------------------------------------------------------------
# IniHasCfg
#------------------------------------------------------------------------------------
sub IniHasCfg
{
    my ($iniFile, $cfgFile) = @_;

	if( !open(FILE_HANDLE, $iniFile) )
	{
		print("ERROR! Could not open file '" . $iniFile . "'\n");
		return 0;
	}
	my @array = <FILE_HANDLE>;
	close(FILE_HANDLE);

	my $inNew_Module = 0; # We are parsing [New_Module] section from INI
	my $lineCount = scalar(@array);
	for(my $i=0; $i<$lineCount; $i++ )
	{
	    $line = $array[$i];

        $inNew_Module = 1 if($line =~ m/\[New_Module\]/);
        $inNew_Module = 0 if($line =~ m/\[End_Module\]/);

        if($inNew_Module)
        {
            if($line =~ m/$cfgFile/i)
            {
                return 1;
            }
        }
    }

    return 0;
}

sub AddCfgToRerunIni
{
    my ($iniFile, $cfgFile) = @_;
    
	my $destIniFile = "./1rerun/TestFramework.ini";

    # Create directory for rerun ini
    mkdir( substr($destIniFile, 0, rindex($destIniFile, "/")) );

    # Rerun ini does not exist. Create one.
    if( ! (-e $destIniFile) )
    {
    	if( !open(FILE_HANDLE, $iniFile) )
    	{
    		print("ERROR! Could not open file '" . $iniFile . "'\n");
    		return 0;
    	}
    	my @array = <FILE_HANDLE>;
    	close(FILE_HANDLE);

    	if( !open(FILE_HANDLE, ">$destIniFile") )
    	{
    		print("ERROR! Could not open file '" . $destIniFile . "'\n");
    		return 0;
    	}

        my $inNew_Module = 0;

        foreach $line ( @array )
        {
            if($line =~ m/\[New_Module\]/)
            {
                $inNew_Module = 1;
                print FILE_HANDLE ($line);
                print FILE_HANDLE ("ModuleName= TestScripter\n");
            }
            if($line =~ m/\[End_Module\]/)
            {
                $inNew_Module = 0;
            }

            if($inNew_Module)
            {
                # Copy only setup cfgs if they are found.
                if($line =~ m/setvalidservices/i)
                {
                    print FILE_HANDLE ($line);
                }
                if($line =~ m/testprovisioning/i)
                {
                    print FILE_HANDLE ($line);
                }
            }
            else
            {
                print FILE_HANDLE ($line);
            }
        }

    	close(FILE_HANDLE);
    }

    # Read the existing file.

    if( !open(FILE_HANDLE, $destIniFile) )
	{
		print("ERROR! Could not open file '" . $destIniFile . "'\n");
		return 0;
	}
	my @array = <FILE_HANDLE>;
	close(FILE_HANDLE);

    # Check if CFG is already there.

    my $inNew_Module = 0;

    foreach $line ( @array )
    {
        if($line =~ m/\[New_Module\]/)
        {
            $inNew_Module = 1;
        }
        if($line =~ m/\[End_Module\]/)
        {
            $inNew_Module = 0;
        }

        if($inNew_Module)
        {
            if($line =~ m/$cfgFile/i)
            {
                return 1;
            }
        }
    }

    # Add the cfg to it.

	if( !open(FILE_HANDLE, ">$destIniFile") )
	{
		print("ERROR! Could not open file '" . $destIniFile . "'\n");
		return 0;
	}

    $inNew_Module = 0;

    foreach $line ( @array )
    {
        if($line =~ m/\[End_Module\]/)
        {
            $inNew_Module = 0;
            print FILE_HANDLE ("TestCaseFile= c:\\TestFramework\\$cfgFile\n");
        }
        print FILE_HANDLE ($line);
    }

	close(FILE_HANDLE);

    return 1;
}

#------------------------------------------------------------------------------------
# MakeSISX
#
# Parameters:
# 	$refCfgs
#------------------------------------------------------------------------------------
sub MakeSISX
{
    my ($refCfgs) = @_;

    my $pkgname = "STIF_failedcases.pkg";
    my $sisname = "STIF_failedcases.sis";
    my $sisnamex = "STIF_failedcases.sisx";

    print("Writing pkg $pkgname\n");
	if( !open(FILE_HANDLE, ">$pkgname") )
	{
		print("ERROR: could not write to file: $pkgname\n");
		return 0;
	}

    print FILE_HANDLE (
    "&EN" . $lineFeed .
    "#{\"RERUN\"},(0x00000000),0,1,0,TYPE=PU" . $lineFeed .
    "%{\"Nokia EN\"}" . $lineFeed .
    ":\"Nokia\"" . $lineFeed
    );

    foreach my $cfg (@$refCfgs)
    {
        my $file = @$cfg[0];
        if( -e $file )
        {
            print FILE_HANDLE ("\"$file\" - \"c:\\TestFramework\\$file\"" . $lineFeed );
        }
    }

    close(FILE_HANDLE);

    # Check that we have signing keys.
    
    my $derFile = "RDTest_02.der";
    my $keyFile = "RDTest_02.key";

    if (-e $derFile and -e $keyFile)
    {
        # Keys are in current dir
    }
    elsif(-e "sis\\$derFile" and -e "sis\\$keyFile")
    {
        # Keys are in internal/sis dir
        $derFile = "sis\\$derFile";
        $keyFile = "sis\\$keyFile";
    }
    elsif(-e "group\\$derFile" and -e "group\\$keyFile")
    {
        # Keys are in internal/group dir
        $derFile = "group\\$derFile";
        $keyFile = "group\\$keyFile";
    }    
    else
    {
        # Find em.
        my @derFiles;
        my @keyFiles;
        FindFiles( ".", "$derFile", 0, \@derFiles );
        FindFiles( ".", "$keyFile", 0, \@keyFiles );
        $derFile = $derFiles[0] if( scalar(@derFiles) > 0 );
        $keyFile = $keyFiles[0] if( scalar(@keyFiles) > 0 );
    }
    
    if (!-e $derFile or !-e $keyFile)
    {
        print("\nNeed signing keys: $derFile and $keyFile in current or subdirs.\n");
        return;
    }
    
    # Create SISX

    my $cmd = "makesis $pkgname $sisname";
    print("$cmd\n");
    system $cmd;

    $cmd = "signsis $sisname $sisnamex $derFile $keyFile";
    print("$cmd\n");
    system $cmd;

    system("move $sisnamex 1rerun\\$sisname");

    system("del $pkgname");
    system("del $sisname");
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
	my ($godir, $fileSearch, $searchType, $refIncfiles, $recurse) = @_;

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
		 	FindFiles( $file, $fileSearch, $searchType, $refIncfiles) if( $recurse );
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
