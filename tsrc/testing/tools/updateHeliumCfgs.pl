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

my $argcount = scalar(@ARGV);

while(scalar(@ARGV) >= 1)
{
	my $argument = shift(@ARGV);

	if($argument eq "-h")
	{
		Help();
		die;
	}
	else  {
        Help();
		die;
	}
}

sub Help
{
    print("\nThis script updates test cases in *Helium.cfg files. First it reads all the CFGs from the same folder and then writes new *Helium.CFG\n");
    exit;
}


UpdateHeliumCfgs();

exit;

sub UpdateHeliumCfgs
{
    my @cfgFiles;
    
    FindFiles( ".", "Helium.cfg", 0, \@cfgFiles );
    
    foreach my $file ( @cfgFiles )
    {
        print("Updating $file.\n");
        UpdateHeliumCfg( $file );
    }
}

sub UpdateHeliumCfg
{
    my ( $fileName ) = @_;
    
    my $file = GetPathFileName( $fileName );
    my $path = GetPathDir( $fileName );
    
    # Read the cfg.
	open(FILE_HANDLE, $fileName) or die ("ERROR! Could not open file $fileName\n");
	my @heliumCfgLines = <FILE_HANDLE>;
	close(FILE_HANDLE);    

    # Find all cases in the cfg.
    my @heliumCases;

    foreach my $line ( @heliumCfgLines )
    {
        if( $line =~ m/^title /i )
        {
            push @heliumCases, ( $line );
        }
    }
    
    # Get all cfgs in same dir as the Helium cfg.
    my @cfgs;
    FindFiles( $path, ".cfg", 0, \@cfgs );
    
    # Get all cfgs that have cases in Helium cfg.
    my @cfgsWithHeliumCases;
    
    foreach my $cfg ( @cfgs )
    {
        # Do not read cases again from helium cfg.
        next if( $cfg =~ /helium\.cfg/i );
        
        my $cfgHasCasesForHelium = 0;
        
        foreach my $heliumCase ( @heliumCases )
        {
            if( CfgHasCase( $cfg, $heliumCase ) )
            {
                $cfgHasCasesForHelium = 1;
                last;
            }
        }
        
        if( $cfgHasCasesForHelium )
        {
            push @cfgsWithHeliumCases, ( $cfg );
        }
    }
    
    # Read cases, defs and subs 
    my @cfgDefsAndIncludes;
    my @cfgSubs;
    my @cfgCases;

    foreach my $cfg ( @cfgsWithHeliumCases )
    {
        print("Reading: " . GetPathFileName($cfg) . "\n");
        ReadCfgDefs( $cfg, \@cfgDefsAndIncludes );
        ReadCfgSubs( $cfg, \@cfgSubs );
        ReadCfgCases( $cfg, \@cfgCases, \@heliumCases );
    }
    
    # Write the helium CFG.
    open(FILE_HANDLE, ">$fileName") or die("Could not write into file $fileName\n");
    
    # Write everything from helium CFG before the first defs.
    foreach my $line ( @heliumCfgLines )
    {
        if( $line =~ m/\[Define\]/ )
        {
            last; 
        }
        print FILE_HANDLE ( $line );
    }
    
    # Write defs, includes and subs.
    print FILE_HANDLE ( @cfgDefsAndIncludes );
    print FILE_HANDLE ( "\n" );
    print FILE_HANDLE ( @cfgSubs );
    print FILE_HANDLE ( "\n" );
    
    # Write cases, in same order as they were.
    foreach my $heliumCase ( @heliumCases )
    {
        my $caseFound = 0;
        
        foreach my $refCase ( @cfgCases )
        {
            if( @$refCase[0] eq $heliumCase )
            {
                my $refCaseLines = @$refCase[1];
                print FILE_HANDLE ( @$refCaseLines );
                $caseFound = 1;
                last;
            }
        }
        
        if( $caseFound )
        {
            print FILE_HANDLE ( "\n" );
        }
        else
        {
            print(" - the cfg has following case but it was not found from other CFGs: $heliumCase\n");
        }
    }
    
    close( FILE_HANDLE );
}

sub CfgHasCase
{
    my ( $cfgFileName, $case ) = @_;
    
	open(FILE_HANDLE, $cfgFileName) or die ("ERROR! Could not open file $cfgFileName\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);    
	
	foreach my $line ( @lines )
	{
	    if( $line =~ m/$case/i )
	    {
	        return 1;
	    }
	}	
	return 0;
}

sub ReadCfgDefs
{
    my ( $cfgFileName, $refDefsAndIncludes ) = @_;
    
	open(FILE_HANDLE, $cfgFileName) or die ("ERROR! Could not open file $cfgFileName\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);    

	my $readLines = 0;
	foreach my $line ( @lines )
	{
	    # Defs starts here.
	    if( $line =~ m/\[Define\]/ )
	    {
	        $readLines = 1;
	    }
	    
	    # Reading the defs.
	    if( $readLines )
	    {
	        my $alreadyHaveThisLine = 0;
	        
	        # Check if we got the line already.
	        foreach my $defLine ( @$refDefsAndIncludes )
	        {
	            if( $defLine eq $line )
	            {
	                $alreadyHaveThisLine = 1;
	            }
	        }
	        
	        # If not, save it.
	        if( !$alreadyHaveThisLine )
	        {
	            print(" - Def: $line");
	            push @$refDefsAndIncludes, ( $line );
	        }
	    }
	    
	    # End of defs.
	    if( $line =~ m/\[Enddefine\]/ )
	    {
	        return;
	    }
	}
}

sub ReadCfgSubs
{
    my ( $cfgFileName, $refSubs ) = @_;
    
	open(FILE_HANDLE, $cfgFileName) or die ("ERROR! Could not open file $cfgFileName\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);
	
	my $inSub = 0;
	foreach my $line ( @lines )
	{
	    # A sub function starts here.
	    if( $line =~ m/\[Sub/ )
	    {
	        $inSub = 1;
            # Check if we got it already.
	        foreach my $subLine ( @$refSubs )
	        {
	            if( $subLine =~ m/\[Sub/ && $subLine eq $line )
	            {
	                $inSub = 0; # Sub with same name has been already read.
	                last; # Breakout from checking.
	            }
	        }
	        if( $inSub )
	        {
	            print(" - Sub: $line");
	        }
	    }
	    
	    # Reading sub.
	    if( $inSub )
	    {
	        push @$refSubs, ( $line );
	    }
	    
	    # Sub ends here.
	    if( $line =~ m/\[EndSub\]/ )
	    {
	        $inSub = 0;
	    }
	}
}

sub ReadCfgCases
{
    my ( $cfgFileName, $refCases, $refCasesToRead ) = @_;
    
	open(FILE_HANDLE, $cfgFileName) or die ("ERROR! Could not open file $cfgFileName\n");
	my @lines = <FILE_HANDLE>;
	close(FILE_HANDLE);
	
	my $inCase = 0;
	my @caseLines;
	my $caseTitle;

	foreach my $line ( @lines )
	{
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
	        
	        # Was the case in list of cases to read.
	        foreach my $heliumCases ( @$refCasesToRead )
	        {
	            if( $caseTitle eq $heliumCases )
	            {
    	            print(" - Case: $caseTitle");

	                # Yes, save it.
	                my @case;
	                $case[0] = $caseTitle;
	                $case[1] = [ @caseLines ];
	                push @$refCases, [ @case ];
	            }
	        }
	    }
	}
}

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
