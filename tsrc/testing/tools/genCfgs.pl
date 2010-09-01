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
    require( $cgidir . "utils.pl" );
}

#------------------------------------------------------------------------------------
# GLOBAL CODE
#------------------------------------------------------------------------------------

my @paramClasses;

while(scalar(@ARGV) <= 0)
{
    Help();
    exit;
}

while(scalar(@ARGV) >= 1)
{
	my $argument = shift(@ARGV);

	if($argument eq "-c")
	{
		my $class = shift(@ARGV);
		push @paramClasses, $class;
	}
	elsif($argument eq "-h")
	{
		Help();
		exit;
	}
	else  {
        Help();
		exit;
	}
}

sub Help
{
    print <<USAGE_EOF;

Creates new STIF CFG files depending if they have the specified class(es) defined. Add line:
    # CaseClass: <classname> 
before the case to define the class.

Usage: genCfgs.pl -c <classname> (-c classname2)
    
USAGE_EOF
    
    exit;
}

ClassifyCfgs( \@paramClasses );

exit;

sub ClassifyCfgs
{
    my ( $refClasses ) = @_;
    
    system("rmdir /s /q conf_temp");
    mkdir("conf_temp");
    
    my @cfgFiles;
    FindFiles( ".", ".cfg", 0, \@cfgFiles );
    
    my @testframeworkInis;
    FindFiles( ".", "testframework.ini", 0, \@testframeworkInis );
    if( scalar( @testframeworkInis ) < 1 )
    {
        die("\nThere must be at least one testframework.ini somewhere in the subpath!\n");
    }
    print("Using testframework.ini from: " . $testframeworkInis[0] . "\n");
    
    my @pkgFiles;
    
    foreach my $file ( @cfgFiles )
    {
        my $dst = ".\\conf_temp\\" . GetPathFileName( $file );
        if( ClassifyCfg( $refClasses, $file, $dst ) )
        {
            push @pkgFiles, GetPathFileName( $file );
            AddCfgToIni( $testframeworkInis[0], ".\\conf_temp\\Testframework.ini", GetPathFileName( $file ) );
            print("Added: " . GetPathFileName($file) . "\n");
        }
    }
    
    if( scalar(@pkgFiles) > 0 )
    {
        MakeSISX( \@pkgFiles );
    }
    else
    {
        system("rmdir /s /q conf_temp"); # remove empty dir
    }
}

sub ClassifyCfg
{
    my ( $refClasses, $sourceFile, $destFile ) = @_;
    
    # Read the cfg.
	open(FILE_HANDLE, $sourceFile) or die ("ERROR! Could not open file $sourceFile\n");
	my @cfgLines = <FILE_HANDLE>;
	close(FILE_HANDLE);    
        
    my @cfgCases;
    ReadCfgCasesWithClass( \@cfgLines, \@cfgCases, $refClasses );
    
    if( !@cfgCases or scalar(@cfgCases) < 1 )
    {
        return 0;
    }
    
    # Write the helium CFG.
    open(FILE_HANDLE, ">$destFile") or die("Could not write into file $destFile\n");
    
    # Write everything from CFG before the first case.
    foreach my $line ( @cfgLines )
    {
        if( $line =~ m/\[Test\]/ )
        {
            last; 
        }
        print FILE_HANDLE ( $line );
    }
    
    foreach my $refCase ( @cfgCases )
    {
        my $refCaseLines = @$refCase[1];
        print FILE_HANDLE ( @$refCaseLines );
        print FILE_HANDLE ( "\n" );
    }
        
    close( FILE_HANDLE );
    
    return 1;
}

sub ReadCfgCasesWithClass
{
    my ( $refLines, $refCases, $refClasses ) = @_;
    
	my $inCase = 0;
	my @caseLines;
	my $caseTitle;
    my @caseClasses = "";

    my $cfgHasClassCases = 0;

	for( my $i = 0; $i < scalar(@$refLines); $i++ )
	{
	    my $line = @$refLines[$i];
	    
	    # Case has class.
	    if( $line =~ m/#[ ]+CaseClass:/ )
	    {
	        my $pos = index( $line, "CaseClass: " );
	        if( $pos == -1 )
	        {
	            $i++;
	            die("Invalid CaseClass at line $i. Must be specified exactly as '# CaseClass: clas1(,class2,class3..)'.\n");
	        }
	        my $classes = substr( $line, $pos + 11 );
	        RemoveWhiteSpaces(\$classes);
	        @caseClasses = split(/,/, $classes);	        
	    }

	    # Case starts here.
	    if( $line =~ m/\[Test\]/ )
	    {
	        undef( @caseLines );
	        $caseTitle = "";
	        $inCase = 1;
	    }
	    
	    # Found title for the case.
	    if( $line =~ m/^title / )
	    {
	        $caseTitle = $line;
	    }
	    
	    # Save case lines.
	    if( $inCase )
	    {
	        push @caseLines, ( $line );
	    }
	    
	    # End of test case.
	    if( $line =~ m/\[Endtest\]/ )
	    {
	        $inCase = 0;

            my $saveCase = 0;
                    
            foreach my $class ( @$refClasses )
            {
                foreach my $caseClass (@caseClasses)
                {
                    if( $caseClass eq $class )
                    {
                        $saveCase = 1;
                        $cfgHasClassCases = 1;
                        #print("Class case: $caseTitle\n"); 
                    }
                }
            }
            
            if( $caseTitle =~ m/Setup/i or $caseTitle =~ m/not a test/i or $caseTitle =~ m/set up/i 
                or $caseTitle =~m/not test/i or $caseTitle =~m/cleanup/i )
            {
                $saveCase = 1;
                #print("Setup case: $caseTitle\n");
            }
            
            if( $saveCase )
            {
                # Yes, save it.
                my @case;
                $case[0] = $caseTitle;
                $case[1] = [ @caseLines ];
                push @$refCases, [ @case ];
	        
	        }
	        undef( @caseClasses );
	    }
	}
	
	if( !$cfgHasClassCases )
	{
        undef( @$refCases );
	}
}

sub FindFiles
{
	my ($godir, $fileSearch, $searchType, $refIncfiles) = @_;

	my $startDir = cwd;

	chdir($godir);

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
			}
		}
	}

	chdir ($startDir);
}

sub MakeSISX
{
    my ($refFiles) = @_;

    # Continue creating pkg.

    my $startDir = cwd;

    chdir( "conf_temp" );

    my $pkgname = "cfgs.pkg";
    my $sisname = "cfgs.sis";
    my $sisnamex = "cfgs.sisx";

    print("Writing pkg $pkgname\n");
	if( !open(FILE_HANDLE, ">$pkgname") )
	{
		print("ERROR: could not write to file: $pkgname\n");
		return 0;
	}

    my $lineFeed = "\n";

    print FILE_HANDLE (
    "&EN" . $lineFeed .
    "#{\"CFGS\"},(0x10102ae9),0,1,0,TYPE=PU" . $lineFeed .
    "%{\"Nokia EN\"}" . $lineFeed .
    ":\"Nokia\"" . $lineFeed
    );

    foreach my $file (@$refFiles)
    {
        print FILE_HANDLE ("\"..\\conf_temp\\$file\" - \"c:\\TestFramework\\$file\"" . $lineFeed );
    }
    
    my $file = "Testframework.ini";
    print FILE_HANDLE (";\"..\\conf_temp\\$file\" - \"c:\\TestFramework\\$file\"" . $lineFeed );

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

    my $cmd = "makesis $pkgname $sisname";
    print("$cmd\n");
    system $cmd;

    $cmd = "signsis $sisname $sisnamex $derFile $keyFile";
    print("$cmd\n");
    system $cmd;
    
    #system("del $pkgname");
    system("del $sisname");
    
    chdir( $startDir );
}


sub AddCfgToIni
{
    my ($sourceFile, $destIniFile, $cfgFile) = @_;

    # Rerun ini does not exist. Create one.
    if( ! (-e $destIniFile) )
    {
    	if( !open(FILE_HANDLE, $sourceFile) )
    	{
    		print("ERROR! Could not open file '" . $sourceFile . "'\n");
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