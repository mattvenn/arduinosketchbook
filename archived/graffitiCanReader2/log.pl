#!/usr/bin/perl
use strict;
use warnings;
#run this in the terminal first
#stty -F /dev/ttyUSB0 57600 raw

my $dev = '/dev/ttyUSB0';

open( CAN, $dev ) or die "couldn't open $dev: $!\n";
print "opened $dev OK\n";

while( my $line = <CAN> )
{
	#the line looks like "Got: FF FF"
	if( $line =~ m/Got:\s+([A-Z0-9]+)\s+([A-Z0-9]+)/ )
	{
		my $pressure = $1;
		my $distance = $2;
		printf( "distance: %3d pressure: %3d\n", hex( $distance ), hex( $pressure ) );
	}
	else
	{
		warn "got unexpected output: $!\n";
	}
}
