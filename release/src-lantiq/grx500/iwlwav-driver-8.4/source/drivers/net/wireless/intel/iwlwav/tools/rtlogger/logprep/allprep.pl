#!/usr/bin/perl -w
#
# Metalink logger preprocessor script
#
# Usage: perl logprep.pl <Origin ID> <Origin Name> <GID list header file> <macro database header file> <macro database source file> <scd file name>
#
# $Id$
#

use English;

my $debug = 0;

# These falues are for maximum length checks
# and must be synchronized with corresponding
# definitions in logdefs.h
my $MAX_LID_VALUE   = (1 << 14) - 1;
my $MAX_FID_VALUE   = (1 <<  5) - 1;
my $MAX_DSIZE_VALUE = (1 << 11) - 1;
my $MAX_WLAN_IF_VALUE = (1 << 2) - 1;
my $MAX_GID_VALUE   = (1 <<  7) - 1;
my $MAX_OID_VALUE   = (1 <<  5) - 1;

# This is a special OID value for 
# components that does not have remote logging support
my $NO_OID_VALUE    = 255;

# When it comes to multiline macros that use __LINE__
# macro inside there is a difference in preprocessors behaviour
# Some of them (like one used in GCC) substitute __LINE__ with number
# of the last line occupied by macro, others, like one used in Green Hils MULTI
# substitute it with number of the first line occupied by macro
# This variable changes behaviour of the script accordingly.
my $multiline_macro_binding_first_line = 0;

my $HeaderData="";
my $SourceData="";
my @GIDListOrig=();
my @GIDNamesList=();
my @StringsList=();
my @MessageList=();

my $curr_group_name;
my $curr_file_id;

my $usage_text = <<USAGE_END;
The command line is incorrect.
Usage:
    perl allprep.pl <Compiler> <Origin ID> <Origin Name> <GID list header file> <macro database header file> <macro database source file> <scd file name> <org_file_name>
    Compilers supported:
        1. gcc
        2. mutli

USAGE_END

($#ARGV + 1) == 8 or die($usage_text);

(my $TargetCompiler, my $OriginID, my $OriginName, my $GIDListFile, my $MacroHeaderFile, my $MacroSourceFile, my $SCDFile, my $org_file_name) = @ARGV;

if(($OriginID > $MAX_OID_VALUE) && ($OriginID != $NO_OID_VALUE))
{
  print STDERR "Origin ID is too big ($OriginID > $MAX_OID_VALUE)\n";
  exit 1;
}

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

sub generate_param_lists
{
  my $macro_suffix = shift;

  %type_by_format_char = (S => "const char *",
                          D => "int32",
                          C => "int8",
                          P => "const void *",
                          H => "int64",
                          Y => "const void *",
                          K => "const void *");

  %packer_by_format_char = (S => "LOGPKT_PUT_STRING",
                            D => "LOGPKT_PUT_INT32",
                            C => "LOGPKT_PUT_INT8",
                            P => "LOGPKT_PUT_PTR",
                            H => "LOGPKT_PUT_INT64",
                            Y => "LOGPKT_PUT_MACADDR",
                            K => "LOGPKT_PUT_IP6ADDR");

  %size_by_format_char = (S => "LOGPKT_STRING_SIZE",
                          D => "LOGPKT_SCALAR_SIZE",
                          C => "LOGPKT_SCALAR_SIZE",
                          P => "LOGPKT_SCALAR_SIZE",
                          H => "LOGPKT_SCALAR_SIZE",
                          Y => "LOGPKT_MACADDR_SIZE",
                          K => "LOGPKT_IP6ADDR_SIZE");

  $macro_suffix =~ s/V//g;

  my $macro_params_list = $macro_suffix;
  $macro_params_list =~ s/(.)/", ".lc($1).(pos($macro_params_list) + 1)/ge;

  my $func_params_list = $macro_suffix;
  $func_params_list =~ s/(.)/", ".$type_by_format_char{$1}." ".lc($1).(pos($func_params_list) + 1)/ge;

  my $pass_params_list = $macro_suffix;
  $pass_params_list =~ s/(.)/", (".$type_by_format_char{$1}.") (".lc($1).(pos($pass_params_list) + 1).")"/ge;

  my $total_params_size = $macro_suffix;
  $total_params_size =~ s/(.)/"\n                       + ".$size_by_format_char{$1}."(".lc($1).(pos($total_params_size) + 1).")"/ge;

  my $pack_params_code = $macro_suffix;
  $pack_params_code =~ s/(.)/"      ".$packer_by_format_char{$1}."(".lc($1).(pos($pack_params_code) + 1).");\n"/ge;

  my $str_length_calc = $macro_suffix;
  $str_length_calc =~ s/[^S]/@/g;
  $str_length_calc =~ s/(S)/"    size_t ".lc($1).(pos($str_length_calc) + 1)."len__ = strlen(".lc($1).(pos($str_length_calc) + 1).") + 1;\n"/ge;
  $str_length_calc =~ s/@//g;

  return ($macro_params_list, $func_params_list, $pass_params_list, $total_params_size,
          $pack_params_code, $str_length_calc);
}

sub generate_code
{
  my $macro_name = shift;
  my $log_level = shift;
  my $macro_suffix = shift;

  (my $macro_params_list, my $func_params_list, my $pass_params_list, my $total_params_size,
   my $pack_params_code, my $str_length_calc) =
       generate_param_lists($macro_suffix);

  my $console_printout;

  if($log_level eq "IWLWAV_RTLOG_ERROR_DLEVEL") {
    $console_printout = "CERROR(fname, lid$pass_params_list);";
  } elsif($log_level eq "IWLWAV_RTLOG_WARNING_DLEVEL") {
    $console_printout = "CWARNING(fname, lid$pass_params_list);";
  } else {
    $console_printout = "CLOG(fname, lid, $log_level$pass_params_list);";
  }

  my $choppedMacroName = substr($macro_name, 0, -1);
  my $num = length($macro_suffix);
  
  my $header = <<END_OF_HEADER;
#if (IWLWAV_RTLOG_MAX_DLEVEL < $log_level)
#define $macro_name\_$macro_suffix(fmt$macro_params_list)
#else
__MTLK_FLOG void
__$macro_name\_$macro_suffix\_$OriginID(uint8 gid, uint8 fid, uint16 lid$func_params_list);

#define $macro_name\_$macro_suffix(fmt$macro_params_list) \\
  __$macro_name\_$macro_suffix\_$OriginID(LOG_LOCAL_GID, LOG_LOCAL_FID, __LINE__$pass_params_list)
#endif /* IWLWAV_RTLOG_MAX_DLEVEL < $log_level */
END_OF_HEADER

my $body;

  if($macro_suffix eq "V")
  {
  $body = <<END_OF_BODY;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= $log_level)
__MTLK_FLOG void
__$macro_name\_$macro_suffix\_$OriginID(uint8 gid, uint8 fid, uint16 lid$func_params_list)
{
	logMacros\_$choppedMacroName(gid, fid, lid, $log_level, 0, NULL);
}
#endif /* IWLWAV_RTLOG_MAX_DLEVEL >= $log_level */
END_OF_BODY
  }
  else
  {
  my $macro_param_array = $macro_suffix;
  $macro_param_array =~ s/(.)/"\tparamArray[".pos($macro_param_array)."] = ".lc($1).(pos($macro_param_array) + 1).";\n"/ge;
  
  $body = <<END_OF_BODY;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= $log_level)
__MTLK_FLOG void
__$macro_name\_$macro_suffix\_$OriginID(uint8 gid, uint8 fid, uint16 lid$func_params_list)
{
	int32 paramArray[$num];
	
$macro_param_array
	logMacros\_$choppedMacroName(gid, fid, lid, $log_level, $num, paramArray);
}
#endif /* IWLWAV_RTLOG_MAX_DLEVEL >= $log_level */
END_OF_BODY
  }
  
  return ($header, $body);
}

sub generate_code_signal
{
  my $macro_name = shift;
  my $log_level = shift;
  my $message_type = shift;

  (my $macro_params_list, my $func_params_list, my $pass_params_list, my $total_params_size,
   my $pack_params_code, my $str_length_calc) =
       generate_param_lists("");
  
  $macro_params_list = "src_task_id, dst_task_id, message_type, message";
  $func_params_list = "uint16 src_task_id, uint16 dst_task_id, uint16 message_size, char *message";
  $pass_params_list = "src_task_id, dst_task_id, sizeof(*message), (char *)message";
  my $pass_params_list2 = "src_task_id, dst_task_id, message_size, (char *)message";

  my $choppedMacroName = substr($macro_name, 0, -1);
  
  my $header = <<END_OF_HEADER;
#if (IWLWAV_RTLOG_MAX_DLEVEL < $log_level)
#define $macro_name($macro_params_list)
#else
__MTLK_FLOG void
__$macro_name\_$OriginID(uint8 gid, uint8 fid, uint16 lid, $func_params_list);

#define $macro_name($macro_params_list) \\
  __$macro_name\_$OriginID(LOG_LOCAL_GID, LOG_LOCAL_FID, __LINE__, \\
               $pass_params_list)
#endif /* IWLWAV_RTLOG_MAX_DLEVEL < $log_level */
END_OF_HEADER

  my $body = <<END_OF_BODY;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= $log_level)
__MTLK_FLOG void
__$macro_name\_$OriginID(uint8 gid, uint8 fid, uint16 lid, $func_params_list)
{
	logMacros\_$choppedMacroName(gid, fid, lid, $log_level, $pass_params_list2);
}
#endif /* IWLWAV_RTLOG_MAX_DLEVEL >= $log_level */
END_OF_BODY

  return ($header, $body);
}

# TLOG macro - header and body
sub generate_code_ctype
{
  my $macro_name = shift;
  my $log_level  = shift;

  my $toCfile = "uint16 src_task_id, uint16 dst_task_id, uint8 message_type, uint16 message_code, uint16 message_size, char *message";
  my $pass1 = "message->header.sFrom.taskID, message->header.sTo.taskID, message_type"; 		# send "From", "To" and message type
  my $pass2 = "message->header.tKMsgType, message_size, (char *)message->abData"; 	# send message enum, message size and message content
  
  my $header = <<END_OF_HEADER;
#if (IWLWAV_RTLOG_MAX_DLEVEL < $log_level)
#define $macro_name(message, message_type, message_size)
#else
__MTLK_FLOG void
__$macro_name\_$OriginID($toCfile);

#define $macro_name(message, message_type, message_size) \\
  __$macro_name\_$OriginID($pass1, $pass2);
#endif /* IWLWAV_RTLOG_MAX_DLEVEL < $log_level */
END_OF_HEADER

  my $body = <<END_OF_BODY;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= $log_level)
__MTLK_FLOG void
__$macro_name\_$OriginID($toCfile)
{
#if (IWLWAV_RTLOG_FLAGS & (RTLF_REMOTE_ENABLED | RTLF_CONSOLE_ENABLED))
  int flags__ = mtlk_log_get_flags(0, 1, 0);
#endif
#if (IWLWAV_RTLOG_FLAGS & RTLF_REMOTE_ENABLED)
  if ((flags__ & LOG_TARGET_REMOTE) != 0)
  {
    size_t datalen__ = LOGPKT_TLOG_SIZE(message_size);
    mtlk_log_TLOG_t mtlk_log_TLOG;
    mtlk_log_buf_entry_t *pbuf__;
    TX_INTERRUPT_SAVE_AREA;
    
    OSAL_DISABLE_INTERRUPTS(&interrupt_save);
    
    pbuf__ = logReserveBuffer(datalen__);
    
    if (pbuf__ != NULL)
    {
      uint8 *p__ = (uint8*)pbuf__;

	  logFillLogEvent(1, 0, message_type, message_code, datalen__, &p__);

      mtlk_log_TLOG.src_task_id = src_task_id;
      mtlk_log_TLOG.dst_task_id = dst_task_id;
      mtlk_log_TLOG.len = message_size;
      
      LOGPKT_PUT_TLOG(mtlk_log_TLOG, message);

      logReleaseBuffer(pbuf__, datalen__);
    }
   OSAL_ENABLE_INTERRUPTS(interrupt_save);
  }
#endif
}
#endif /* IWLWAV_RTLOG_MAX_DLEVEL >= $log_level */
END_OF_BODY

  return ($header, $body);
}

sub get_macro_suffix_string
{
  my $string = shift;
  my $packed_format = "";

  dbg_print "The format string is ===========================\n$string\n=============================\n";

  $string =~ s/
                (?: (?<!%)(?:%%)*%(?!%) )    #Match odd number of percent sign repetitions only
                (?:
                  .*?                        #flags, width, precision
                  (l*)                       #length
                  ([cduixXspBYK])            #Format letter
                )
              /
                my $format_symbol;
                my $length=$1;
                my $format_letter=$2;

                if($format_letter =~ \/[diuxX]\/)     { $format_symbol = (length($length) < 2) ? "D" : "H"; }
                elsif($format_letter =~ \/[B]\/)      { $format_symbol = "D"; }
                else                                  { $format_symbol = uc($format_letter); }

                dbg_print "===length: $length===format_letter: $format_letter===format_symbol: $format_symbol===\n";
                $packed_format = $packed_format.$format_symbol;
              /xeg;

  $packed_format = "V" if(not $packed_format);
  dbg_print "The packed format string is $packed_format ===========================\n";

  return $packed_format;
}

sub is_curr_file_id
{
  my $group_name = shift;
  my $file_id = shift;

  return (($group_name eq $curr_group_name) and ($file_id == $curr_file_id));
}

sub clear_current_file_strings
{
  return if not defined $curr_group_name or not defined $curr_file_id;

  my @NewStringList;

  foreach  $StringRef (@StringsList)
  {
    (my $line_number, my $group_name, my $file_id, my $format_string) = @$StringRef;
    push @NewStringList, [ ($line_number, $group_name, $file_id, $format_string) ] if not is_curr_file_id($group_name, $file_id);
  }

  @StringsList = @NewStringList;

  my @NewMessageList;

  foreach  $MessageRef (@MessageList)
  {
    (my $line_number, my $group_name, my $file_id, my $type_name) = @$MessageRef;
    push @NewMessageList, [ ($line_number, $group_name, $file_id, $type_name) ] if not is_curr_file_id($group_name, $file_id);
  }

  @MessageList = @NewMessageList;
}

sub process_group_definition
{
  my $group_name = shift;

  push(@GIDNamesList, $group_name);
  $curr_group_name = $group_name;
  clear_current_file_strings();

  return $group_name;
}

sub process_file_definition
{
  $curr_file_id = shift;

  if($curr_file_id > $MAX_FID_VALUE)
  {
    print STDERR "File ID is too big ($curr_file_id > $MAX_FID_VALUE)\n";
    exit 1;
  }

  clear_current_file_strings();

  return $curr_file_id;
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

  $log_level = "IWLWAV_RTLOG_ERROR_DLEVEL" if $macro =~ /^ELOG.*/;
  $log_level = "IWLWAV_RTLOG_WARNING_DLEVEL" if $macro =~ /^WLOG.*/;

  if (not defined $log_level)
  {
    #If log level not specified then this is not a printout we recognize
    #do nothing and keep the code as is.
    return $macro.$text_before_format_string.$format_string.$text_after_format_string;
  }

  dbg_print "The line is ===========================\n$line\n=============================\n";
  dbg_print "=============================\n";
  dbg_print "macro is                        ====".$macro."====\n";
  dbg_print "log level is                    ====".$log_level."====\n";
  dbg_print "text before format string is    ====".$text_before_format_string."====\n";
  dbg_print "format string is                ====".$format_string."====\n";
  dbg_print "text after format string is     ====".$text_after_format_string."====\n";
  dbg_print "=============================\n";

  my $macro_suffix = get_macro_suffix_string($format_string);

  #Eliminate escaped endlines and merge chunks of
  #the format string and put it into the strings database
  my $SCDFormatString = $format_string;
  $SCDFormatString =~ s/\\\n//gm;
  $SCDFormatString =~ s/\"[\s\n]*\"//gm;

  if (not defined $curr_group_name)
  {
    print STDERR "No group name definition before printout at line $line_number\n";
    exit 1;
  }

  if (not defined $curr_file_id)
  {
    print STDERR "No file ID definition before printout at line $line_number\n";
    exit 1;
  }

  if($line_number > $MAX_LID_VALUE)
  {
    print STDERR "Line ID is too big ($line_number > $MAX_LID_VALUE)\n";
    exit 1;
  }

  push @StringsList, [ ($line_number, $curr_group_name, $curr_file_id, $SCDFormatString) ];

  #Generate the macro and put it to code database
  (my $header, my $body) = generate_code($macro, $log_level, $macro_suffix);
  if( -1 == index($HeaderData, " $macro\_$macro_suffix(") )
  {
    $HeaderData .= $header."\n\n";
    $SourceData .= $body."\n\n";
  }

  return $macro."_".$macro_suffix.$text_before_format_string.$format_string.$text_after_format_string;
}

sub process_fatal_command
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

  if (not defined $curr_group_name)
  {
    print STDERR "No group name definition before printout at line $line_number\n";
    exit 1;
  }

  if (not defined $curr_file_id)
  {
    print STDERR "No file ID definition before printout at line $line_number\n";
    exit 1;
  }

  if($line_number > $MAX_LID_VALUE)
  {
    print STDERR "Line ID is too big ($line_number > $MAX_LID_VALUE)\n";
    exit 1;
  }

  push @StringsList, [ ($line_number, $curr_group_name, $curr_file_id, 'Fatal Error in file: "'.$org_file_name.'"; Fatal Print: '.$SCDFormatString) ];
  
  return $macro.$text_before_format_string.$format_string.$text_after_format_string;
}

sub process_assert_command
{
  my $line_number = shift;

  if (not defined $curr_group_name)
  {
    print STDERR "No group name definition before ASSERT at line $line_number\n";
    exit 1;
  }

  if (not defined $curr_file_id)
  {
    print STDERR "No file ID definition before ASSERT at line $line_number\n";
    exit 1;
  }

  if($line_number > $MAX_LID_VALUE)
  {
    print STDERR "Line ID is too big ($line_number > $MAX_LID_VALUE)\n";
    exit 1;
  }

  push @StringsList, [ ($line_number, $curr_group_name, $curr_file_id, 'Assert in file: "'.$org_file_name.'" in line: '.$line_number) ];
  
  #return "ASSERT";
}

sub process_signal_command
{
  my $line_number = shift;
  my $macro       = shift;
  my $log_level   = shift;
  my $src_task_id = shift;
  my $dst_task_id = shift;
  my $message_type = shift;
  my $rest = shift;

  if (not defined $log_level)
  {
    #If log level not specified then this is not a printout we recognize
    #do nothing and keep the code as is.
      return $macro."(".$src_task_id.", ".$dst_task_id.", ".$message_type.", ".$rest;
  }

  my $macro_suffix = "DDZB";#get_macro_suffix_string($format_string);

  if (not defined $curr_group_name)
  {
    print STDERR "No group name definition before SLOG at line $line_number\n";
    exit 1;
  }

  if (not defined $curr_file_id)
  {
    print STDERR "No file ID definition before SLOG at line $line_number\n";
    exit 1;
  }

  if($line_number > $MAX_LID_VALUE)
  {
    print STDERR "Line ID is too big ($line_number > $MAX_LID_VALUE)\n";
    exit 1;
  }

  push @MessageList, [ ($line_number, $curr_group_name, $curr_file_id, $message_type) ];
  
  #Generate the macro and put it to code database
  (my $header, my $body) = generate_code_signal($macro, $log_level, $message_type);
  if( -1 == index($HeaderData, " $macro(") )
  {
    $HeaderData .= $header."\n\n";
    $SourceData .= $body."\n\n";
  }

  return $macro."(".$src_task_id.", ".$dst_task_id.", ".$message_type.", ".$rest;
}

# TLOG
sub process_ctype_command
{
  my $macro     = shift;
  my $log_level = shift; 
  my $rest 		= shift;
  my $message_type = shift;
  my $line_number = shift;
  
  if (not defined $log_level)
  {
    #If log level not specified then this is not a printout we recognize
    #do nothing and keep the code as is.
      return $macro."(".$message_type.$rest;
  }
  
  #Generate the macro and put it to code database
  (my $header, my $body) = generate_code_ctype($macro, $log_level);
  if( -1 == index($HeaderData, " $macro(") )
  {
    $HeaderData .= $header."\n\n";
    $SourceData .= $body."\n\n";
  }

  return $macro."(".$message_type.$rest;
}

#Read GID list and build number to name conversion hash
my %GroupIDToName = ();
if(-e $GIDListFile)
{
  open my $fh, '<', $GIDListFile or die "error opening $GIDListFile for reading: $!";
  local $INPUT_RECORD_SEPARATOR="\n";
  while (<$fh>)
  {
      die "Incorrect GID list entry $_" if(not s/^\#define\s+(\w+)\s+(\w+).*$/push(@GIDNamesList, $1) and ($GroupIDToName{$2} = $1)/eg);
  }
  @GIDListOrig = @GIDNamesList;
}

#Read headers database
#if(-e $MacroHeaderFile)
#{
#  open my $fh, '<', $MacroHeaderFile or die "error opening $MacroHeaderFile for reading: $!";
#  $HeaderData = do { local $/; <$fh> };
#}

#Read source database
#if(-e $MacroSourceFile)
#{
#  open my $fh, '<', $MacroSourceFile or die "error opening $MacroSourceFile for reading: $!";
#  $SourceData = do { local $/; <$fh> };
#}

#Read string list
if(-e $SCDFile)
{
  open my $fh, '<', $SCDFile or die "error opening $SCDFile for reading: $!";
  while (<$fh>)
  {
    s/^S\s+(\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(.*)$
     /push(@StringsList, [ ($4, $GroupIDToName{$2}, $3, $5) ])/egx
  }
}

#Read message list
if(-e $SCDFile)
{
  open my $fh, '<', $SCDFile or die "error opening $SCDFile for reading: $!";
  while (<$fh>)
  {
    s/^T\s+(\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(.*)$
     /push(@MessageList, [ ($4, $GroupIDToName{$2}, $3, $5) ])/egx
  }
}

#$INPUT_RECORD_SEPARATOR=";";
#my $CompleteMacroNameRe = '((?<!\w)[IWE]LOG|ASSERT|FATAL([0-9]){0,1})(?: _[A-Z]+){0,1}';
my $FatalMacroNameRe 	= '((?<!\w)FATAL([0-9]){0,1})(?: _[A-Z]+){0,1}';
my $DebugFatalMacroNameRe 	= '((?<!\w)DEBUG_FATAL([0-9]){0,1})(?: _[A-Z]+){0,1}';
my $AssertMacroNameRe 	= '(?<!\w)ASSERT';
my $DebugAssertMacroNameRe 	= '(?<!\w)DEBUG_ASSERT';
my $LogMacroNameRe 		= '((?<!\w)[IWE]LOG([0-9]){0,1})(?: _[A-Z]+){0,1}';
my $SignalMacroNameRe	= '((?<!\w)SLOG([0-9]){0,1})';
my $CurrentLineNumber 	= 1;
my $last_log_line;
my $last_log_orig_line;
my $last_log_line_didnt_end_with_semi_comma = 0;
my $CtypeMacroNameRe	= '((?<!\w)TLOG_Sender([0-9]){0,1})'; # TLOG format

while (<STDIN>)
{
    my $line=$_;
    my $orig_line = $line;

    #If chunk contains printout - get number of lines before printout
    #This number will be used to determine line number of the printout macro
    my $LinesBeforePrintout = 0;
    if($line =~ /.*$FatalMacroNameRe(.|\n)*/)
    {
      if($multiline_macro_binding_first_line)
      {
          $line =~ s/$FatalMacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
    }
	if($line =~ /.*$DebugFatalMacroNameRe(.|\n)*/)
    {
      if($multiline_macro_binding_first_line)
      {
          $line =~ s/$DebugFatalMacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
    }
	elsif($line =~ /.*$DebugAssertMacroNameRe(.|\n)*/)
	{
	  if($multiline_macro_binding_first_line)
      {
          $line =~ s/$DebugAssertMacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
    }
    elsif($line =~ /.*$AssertMacroNameRe(.|\n)*/)
    {
      if($multiline_macro_binding_first_line)
      {
          $line =~ s/$AssertMacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
    }
    elsif($line =~ /.*$LogMacroNameRe(.|\n)*/)
    {
      if($multiline_macro_binding_first_line)
      {
          $line =~ s/$LogMacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
    }
    elsif($line =~ /.*$SignalMacroNameRe(.|\n)*/)
    {
      if($multiline_macro_binding_first_line)
      {
          $line =~ s/$SignalMacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
	}
	elsif($line =~ /.*$CtypeMacroNameRe(.|\n)*/) # TLOG
    {
      if($multiline_macro_binding_first_line)
      {
          $line =~ s/$CtypeMacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
    }

    #Find group IDS
    $line =~ s/(LOG_LOCAL_GID\s+)(\w+)/$1.process_group_definition($2)/xemg;
    $line =~ s/(LOG_LOCAL_FID\s+)(\w+)/$1.process_file_definition($2)/xemg;

    #Process printouts
	if($last_log_line_didnt_end_with_semi_comma)
	{
		if($line =~ /.*;.*/)
		{
			$last_log_line =~ s/
				$LogMacroNameRe									#Macro name
				((?: [^"]|\n)*")                                #Text between macro name and beginning of the format string
				((?: .|\n)*?)                                   #Format string
				((?<!\\)"\s*[\,\)](?: .|\n)*)                   #Text after format string
				/process_print_command($last_log_orig_line, $CurrentLineNumber + $LinesBeforePrintout, $1, $2, $3, $4, $5)/xemg;
			$last_log_line_didnt_end_with_semi_comma = 0;
		}
	}
	elsif($line =~ /\A\s*$LogMacroNameRe(.|\n)*/)
	{
		$line =~ s/
			$LogMacroNameRe									#Macro name
			((?: [^"]|\n)*")                                #Text between macro name and beginning of the format string
			((?: .|\n)*?)                                   #Format string
			((?<!\\)"\s*[\,\)](?: .|\n)*)                   #Text after format string
			/process_print_command($orig_line, $CurrentLineNumber + $LinesBeforePrintout, $1, $2, $3, $4, $5)/xemg;
		
		#if log is found and line doesn't end in ; it means that this log is multiline
		#save log line and all inputs for later use
		if($line =~ /.*;.*/)
		{
			$last_log_line_didnt_end_with_semi_comma = 0;
		}
		else
		{
			$last_log_line = $line;
			$last_log_orig_line = $orig_line;
			$last_log_line_didnt_end_with_semi_comma = 1;
		}
	}
	elsif($line =~ /\A\s*$DebugAssertMacroNameRe(\s*)(\(+)/)
	{
		#$line =~ s/
		#	$DebugAssertMacroNameRe                              #Macro name
		#	/process_assert_command($CurrentLineNumber + $LinesBeforePrintout)/xemg;
		process_assert_command($CurrentLineNumber + $LinesBeforePrintout);
	}
	elsif($line =~ /\A\s*$AssertMacroNameRe(\s*)(\(+)/)
	{
		#$line =~ s/
		#	$AssertMacroNameRe                              #Macro name
		#	/process_assert_command($CurrentLineNumber + $LinesBeforePrintout)/xemg;
		process_assert_command($CurrentLineNumber + $LinesBeforePrintout);
	}
	elsif($line =~ /\A\s*$FatalMacroNameRe(.|\n)*/)
	{
		$line =~ s/
			$FatalMacroNameRe							#Macro name
			((?: [^"]|\n)*")                                #Text between macro name and beginning of the format string
			((?: .|\n)*?)                                   #Format string
			((?<!\\)"\s*[\,\)](?: .|\n)*)                   #Text after format string
			/process_fatal_command($orig_line, $CurrentLineNumber + $LinesBeforePrintout, $1, $2, $3, $4, $5)/xemg;
	}
	elsif($line =~ /\A\s*$DebugFatalMacroNameRe(.|\n)*/)
	{
			$line =~ s/
			$DebugFatalMacroNameRe							#Macro name
			((?: [^"]|\n)*")                                #Text between macro name and beginning of the format string
			((?: .|\n)*?)                                   #Format string
			((?<!\\)"\s*[\,\)](?: .|\n)*)                   #Text after format string
			/process_fatal_command($orig_line, $CurrentLineNumber + $LinesBeforePrintout, $1, $2, $3, $4, $5)/xemg;
	}
	elsif($line =~ /\A\s*$SignalMacroNameRe(.|\n)*/)
	{
		$line =~ s/
			$SignalMacroNameRe  #Macro name
      (?:\s*[\(]\s*)      #Text between macro name and first parameter
      ([\w]+)             #Task id of source
      (?:\s*[\,]\s*)      #Comma
      ([\w]+)             #Task id of destination
      (?:\s*[\,]\s*)      #Comma
      ([\w]+)             #Message type
      (?:\s*[\,]\s*)      #Comma
      (.*)                #The rest
			/process_signal_command($CurrentLineNumber + $LinesBeforePrintout, $1, $2, $3, $4, $5, $6)/xemg;
	}

# TLOG
	elsif($line =~ /\A\s*$CtypeMacroNameRe(.|\n)*/)
	{
		$line =~ s/
			$CtypeMacroNameRe  #Macro name (TLOG)
      (?:\s*[\(]\s*)      # Text between macro name and first parameter [ ( ]
      ([\w]+)             # K-MSG variable
      (.*)                # The rest [ ); ]
			/process_ctype_command($1, $2, $4, $3, $CurrentLineNumber + $LinesBeforePrintout)/xemg;
	}
	$CurrentLineNumber += $line =~ s/\n/\n/mg;

    print $line;
}

open my $fhSCD, '>', $SCDFile or die "error opening $SCDFile for writing: $!";
print $fhSCD "O $OriginID $OriginName\n";

#Flush GID list into both C header and SCD file
my $i=1;
my @GIDListNew = ();
my %seen = ();

foreach $GID (@GIDNamesList)
{
  unless ($seen{$GID}++)
  {
    push(@GIDListNew, $GID);

    if($i > $MAX_GID_VALUE)
    {
      print STDERR "Group ID is too big for $GID ($i > $MAX_GID_VALUE)\n";
      exit 1;
    }

    print $fhSCD "G $OriginID ".$i++." $GID\n";
  }
}

#If new GIDs were added we have to rebuild the GID list file
if($#GIDListNew != $#GIDListOrig)
{
  $i=1;
  open my $fh, '>', $GIDListFile or die "error opening $GIDListFile for writing: $!";

  foreach $GID (@GIDListNew)
  {
    print $fh "\#define $GID\t".$i++."\n";
  }

  close $fh;
}

#Build group name to ID conversion hash
my %GroupNameToID = ();
$i=1;
foreach $GID (@GIDListNew)
{
  $GroupNameToID{$GID} = $i++;
}

foreach  $StringRef (@StringsList)
{
  (my $line_number, my $group_id, my $file_id, my $format_string) = @$StringRef;
  print $fhSCD "S $OriginID ".$GroupNameToID{$group_id}." $file_id "."$line_number $format_string\n";
}

foreach  $MessageRef (@MessageList)
{
  (my $line_number, my $group_id, my $file_id, my $format_string) = @$MessageRef;
  print $fhSCD "T $OriginID ".$GroupNameToID{$group_id}." $file_id "."$line_number $format_string\n";
}

close $fhSCD;

#Flush headers database
#open $fh, '>', $MacroHeaderFile or die "error opening $MacroHeaderFile for writing: $!";
#print $fh $HeaderData;
#close $fh;

#Flush source database
#open $fh, '>', $MacroSourceFile or die "error opening $MacroSourceFile for writing: $!";
#print $fh $SourceData;
#close $fh;

exit 0;
