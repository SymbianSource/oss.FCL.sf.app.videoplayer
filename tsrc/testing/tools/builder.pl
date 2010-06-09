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


use Cwd; # for cwd

my $debug = 0;
my $allowRun = 1;
my $optionClean = 1;
my $optionBuild = 1;
my $optionFusion = 1;
my $optionEngineTests = 1;
my $optionUnitTests = 1;
my $optionPlatform = "";

########################################
# BUILD LISTS
#

my @fusionComponents = ( 
		"videoutils\\group",
		"videoplayer\\group"
	);

my @engineTests = (
		"videoutils\\tsrc\\group",
		"videoplayer\\tsrc\\group"
	);

my @unitTests = (
		"videoplayer\\videoplayback\\videohelix\\tsrc\\ut_videohelixtest\\group",
		"videoplayer\\videoplayback\\videoplaybackcontrols\\tsrc\\videoplaybackcontrols_test\\group",
		"videoplayer\\videoplayback\\videoplaybackviews\\tsrc\\ut_userinputhandlertest\\group",
		"videoplayer\\videoplayerapp\\mpxvideoplayer\\tsrc\\ut_mpxvideoplayertest\\group",
		"videoplayer\\videocollection\\hgmyvideos\\tsrc\\ut_vcxhgmyvideosmainview\\group"
	);

########################################
# PARSE ARGUMENTS
#

while(scalar(@ARGV) >= 1)
{
	my $argument = shift(@ARGV);

	if($argument eq "-nobuild")
	{
		$optionBuild = 0;
	}

	elsif($argument eq "-noclean")
	{
		$optionClean = 0;
	}

	elsif($argument eq "-nounit")
	{
		$optionUnitTests = 0;
	}
	
	elsif($argument eq "-noengine")
	{
		$optionEngineTests = 0;
	}

	elsif($argument eq "-nofusion")
	{
		$optionFusion = 0;
	}
	
	elsif($argument eq "-platform")
	{
		$optionPlatform = shift(@ARGV);
	}

	elsif($argument eq "-debug")
	{
		$debug = 1;
	}

	elsif($argument eq "-norun")
	{
		$allowRun = 0;
	}
	
	elsif($argument eq "-help")
	{
		ShowHelp();
	}
	
	else
	{
		ShowHelp();
	}
}

########################################
# MAIN FUNCTION
#

my $basepath = FindBasePath();
exit("NO VIDEOAPP_DOMAIN FOLDER FOUND") if $basepath eq "";

if ( $optionClean )
{
	if ( $optionFusion )
	{
		foreach my $component ( @fusionComponents )
		{
		    Clean( $basepath, $component, $optionPlatform, 0 );
		}
	}
		
	if ( $optionEngineTests )
	{
		foreach my $component ( @engineTests )
		{
		    Clean( $basepath, $component, $optionPlatform, 1 );
		}
	}
		
	if ( $optionUnitTests )
	{
		# Clean all unit tests
		foreach my $component ( @unitTests )
		{
		    Clean( $basepath, $component, $optionPlatform, 1 );
		}
	}
}

if ( $optionBuild )
{
	if ( $optionFusion )
	{
		foreach my $component ( @fusionComponents )
		{
		    Build( $basepath, $component, $optionPlatform, 0 );
		}
	}
		
	if ( $optionEngineTests )
	{
		# Clean all engine tests
		foreach my $component ( @engineTests )
		{
		    Build( $basepath, $component, $optionPlatform, 1 );
		}
	}
		
	if ( $optionUnitTests )
	{
		# Clean all unit tests
		foreach my $component ( @unitTests )
		{
		    Build( $basepath, $component, $optionPlatform, 1 );
		}
	}
}

########################################
# FindBasePath
#

sub FindBasePath
{
	print ">> FindBasePath\n" if $debug;
	$currpath = cwd;
	$basepath = "";
	my $domain = "VideoApp_Domain";
	
	my $pos = rindex( $currpath, $domain );
	
	if( $pos != -1 )
	{
		$basepath = "\\" . substr( $currpath, 3, $pos + length( $domain ) - 3 );
		$basepath =~ s/\//\\/g;
	  print "BASEPATH FOUND: $basepath\n" if $debug;
	}
	
	print "<< FindBasePath (" . $basepath . ")\n" if $debug;
	return $basepath;
}

########################################
# Clean
#

sub Clean
{
	my ( $baseDir, $destDir, $optionPlatform, $isTestComponent ) = @_;
	print ">>> CLEAN ${baseDir}\\${destDir}\\bld.inf FOR $optionPlatform\n" if $debug;
	print "##\n#\n# CLEANING ${baseDir}\\${destDir}\n#\n##\n";
	
	my $cmd = "";
	
	if( $isTestComponent )
	{
		if( $optionPlatform eq "" ) # platform not given, clean armv5.test and winscw.test
		{
			$cmd = "sbs reallyclean -b ${baseDir}\\${destDir}\\bld.inf -c winscw.test";
			print $cmd . "\n" if $debug;
			system($cmd) if $allowRun;
			$cmd = "sbs reallyclean -b ${baseDir}\\${destDir}\\bld.inf -c armv5.test";
			print $cmd . "\n" if $debug;
			system($cmd) if $allowRun;
		}
		else # platform is given, run clean only for that
		{
			$cmd .= "sbs reallyclean -b ${baseDir}\\${destDir}\\bld.inf -c ${optionPlatform}\.test";
			print $cmd . "\n" if $debug;
			system($cmd) if $allowRun;
		}
	}
	else # is not testcomponent
	{
		$cmd .= "sbs reallyclean -b ${baseDir}\\${destDir}\\bld.inf";
		$cmd .= " -c $optionPlatform" if $optionPlatform ne "";
		print $cmd . "\n" if $debug;
		system($cmd) if $allowRun;
	}
	print "<<< CLEAN\n" if $debug;
}

########################################
# Build
#

sub Build
{
	my ( $baseDir, $destDir, $optionPlatform, $isTestComponent ) = @_;
	print ">>> BUILD ${baseDir}\\${destDir}\\bld.inf FOR $optionPlatform\n" if $debug;
	print "##\n#\n# BUILDING ${baseDir}\\${destDir}\n#\n##\n";
	#my $cmd = "sbs -b ${baseDir}\\${destDir}\\bld.inf";
	#$cmd .= " -c $optionPlatform" if $optionPlatform ne "";
	#$cmd .= ".test" if $isTestComponent;
	#print $cmd . "\n" if $debug;
	#system($cmd) if $allowRun;
	
	my $cmd = "";
	
	if( $isTestComponent )
	{
		if( $optionPlatform eq "" ) # platform not given, clean armv5.test and winscw.test
		{
			$cmd = "sbs -b ${baseDir}\\${destDir}\\bld.inf -c winscw.test";
			print $cmd . "\n" if $debug;
			system($cmd) if $allowRun;
			$cmd = "sbs -b ${baseDir}\\${destDir}\\bld.inf -c armv5.test";
			print $cmd . "\n" if $debug;
			system($cmd) if $allowRun;
		}
		else # platform is given, run clean only for that
		{
			$cmd .= "sbs -b ${baseDir}\\${destDir}\\bld.inf -c ${optionPlatform}\.test";
			print $cmd . "\n" if $debug;
			system($cmd) if $allowRun;
		}
	}
	else # is not testcomponent
	{
		$cmd .= "sbs -b ${baseDir}\\${destDir}\\bld.inf";
		$cmd .= " -c $optionPlatform" if $optionPlatform ne "";
		print $cmd . "\n" if $debug;
		system($cmd) if $allowRun;
	}
	
	print "<<< BUILD\n" if $debug;
}

#------------------------------------------------------------------------------------
# ShowHelp
#------------------------------------------------------------------------------------
sub ShowHelp {

print <<USAGE_EOF;
builder.pl

This script will search the VideoApp_Domain root folder from current path.
if the root folder is not found, the script will exit. In any other case,
it builds all components.

Options:

-platform   wanted platform
-noclean    no clean.
-nobuild    no build.
-nofusion   exclude fusion components.
-noengine   exclude engine tests.
-nounit     exclude unit tests.
-debug      show debug message
-norun      do not run, for debugging purposes
-help       You're reading it.

Examples:

builder.pl -nobuild -platform armv5_udeb
	This will clean all the armv5_udeb components without building them.

builder.pl -noclean -nofusion -noengine -platform armv5_udeb
	This will build only unit tests without cleaning them first.

USAGE_EOF

	exit();

};