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
use FindBin; # for FindBin:Bin

#------------------------------------------------------------------------------------
# GLOBAL CODE
#------------------------------------------------------------------------------------

my $version = 10.1;
my $file = "";

while(scalar(@ARGV) >= 1)
{
	my $argument = shift(@ARGV);

	if($argument eq "-v")
		{
		$version = shift(@ARGV);
		}
		
	if($argument eq "-f")
		{
		$file = shift(@ARGV);
		}
}

open(FILE_HANDLE, $file) or die ("Could not read file '$file'\n");
my @lines = <FILE_HANDLE>;
close(FILE_HANDLE);

foreach my $line ( @lines )
  {
  if( $line =~ /\/epoc32([^\"]+)\.dll/i )
   	{
     	my $filename = "/epoc32" . $1 . ".dll";
			$filename =~ s/\//\\/g;
			print $filename . " TO VERSION " . $version . "\n";
			system("elftran $filename -version $version");
		}
	}