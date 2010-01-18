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

my $regexpCaseName = '[<>\x3C\x3E\(\)\w.,\/:_\-&\' ]+';

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
# Rerun_ReadFailedCases
# Parameters:
#	$filename, filename of the result IPTV_Engine testreport.
#	$refCfgs, reference to array which contains failed cases as a result
#
# array
#  {
#    array { $cfgFilename, @cases },
#    array { $cfgFilename, @cases },
#    ...
#  }
#
# returns 1 if something fails, otherwise 0
#------------------------------------------------------------------------------------

sub Rerun_ReadFailedCases
{
    my( $filename, $refCfgs ) = @_;

	if( !open(FILE_HANDLE, $filename) )
	{
		print("ERROR! Could not open file '" . $filename . "'\n");
		return 1;
	}
	my @array = <FILE_HANDLE>;
	close(FILE_HANDLE);

    my $refCfg;
    my $cfgHasFailedCases = 0;

	my $lineCount = scalar(@array);
	for($i=0; $i<$lineCount; $i++ )
	{
	    $line = $array[$i];
        XmlToText(\$line);
	    $line =~ s/\'/\"/g;

        # Read the name of cfg
	    if($line =~ m/Cell ss:MergeAcross=\"11\" ss:StyleID=\"/ )
	    {
	        #<Cell ss:MergeAcross="11" ss:StyleID="m150670304"><Data ss:Type="String">IptvEpgEngineTest.cfg</Data></Cell>
	        if($line =~ m/>([a-z0-9_\-]+).cfg<\/Data>/i)
	        {
                # Add cfg to cfg list if it has failed cases.
                if( defined( @$refCfg ) and $cfgHasFailedCases )
                {
                    push @$refCfgs, [ @$refCfg ];
                }

                # Initialize new array for new cfg, if the cfg exists.
                $cfgHasFailedCases = 0;
     	        
     	        my $cfgFilename = $1 . ".cfg";

    	        my @cfg;
    	        $refCfg = \@cfg;
    	        push @cfg, $cfgFilename;
    	        my @cases;
    	        push @cfg, [@cases];
	        }
	    }

        #<Cell ss:MergeAcross="5" ss:StyleID="m150670456"><Data ss:Type="String">ET17099 &lt;not a test&gt; Setup EPG tests</Data></Cell>
        #<Cell ss:MergeAcross="5" ss:StyleID="m150686796"><Data ss:Type="String">ET17001 Update EPG 3G</Data></Cell>
	    #<Cell ss:StyleID="s57"><Data ss:Type="String">PASSED</Data></Cell>

	    # Read case and the result
	    if( defined( @$refCfg ) && $line =~ m/Cell ss:MergeAcross=\"5\" ss:StyleID=\"/ )
	    {
	        if($line =~ m/>($regexpCaseName)<\/Data><\/Cell>/i)
	        {
	            my $caseName = $1;
                my $saveCase = 0;

	            $line2 = $array[$i+1];
	            XmlToText(\$line2);
	            
	            #print("Found case: $caseName\n");
	            
	            if( $line =~ m/Setup/i or $line =~ m/not a test/i or $line =~ m/set up/i or $line =~m/not test/i or $line =~m/cleanup/i)
	            {
	                #print(" - is for setup\n");
	                $saveCase = 1;
	            }

	            # If case has failed.
	            if($line2 =~ m/>CRASHED</ or $line2 =~ m/>FAILED</)
	            {
	                #print(" - failed\n");
	                $saveCase = 1;
	                $cfgHasFailedCases = 1;
	            }

                # It's marked for rerun.
	            if($saveCase)
	            {
                    my $refCases = @$refCfg[1];
	                push @$refCases, $caseName;
	                #print(" - ADDED\n");
	            }
	        }
        }
	}

    # If cfg has failed cases then add it
    if(defined(@$refCfg) and $cfgHasFailedCases != 0)
    {
        push @$refCfgs, [ @$refCfg ];
    }
	return 0;
}


#------------------------------------------------------------------------------------
# Rerun_GenerateCfg
# Parameters:
#   $srcFile, $destFile
#------------------------------------------------------------------------------------
sub Rerun_GenerateCfg
{
    my($srcFile, $destFile, $refFailedCfgs) = @_;
    
    my $refCases = 0;
    
    foreach my $cfg( @$refFailedCfgs )
    {
        my $cfgFile = GetPathFileName( $srcFile );
        if( @$cfg[0] =~ m/$cfgFile/i )
        {
            $refCases = @$cfg[1];    
            last;
        }
    }
    
    if( !$refCases )
    {
        my $cfgFile = GetPathFileName( $srcFile );
        #print("Writing empty cfg for $cfgFile, no fails.\n");

        open(FILE_HANDLE, ">$destFile");
        close(FILE_HANDLE);
        
        return 0;
    }
    
    my @foundFailedCases;

    my @lines;

	if( open(FILE_HANDLE, $srcFile) )
	{
		@lines = <FILE_HANDLE>;
		close(FILE_HANDLE);
	}
	else
	{
		print("ERROR: could not read file: $srcFile\n");
		return 0;
	}

	print("Writing cfg $destFile\n");
	if( !open(FILE_HANDLE, ">$destFile") )
	{
		print("ERROR: could not write to file: $destFile\n");
		return 0;
	}

    my $lastLineBeforeCases = 0;
    for( my $i=0; $i<scalar(@lines); $i++ )
    {
        $line = $lines[$i];
        $lastLineBeforeCases = $i if( $line =~ m/\[Enddefine\]/i or $line =~ m/\[EndSub\]/i );
    }

    # Write lines before first case
    foreach my $line(@lines)
    {
        print FILE_HANDLE ( $line );

        if($line =~ m/\[Enddefine\]/i)
        {
            last;
        }
    }

    # Write all subs

    my $inSub = 0;
    my $refSubLines;
    foreach my $line (@lines)
    {
        RemoveWhiteSpaces(\$line);

        # Sub starts?
        if($line =~ m/\[Sub [a-zA-Z0-9]+\]/)
        {
            $inSub= 1;
            my @subLines;
            $refSubLines = \@subLines;
        }

        # Add lines
        if($inSub)
        {
            push @$refSubLines, $line;
        }

        # Sub ends?
        if($line =~ m/\[EndSub\]/)
        {
            $inSub = 0;

            foreach $l (@$refSubLines)
            {
                print FILE_HANDLE ( "$l" . "\n" );
            }

            print FILE_HANDLE ( "\n" );
            print FILE_HANDLE ( "\n" );
        }
    }

    print FILE_HANDLE ( "\n" );
    print FILE_HANDLE ( "\n" );

    my $savedCases = 0;

    # Write cases
    my $inCase = 0;
    my $refCaseLines;
    my $writeCase = 0;
    foreach my $line(@lines)
    {
        RemoveWhiteSpaces(\$line);

        # Case starts?
        if($line =~ m/\[Test\]/)
        {
            $inCase = 1;
            my @caseLines;
            $refCaseLines = \@caseLines;
            $writeCase = 0;
        }

        # Add lines
        if($inCase)
        {
            push @$refCaseLines, $line;
        }

        # Case ends?
        if($line =~ m/\[Endtest\]/)
        {
            $inCase = 0;

            # And it had failed. Write into new cfg.
            if($writeCase)
            {
                foreach $l(@$refCaseLines)
                {
                    print FILE_HANDLE ( "$l" . "\n" );
                }

                print FILE_HANDLE ( "\n" );
                print FILE_HANDLE ( "\n" );
            }
        }

        # title line for the case
        if($inCase and $line =~ m/^title /)
	    {
	    	my $caseName = substr($line, length("title "));
	    	RemoveWhiteSpaces(\$caseName);

            foreach my $failedCaseName (@$refCases)
            {
                my $addFoundCase = 0;
                if( $line =~ m/Setup/i or $line =~ m/not a test/i or $line =~ m/set up/i or $line =~m/not test/i or $line =~m/cleanup/i)
                {
                    $addFoundCase = 1;
                }
                
                if($failedCaseName eq $caseName)
                {
                    $addFoundCase = 1;
                }
                if( $addFoundCase )
                {
                    push @foundFailedCases, $caseName;
                    print(" - Case: $failedCaseName\n");
                    $writeCase = 1;
                    $savedCases++;
                }
            }
        }
    }

    my $caseIsFound = 0;
    foreach my $failedCaseName (@$refCases)
    {
        foreach my $foundCaseName (@foundFailedCases)
        {
            $caseIsFound = 1 if($foundCaseName eq $failedCaseName);
        }
        if(!$caseIsFound)
        {
            print("ERROR: Case '$failedCaseName' couldn't be found from the CFG files!\n");
        }
        $caseIsFound = 0;
    }
}
