#!/usr/bin/perl -w
#
# Metalink logger preprocessor script
#
# Usage: perl assertprep.pl <Compiler> <Origin ID> <Origin Name> <FID list header file> <scd file name>
#
# $Id: assertprep.pl 22222 2011-08-09 13:24:37Z modlin $
#

use English;

my $debug = 0;

# When it comes to multiline macros that use __LINE__
# macro inside there is a difference in preprocessors behaviour
# Some of them (like one used in GCC) substitute __LINE__ with number
# of the last line occupied by macro, others, like one used in Green Hils MULTI
# substitute it with number of the first line occupied by macro
# This variable changes behaviour of the script accordingly.
my $multiline_macro_binding_first_line = 0;

my $HeaderData="";
my $SourceData="";
my @StringsList=();

my $curr_file_id;

my $usage_text = <<USAGE_END;
The command line is incorrect.
Usage:
    perl assertprep.pl <Compiler> <Origin ID> <Origin Name> <FID list header file> <scd file name> <org_file_name>
    Compilers supported:
        1. gcc
        2. mutli

USAGE_END

($#ARGV + 1) == 6 or die($usage_text);

(my $TargetCompiler, my $OriginID, my $OriginName, my $FIDListFile, my $SCDFile, my $org_file_name) = @ARGV;

if($TargetCompiler eq "gcc")
{
    $multiline_macro_binding_first_line = 0;
}
elsif($TargetCompiler eq "multi")
{
    $multiline_macro_binding_first_line = 1;
}
else
{
    die($usage_text);
}

sub dbg_print($)
{
    printf(STDERR shift) if($debug);
}

sub curr_file_id
{
  my $file_id = shift;

  return ($file_id eq $curr_file_id);
}

sub clear_current_file_strings
{
  return if not defined $curr_file_id;

  my @NewStringList;

  foreach  $StringRef (@StringsList)
  {
    (my $line_number, my $file_id, my $format_string) = @$StringRef;
    push @NewStringList, [ ($line_number, $file_id, $format_string) ] if not curr_file_id($file_id);
  }

  @StringsList = @NewStringList;
}

sub process_file_definition
{
  my $file_id = shift;
  my $file_name = shift;

  push(@FIDList, [ ($file_id, $file_name) ]);
  $curr_file_id = $file_id;
  clear_current_file_strings();

  return $file_id;
}

sub process_print_command
{
  my $line = shift;
  my $line_number = shift;
  my $macro = shift;
  my $log_level = shift;
  my $text_before_format_string = shift;
  my $format_string = shift;
  my $text_after_format_string = shift;

  #Eliminate escaped endlines and merge chunks of
  #the format string and put it into the strings database
  my $SCDFormatString = $format_string;
  $SCDFormatString =~ s/\\\n//gm;
  $SCDFormatString =~ s/\"[\s\n]*\"//gm;

  if (not defined $curr_file_id)
  {
    print STDERR "No file ID definition before printout at line $line_number\n";
    exit 1;
  }

  push @StringsList, [ ($line_number, $curr_file_id, $SCDFormatString) ];
}

#Read FID list and build number to name conversion hash
my %FileIDToName = ();
if(-e $FIDListFile)
{
  open my $fh, '<', $FIDListFile or die "error opening $FIDListFile for reading: $!";
  local $INPUT_RECORD_SEPARATOR="\n";
  while (<$fh>)
  {
      die "Incorrect FID list entry $_" if(not s/^\#define\s+(\w+)\s+(\w+)\s+\/\/+(.*)$/push(@FIDList, [ ($1, $3) ]) and ($FileIDToName{$2} = $1)/eg);
  }
  @FIDListOrig = @FIDList;
}

#Read string list
if(-e $SCDFile)
{
  open my $fh, '<', $SCDFile or die "error opening $SCDFile for reading: $!";
  while (<$fh>)
  {
    s/^S\s+(\w+)\s+(\w+)\s+(\w+)\s+(.*)$
     /push(@StringsList, [ ($3, $FileIDToName{$2}, $4) ])/egx;
  }
}

$INPUT_RECORD_SEPARATOR=";";
my $MacroNameRe = '((?<!\w)ASSERT|FATAL([0-9]){0,1})(?: _[A-Z]+){0,1}';
my $MacroNameReOnlyFatal = '((?<!\w)FATAL([0-9]){0,1})(?: _[A-Z]+){0,1}';
my $CurrentLineNumber = 1;

while (<STDIN>)
{
    my $line=$_;
    my $orig_line = $line;

    #If chunk contains printout - get number of lines before printout
    #This number will be used to determine line number of the printout macro
    my $LinesBeforePrintout = 0;
    if($line =~ /.*$MacroNameRe(.|\n)*/)
    {
      if($multiline_macro_binding_first_line)
      {
          $line =~ s/$MacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
    }

    #Find fileAssert IDs
    $line =~ s/(LOCAL_ASSERT_FID\s+)(\w+)/$1.process_file_definition($2, $org_file_name)/xemg;

    #Process printouts only for fatal
	if($line =~ /.*$MacroNameReOnlyFatal(.|\n)*/)
    {
		$line =~ s/
			$MacroNameReOnlyFatal                          #Macro name
            ((?: [^"]|\n)*")                                #Text between macro name and beginning of the format string
            ((?: .|\n)*?)                                   #Format string
            ((?<!\\)"\s*[\,\)](?: .|\n)*)                   #Text after format string
            /process_print_command($orig_line, $CurrentLineNumber + $LinesBeforePrintout, $1, $2, $3, $4, $5)/xemg;
	}
    	
    $CurrentLineNumber += $line =~ s/\n/\n/mg;

    print $line;
}

open my $fhSCD, '>', $SCDFile or die "error opening $SCDFile for writing: $!";
print $fhSCD "O $OriginID $OriginName\n";

#Flush FID list into both C header and SCD file
my $i=1;
my @FIDListNew = ();
my %seen = ();

foreach $FID (@FIDList)
{
  (my $myFid, my $myFname) = @$FID;
  unless ($seen{$myFid}++)
  {
    push(@FIDListNew, $FID);
    print $fhSCD "F $OriginID ".$i++." $myFid $myFname\n";
  }
}

#If new FIDs were added we have to rebuild the FID list file
if($#FIDListNew != $#FIDListOrig)
{
  $i=1;
  open my $fh, '>', $FIDListFile or die "error opening $FIDListFile for writing: $!";

  foreach $FID (@FIDListNew)
  {
  	(my $myFid, my $myFname) = @$FID;
    print $fh "\#define $myFid\t".$i++." \/\/$myFname\n";
  }

  close $fh;
}

#Build file name to ID conversion hash
my %FileNameToID = ();
$i=1;
foreach $FID (@FIDListNew)
{
  (my $myFid, my $myFname) = @$FID;
  $FileNameToID{$myFid} = $i++;
}

foreach  $StringRef (@StringsList)
{
  (my $line_number, my $file_id, my $format_string) = @$StringRef;
	
  print $fhSCD "S $OriginID ".$FileNameToID{$file_id}." $line_number $format_string\n";
}

close $fhSCD;

exit 0;
