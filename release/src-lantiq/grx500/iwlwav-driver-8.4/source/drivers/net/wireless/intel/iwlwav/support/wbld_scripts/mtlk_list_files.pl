#
# $Id$
#
# This script generates file list section of VCPROJ file
# Syntax: mtlk_list_files.pl <source root dir> <config file path>
#
# The scripts reads VCPROJ template from standard input, looks for marker [!!!FILES!!!],
# replaces it with generated file list section and prints out the result to 
# standard output.
#
# The set of files added to XML output is controled by the means of text config file.
# Config file syntax:
#      Line started with # is a comment line, ignored by the script.
#      Line without # at the beginning is a rule definition line.
#        Rule definition lines have following format:
#          <Directive> <Perl regexp>
#        For each file in <source root dir> script obtains its relative to <source root dir>
#        name and matches it to given regexp. If file name matches regexp, script behaves according
#        to corresponding <Directive>.
#
#      Two directives are currently supported:
#        EXCL - exclude file from listing;
#        INCL - include file to listing.
#
#      EXCL directive has priority over INCL.
#      File that does not math no INCL nor EXCL rules is not listed.
#

use strict;
use File::Find;

my $debug = 0;
my $build_config_list = 1;

my @resultFileList = ();
my @includeRulesList = ();
my @excludeRulesList = ();
my @configurationsList = ();

my $search_root = shift || die "Argument missing: directory name\n";
my $config_file = shift || die "Argument missing: configuration file\n";
my $cfg_template = shift || die "Argument missing: configuration template\n";
my $cfg_list_template = shift || die "Argument missing: configuration list template\n";

log_print("VCPROJ file generator script started");

log_print("Reading config file...");

processConfigFile();
map { dbg_print("Include rule: $_\n"); } @includeRulesList;
map { dbg_print("Exclude rule: $_\n"); } @excludeRulesList;

log_print("DONE");

log_print("Building file list...");
loadFiles();
log_print("DONE");

if($build_config_list)
{
  log_print("Building configurations list...");
  loadConfigurations();
  log_print("DONE");
}

log_print("Generating VCPROJ XML...");

while (<STDIN>)
{
    my $line = $_;
    my $chomped_line = $line;
    chomp($chomped_line);

    my @tempResultFileList = @resultFileList;
    
    if(trim($chomped_line) eq "[!!!FILES!!!]")
      { dumpProjFileList("."); }
    elsif(trim($chomped_line) eq "[!!!FILES2010!!!]")
      { dumpProjFileList2010("."); }
    elsif(trim($chomped_line) eq "[!!!FILTER_FILES2010!!!]")
      { dumpProjFilterFileList2010("."); }
    elsif(trim($chomped_line) eq "[!!!FILTER_DEFS2010!!!]")
      { dumpProjFilterDefList2010("."); }
    elsif($build_config_list and (trim($chomped_line) eq "[!!!CONFIGURATIONS!!!]"))
      { dumpConfigurationsList($cfg_template); }
    elsif($build_config_list and (trim($chomped_line) eq "[!!!CONFIGURATIONS_LIST!!!]"))
      { dumpConfigurationsList($cfg_list_template); }        
    else
      { print $line; }
      
    @resultFileList = @tempResultFileList;
}

log_print("DONE");

log_print("VCPROJ file generator script executed successfully");

sub loadFiles()
{
  find({wanted => \&filterFile, no_chdir => 1},"$search_root"); #custom subroutine find, parse $search_root
}

sub loadConfigurations()
{
  find({wanted => \&filterEnvFile, no_chdir => 1},"$search_root/support"); #custom subroutine find, parse $search_root
}

# This function is called recursvely for each file found
# 
sub filterFile()
{
  my $file_name = $File::Find::name;

  #Do not include directories into the search results
  return if(-d $file_name);

  #Fix slash types
  $file_name =~ s/\//\\/g;

  #Replace root with .\
  my $search_root_regexp = path_to_regexp($search_root);
  dbg_print("Matching file \"$file_name\" to remove search root \"$search_root_regexp\"\n");
  $file_name =~ s/^$search_root_regexp(.*)/\.\\$1/;

  # Eliminate multiple slashes
  $file_name =~ s/\\+/\\/g;

  #Check exclude rules match
  map 
  {
    my $exclude_rule = $_;

    dbg_print("Matching file \"$file_name\" for exclude rule \"$exclude_rule\"\n");

    return if($file_name =~ /\.\\$exclude_rule/);

  } @excludeRulesList;

  #Check include rules match
  map 
  {
    my $include_rule = $_;

    dbg_print("Matching file \"$file_name\" for include rule \"$include_rule\"\n");

    if($file_name =~ /\.\\$include_rule/)
    {
       push @resultFileList, $file_name; 
       return;
    }
  } @includeRulesList;
}

sub filterEnvFile()
{
  my $file_name = $File::Find::name;

  #Do not include directories into the search results
  return if(-d $file_name);

  #Fix slash types
  $file_name =~ s/\//\\/g;

  #Filter out non-environment files
  if($file_name !~ /^.*\\[^\\]+\.env$/) { return; }

  #Build environment name based on configuration name
  $file_name !~ s/^.*\\([^\\]+)\.env$/$1/;

  push @configurationsList, $file_name;
}

sub processConfigFile()
{
  open(my $configFileHandle,$config_file) || die "Cannot open config file ($config_file) for reading: $!";
  my $config_line_number = 0;

  while (my $config_line=<$configFileHandle>)
  {
        chomp($config_line);
        $config_line = trim($config_line);
        $config_line_number = $config_line_number + 1;

        if (!$config_line)
        {
            dbg_print("Empty line $config_line_number skipped\n");
            next;
        }
        if ($config_line =~ /^\#.*/)
        {
            dbg_print("Comment line \"$config_line\" skipped\n");
            next;
        }
        if ($config_line =~ /^INCL\s+[^\s]+/)
        {
            dbg_print("Include line \"$config_line\" processed\n");
            $config_line =~ s/^INCL\s+([^\s]+)/$1/;
            dbg_print("Include rule \"$config_line\" processed\n");

            push @includeRulesList, $config_line;
            next;
        }
        if ($config_line =~ /^EXCL\s+[^\s]+/)
        {
            dbg_print("Exclude line \"$config_line\" processed\n");
            $config_line =~ s/^EXCL\s+([^\s]+)/$1/;
            dbg_print("Exclude rule \"$config_line\" processed\n");

            push @excludeRulesList, $config_line;
            next;
        }

        die "Unknown directive in $config_file:$config_line_number: \"$config_line\"";
    }
    close($config_file);
}

#
# This function dumps enumeration results with required XML tags inserted
sub dumpProjFileList($)
{ 
  my $curr_dir = shift;

  my $curr_dir_regexp = path_to_regexp($curr_dir);
  dbg_print("Processing results list directory \"$curr_dir\", regexp \"$curr_dir_regexp\"\n");
  dbg_print("Current results list @resultFileList\n");

  my @curr_dir_list = grep(/^$curr_dir_regexp\\.*/, @resultFileList);

  while(@curr_dir_list)
  {
    my $curr_entry_full = $curr_dir_list[0];
    my $curr_entry = $curr_dir_list[0];
    $curr_entry =~ s/^$curr_dir_regexp\\(.*)/$1/;

    dbg_print("Processing results list entry $curr_entry\n");

    if($curr_entry !~ m/\\/)
    {
       #This is a file, print it and remove from list

       print ("\t<File RelativePath=\"$curr_entry_full\"></File>\n");

       my $processedFileRegexp = path_to_regexp("$curr_dir\\$curr_entry");
       my @newFileList = grep(!/^$processedFileRegexp$/, @resultFileList);
       @resultFileList = @newFileList;
    }
    else
    {
       #This is a folder, print folder tag and go into recursion
       my $sub_dir = $curr_entry;
       $sub_dir =~ s/^(.*?)\\.*/$1/;

       print ("<Filter Name=\"$sub_dir\">\n");

       dumpProjFileList("$curr_dir\\$sub_dir");

       print ("</Filter>\n");
    }

    @curr_dir_list = grep(/^$curr_dir_regexp\\.*/, @resultFileList);
  }
}

# This function dumps enumeration results with required XML tags inserted
sub dumpProjFileList2010($)
{ 
  my $curr_dir = shift;

  my $curr_dir_regexp = path_to_regexp($curr_dir);
  dbg_print("Processing results list directory \"$curr_dir\", regexp \"$curr_dir_regexp\"\n");
  dbg_print("Current results list @resultFileList\n");

  my @curr_dir_list = grep(/^$curr_dir_regexp\\.*/, @resultFileList);

  while(@curr_dir_list)
  {
    my $curr_entry_full = $curr_dir_list[0];
    my $curr_entry = $curr_dir_list[0];
    $curr_entry =~ s/^$curr_dir_regexp\\(.*)/$1/;

    dbg_print("Processing results list entry $curr_entry\n");

    if($curr_entry !~ m/\\/)
    {
       #This is a file, print it and remove from list

       print ("\t<ClCompile Include=\"".trim_leading_dot($curr_entry_full)."\" />\n");
       
       my $processedFileRegexp = path_to_regexp("$curr_dir\\$curr_entry");
       my @newFileList = grep(!/^$processedFileRegexp$/, @resultFileList);
       @resultFileList = @newFileList;
    }
    else
    {
       #This is a folder, print folder tag and go into recursion
       my $sub_dir = $curr_entry;
       $sub_dir =~ s/^(.*?)\\.*/$1/;

       dumpProjFileList2010("$curr_dir\\$sub_dir");
    }

    @curr_dir_list = grep(/^$curr_dir_regexp\\.*/, @resultFileList);
  }
}

#
# This function dumps enumeration results with required XML tags inserted
sub dumpProjFilterFileList2010($)
{ 
  my $curr_dir = shift;

  my $curr_dir_regexp = path_to_regexp($curr_dir);
  dbg_print("Processing results list directory \"$curr_dir\", regexp \"$curr_dir_regexp\"\n");
  dbg_print("Current results list @resultFileList\n");

  my @curr_dir_list = grep(/^$curr_dir_regexp\\.*/, @resultFileList);

  while(@curr_dir_list)
  {
    my $curr_entry_full = $curr_dir_list[0];
    my $curr_entry = $curr_dir_list[0];
    $curr_entry =~ s/^$curr_dir_regexp\\(.*)/$1/;

    dbg_print("Processing results list entry $curr_entry\n");

    if($curr_entry !~ m/\\/)
    {
       #This is a file, print it and remove from list
       print ("\t<ClCompile Include=\"".trim_leading_dot($curr_entry_full)."\">\n");
       if ($curr_dir ne ".")
       {
         print ("\t\t<Filter>".trim_leading_dot($curr_dir)."</Filter>\n");
       }
       print ("\t</ClCompile>\n");
       
       my $processedFileRegexp = path_to_regexp("$curr_dir\\$curr_entry");
       my @newFileList = grep(!/^$processedFileRegexp$/, @resultFileList);
       @resultFileList = @newFileList;
    }
    else
    {
       #This is a folder, print folder tag and go into recursion
       my $sub_dir = $curr_entry;
       $sub_dir =~ s/^(.*?)\\.*/$1/;

       dumpProjFilterFileList2010("$curr_dir\\$sub_dir");
    }

    @curr_dir_list = grep(/^$curr_dir_regexp\\.*/, @resultFileList);
  }
}

#
# This function dumps enumeration results with required XML tags inserted
sub dumpProjFilterDefList2010($)
{ 
  my $curr_dir = shift;

  my $curr_dir_regexp = path_to_regexp($curr_dir);
  dbg_print("Processing results list directory \"$curr_dir\", regexp \"$curr_dir_regexp\"\n");
  dbg_print("Current results list @resultFileList\n");

  my @curr_dir_list = grep(/^$curr_dir_regexp\\.*/, @resultFileList);

  if ($curr_dir ne ".")
  {  
    print ("\t<Filter Include=\"".trim_leading_dot($curr_dir)."\">\n");
    print ("\t\t<UniqueIdentifier>". get_guid() . "</UniqueIdentifier>\n");
    print ("\t</Filter>\n");
  }
  
  while(@curr_dir_list)
  {
    my $curr_entry_full = $curr_dir_list[0];
    my $curr_entry = $curr_dir_list[0];
    $curr_entry =~ s/^$curr_dir_regexp\\(.*)/$1/;

    dbg_print("Processing results list entry $curr_entry\n");

    if($curr_entry !~ m/\\/)
    {
       my $processedFileRegexp = path_to_regexp("$curr_dir\\$curr_entry");
       my @newFileList = grep(!/^$processedFileRegexp$/, @resultFileList);
       @resultFileList = @newFileList;
    }
    else
    {
       #This is a folder, print folder tag and go into recursion
       my $sub_dir = $curr_entry;
       $sub_dir =~ s/^(.*?)\\.*/$1/;

       dumpProjFilterDefList2010("$curr_dir\\$sub_dir");
    }

    @curr_dir_list = grep(/^$curr_dir_regexp\\.*/, @resultFileList);
  }
}

my $guid_ctnr = 0;

# {6d398912-c7d0-4b06-a74b-98cbcdd40f3b}
sub get_guid($)
{
  return sprintf("{6d398912-c7d0-4b06-a74b-%04x%08x}", $guid_ctnr++, rand(0xFFFFFFFF));
}

sub trim_leading_dot($)
{
  my $path = shift;
  $path =~ s/^\.\\//;
  return $path;
}

#
# This function dumps configurations list
sub dumpConfigurationsList($)
{
  my $template = shift;
  my @tmpConfigurationsList = @configurationsList;
  foreach (@tmpConfigurationsList) {
    my $cfg_name=$_;
    open CFGTPL, "$template" or die $!;
    while (<CFGTPL>) {
      my $line=$_;
      $line =~ s/\[\!\!\!CGF_NAME\!\!\!\]/$cfg_name/g;
      print $line;
    }
    close CFGTPL;
  }
}

sub dbg_print($)
{
    print(shift) if($debug);
}

sub log_print($)
{
   my $msg = shift;
   (my $sec, my $min, my $hour, my $mday, my $mon, my $year, my $wday, my $yday, my $isdst)=localtime(time);
   printf(STDERR "[%4d-%02d-%02d %02d:%02d:%02d]\t".$msg."\n", $year+1900,$mon+1,$mday,$hour,$min,$sec);
}

sub trim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/[\s\r]+$//;
    return $string;
}

sub path_to_regexp($)
{
  my $path = shift;
  $path =~ s/([\:\\\/\.\,\[\]])/\\$1/g;
  return $path;
}
