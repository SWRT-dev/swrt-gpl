#!/usr/bin/perl
#
# $Id$
# This script is intended to manage version definition
# file used by Metalink driver release system.
# Version definition file contains version in form
#       MTLK_VER=X1.X2. ... .(Xm). ... .[Xk]. ... .Xn
# where Xi are some alphanumeric strings.
# One of the Xi should be enclosed in round brackets.
# One of the Xi should be enclosed in square brackets.
# Component in round version is treated as "major version"
# Component in square brackets is treated as minor version
#
# Script reads input file from STDIN;
#
# There are two modes of the script operation:
#
# 1. Version increase (-i/-I command line parameters):
#  All lines in file except the first line that starts
#  with "MTLK_VER=" keyword are printed to STDOUT as is.
#
#  The first line that starts with "MTLK_VER=" will be
#  procesed in the following way:
#    - If -I command line parameter is given:
#      Major verison will be increased by 1,
#      Minor version will be zeroed;
#    - If -i command line parameter is given:
#      Minor version will be increased by one;
#  The new line contents will be printed to STDOUT
#
# 2. Version extraction (-g command line parameter):
#  All lines in file except the first line that starts
#  with "MTLK_VER=" keyword are ignored.
#  The first line that starts with "MTLK_VER=" will be
#  procesed in the following way:
#    Leading "MTLK_VER=" will be suppressed;
#    All left and right round and square bracket symbols will be suppressed;
#    Resulted string will be printed to STDOUT.
# 3. Branch version extraction (-G command line parameter):
#  All lines in file except the first line that starts
#  with "MTLK_VER=" keyword are ignored.
#  The first line that starts with "MTLK_VER=" will be
#  procesed in the following way:
#    Leading "MTLK_VER=" will be suppressed;
#    Text enclosed by curly braces will be printed to STDOUT
#    Everything else will be suppressed

if ( !defined($ARGV[0]) || ( $ARGV[0] ne "-i" && $ARGV[0] ne "-I" && $ARGV[0] ne "-g" && $ARGV[0] ne "-G" ) )
{
    print STDERR "Usage: \"mtlk_ver.pl -i\" or \"vertool.pl -g\" \n";
    print STDERR "\tmtlk_ver.pl -I print contents of STDIN with generated next major version number to STDOUT\n";
    print STDERR "\tmtlk_ver.pl -i print contents of STDIN with generated next minor version number to STDOUT\n";
    print STDERR "\tmtlk_ver.pl -G extract branch version from STDIN and print to STDOUT\n";
    print STDERR "\tmtlk_ver.pl -g extract version from STDIN and print to STDOUT\n";
    exit 2;
}

if($ARGV[0] eq "-i")
{
    while(<STDIN>)
    {
        if($_ =~ /(^MTLK_VER=.*\[)(\d+)(\].*\n*)/) {print "$1".($2+1)."$3"}
        else {print $_};
    }
    exit 0;
}
if($ARGV[0] eq "-I")
{
    while(<STDIN>)
    {
        $line = $_;
        if($line =~ /(^MTLK_VER=.*\()(\d+)(\).*\n*)/) { $line = "$1".($2+1)."$3" }
        if($line =~ /(^MTLK_VER=.*\[)(\d+)(\].*\n*)/) { $line = "$1".(0)."$3" }
        print $line;
    }
    exit 0;
}
elsif($ARGV[0] eq "-g")
{
    while(<STDIN>)
    {
        if($_ =~ /^MTLK_VER=(\S+)/) 
            {$match = $1; $match =~ s/[\[\]\(\)\{\}]//eg; print $match; exit 0;};
    }
    exit 1;
}
elsif($ARGV[0] eq "-G")
{
    while(<STDIN>)
    {
        if($_ =~ /^MTLK_VER=(\S+)/) 
            {$match = $1; $match =~ s/.*\{(.*)\}.*/$1/eg; print $match; exit 0;};
    }
    exit 1;
}
else {die "Unknown command";};
