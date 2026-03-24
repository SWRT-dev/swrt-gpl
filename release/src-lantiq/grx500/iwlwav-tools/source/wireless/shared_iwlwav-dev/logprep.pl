#!/usr/bin/perl -w
#
#/******************************************************************************
#
#         Copyright (c) 2020, MaxLinear, Inc.
#         Copyright 2016 - 2020 Intel Corporation
#         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
#         Copyright 2009 - 2014 Lantiq Deutschland GmbH
#         Copyright 2007 - 2008 Infineon Technologies AG
#
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
#
#*******************************************************************************/

# logger preprocessor script
#
# Usage: perl logprep.pl <Origin ID> <Origin Name> <GID list header file> <macro database header file> <macro database source file> <scd file name>
#
# $Id$
#

use English;
use Fcntl qw(:flock SEEK_END);
use File::Basename;
use File::Copy;

my $debug = 0;

# Set to 1 to generate GID list automaticaly
# Set to 0 to don't modify a ready GID list
my $auto_generate_gid_list = 0;

# These falues are for maximum length checks
# and must be synchronized with corresponding
# definitions in logdefs.h
my $MAX_LID_VALUE   = (1 << 14) - 1;
my $MAX_FID_VALUE   = (1 <<  5) - 1;
my $MAX_DSIZE_VALUE = (1 << 11) - 1;
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

my $ErrorFlag = 0;
my $InputFileName;
my $fhInput;

my $HeaderData="";
my $SourceData="";
my $UpdateFlag=0;
my @GIDListOrig=();
my @GIDNamesList=();
my @StringsList=();
my @MessageList=();

my %GroupIDToName = ();
my %GroupNameToID = ();
my %GroupSeen = ();

my $curr_group_name;
my $curr_file_id;

my $usage_text = <<USAGE_END;
The command line is incorrect.
Usage:
    perl logprep.pl <Compiler> <Origin ID> <Origin Name> <GID list header file> <macro database header file> <macro database source file> <scd file name> <elf file name> [input file name]
    Compilers supported:
        1. gcc
        2. mutli

USAGE_END

($#ARGV + 1) >= 8 or die($usage_text);

(my $TargetCompiler, my $OriginID, my $OriginName, my $GIDListFile, my $MacroHeaderFile, my $MacroSourceFile, my $SCDFile, my $ELFFile, $InputFileName) = @ARGV;
my $GIDListDir = dirname($GIDListFile);

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

sub open_input_file
{
  my $fname = shift;
  open my $fh, '<', $fname or die "error opening file $fname for reading: $!";
  return $fh;
}

sub print_err_msg
{
  my $fname = $InputFileName;
  my $lnum = shift;
  my $msg = shift;
  print STDERR "$fname:$lnum: error: $msg\n";
}

sub log_fatal
{
  print_err_msg(@_);
  exit 1;
}

sub log_error
{
  print_err_msg(@_);
  $ErrorFlag = 1;
}

sub dbg_print($)
{
    print STDERR shift if($debug);
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
    $console_printout = "CERROR(fname, lid, fmt$pass_params_list);";
  } elsif($log_level eq "IWLWAV_RTLOG_WARNING_DLEVEL") {
    $console_printout = "CWARNING(fname, lid, fmt$pass_params_list);";
  } elsif($log_level eq "IWLWAV_RTLOG_RELEASE_DLEVEL") {
    $console_printout = "CLOG(fname, lid, 0, fmt$pass_params_list);";
  } else {
    $console_printout = "CLOG(fname, lid, $log_level, fmt$pass_params_list);";
  }

  my $header = <<END_OF_HEADER;
#if (IWLWAV_RTLOG_MAX_DLEVEL < $log_level)
#define $macro_name\_$macro_suffix(fmt$macro_params_list)
#else

__MTLK_FLOG void
__$macro_name\_$macro_suffix\_$OriginID(const char *fname, int level, uint8 oid, uint8 gid, uint8 fid, uint16 lid, const char *fmt$func_params_list);

#define $macro_name\_$macro_suffix(fmt$macro_params_list) \\
  __$macro_name\_$macro_suffix\_$OriginID(LOG_CONSOLE_TEXT_INFO, $log_level, LOG_LOCAL_OID, LOG_LOCAL_GID, LOG_LOCAL_FID, __LINE__, \\
               (fmt)$pass_params_list)
#endif /* IWLWAV_RTLOG_MAX_DLEVEL < $log_level */
END_OF_HEADER

  my $body = <<END_OF_BODY;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= $log_level)
__MTLK_FLOG void
__$macro_name\_$macro_suffix\_$OriginID(const char *fname, int level, uint8 oid, uint8 gid, uint8 fid, uint16 lid, const char *fmt$func_params_list)
{
#if (IWLWAV_RTLOG_FLAGS & (RTLF_REMOTE_ENABLED | RTLF_CONSOLE_ENABLED))
  int flags__ = mtlk_log_get_flags(level, oid, gid);
#endif
#if (IWLWAV_RTLOG_FLAGS & RTLF_REMOTE_ENABLED)
  #ifndef __KLOCWORK__
  if ((flags__ & LOG_TARGET_REMOTE) != 0) {

    int32 lid_int32 = lid;
    size_t fnamelen__ = strlen(fname) + 1;
$str_length_calc
    size_t datalen__ = LOGPKT_STRING_SIZE(fname)
                       + LOGPKT_SCALAR_SIZE(lid_int32)$total_params_size;
    uint8 *pdata__;
    mtlk_log_event_t log_event__;
    mtlk_log_buf_entry_t *pbuf__ = mtlk_log_new_pkt_reserve(LOGPKT_EVENT_HDR_SIZE + datalen__, &pdata__);
    if (pbuf__ != NULL) {
      uint8 *p__ = pdata__;

      MTLK_ASSERT(pdata__ != NULL);
      MTLK_ASSERT(datalen__ <= $MAX_DSIZE_VALUE);

   // OLD header format
   // log_event__.info = LOG_MAKE_INFO(0, oid, gid);
   // log_event__.info_ex = LOG_MAKE_INFO_EX(fid, lid, datalen__, 0);

   // NEW header format
   // LOG_MAKE_INFO(log_event__, ver, dsize, wif, dest, prior, exp, oid, gid, fid, lid)
      LOG_MAKE_INFO(log_event__, /* ver */ 0, datalen__, /* wif */ 0,
                    0, 0, 0, /* dest, prior, exp, */
                    oid, gid, fid, lid);

      log_event__.timestamp = mtlk_log_get_timestamp();

      /* We do not copy the whole structure to avoid issues with  */
      /* incorrect packing. Client side assumes this structure    */
      /* is 1-byte packed, but some compilers have issues with    */
      /* creation of such structures.                             */
      /* WARNING: Because of this type of usage order of fields   */
      /* in the sctructure is important and must be preserved.    */
      logpkt_memcpy(p__, LOGPKT_EVENT_HDR_SIZE, &log_event__, LOGPKT_EVENT_HDR_SIZE);

      p__ += LOGPKT_EVENT_HDR_SIZE;

      LOGPKT_PUT_STRING(fname);
      LOGPKT_PUT_INT32(lid_int32);
$pack_params_code
      MTLK_ASSERT(p__ == pdata__ + datalen__ + LOGPKT_EVENT_HDR_SIZE);
      mtlk_log_new_pkt_release(pbuf__);
    }
  }
  #endif /*__KLOCWORK__*/
#endif
#if (IWLWAV_RTLOG_FLAGS & RTLF_CONSOLE_ENABLED)
  if ((flags__ & LOG_TARGET_CONSOLE) != 0) {
    $console_printout
  }
#endif
}
#endif /* IWLWAV_RTLOG_MAX_DLEVEL >= $log_level */
END_OF_BODY

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

  my $header = <<END_OF_HEADER;
#if (IWLWAV_RTLOG_MAX_DLEVEL < $log_level)
#define $macro_name($macro_params_list)
#else

__MTLK_FLOG void
__$macro_name\_$OriginID(const char *fname, uint8 gid, uint8 fid, uint16 lid, $func_params_list);

#define $macro_name($macro_params_list) \\
  __$macro_name\_$OriginID(LOG_CONSOLE_TEXT_INFO, LOG_LOCAL_GID, LOG_LOCAL_FID, __LINE__, \\
               $pass_params_list)
#endif /* IWLWAV_RTLOG_MAX_DLEVEL < $log_level */
END_OF_HEADER

  my $body = <<END_OF_BODY;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= $log_level)
__MTLK_FLOG void
__$macro_name\_$OriginID(const char *fname, uint8 gid, uint8 fid, uint16 lid, $func_params_list)
{
#if (IWLWAV_RTLOG_FLAGS & RTLF_REMOTE_ENABLED)
  int flags__ = mtlk_log_get_flags($log_level, $OriginID, gid);
  if ((flags__ & LOG_TARGET_REMOTE) != 0)
  {
    size_t datalen__ = LOGPKT_SIGNAL_SIZE(message_size);
    mtlk_log_multi_buffer_t *buffers;
    mtlk_log_signal_t mtlk_log_signal;

    if (mtlk_log_new_pkt_reserve_multi(LOGPKT_EVENT_HDR_SIZE_VER2 + datalen__, &buffers))
    {
      char *packet = message;
      mtlk_log_multi_buffer_t *current_buffer = &buffers[0];
      while (current_buffer->buf)
      {
        uint8 *p__ = current_buffer->data;
        uint16 packet_size = current_buffer->data_size;

        if (current_buffer == &buffers[0]) /* first packet contains header */
        {
          mtlk_log_event_t log_event__;

      // OLD header format
      // log_event__.info      = LOG_MAKE_INFO(2, $OriginID, gid);
      // log_event__.info_ex   = LOG_MAKE_INFO_EX(fid, lid, 0, 0);
      // log_event__.size      = datalen__;

          LOG_MAKE_INFO(log_event__, /* ver */ 2, datalen__, /* wif */ 0,
                        0, 0, 0, /* dest, prior, exp, */
                        $OriginID, gid, fid, lid);

          log_event__.timestamp = mtlk_log_get_timestamp();
          
          logpkt_memcpy(p__, LOGPKT_EVENT_HDR_SIZE_VER2, &log_event__, LOGPKT_EVENT_HDR_SIZE_VER2);
          
          p__ += LOGPKT_EVENT_HDR_SIZE_VER2;
          packet_size -= LOGPKT_EVENT_HDR_SIZE_VER2;
                                           
	  mtlk_log_signal.src_task_id = src_task_id;
	  mtlk_log_signal.dst_task_id = dst_task_id;
	  mtlk_log_signal.len = message_size;
      
          LOGPKT_PUT_SIGNAL_HEADER(mtlk_log_signal);

          packet_size -= LOGPKT_SIGNAL_HDR_SIZE;
                                           
          MTLK_ASSERT((int16)packet_size >= 0);
          MTLK_ASSERT(p__ == current_buffer->data + LOGPKT_EVENT_HDR_SIZE_VER2 + LOGPKT_SIGNAL_HDR_SIZE);
        }

        LOGPKT_PUT_SIGNAL_BODY(packet, packet_size);

        MTLK_ASSERT(p__ == current_buffer->data + current_buffer->data_size);

        packet += packet_size;

        ++ current_buffer;
      }
      mtlk_log_new_pkt_release_multi(buffers);
    }
  }
#endif
}
#endif /* IWLWAV_RTLOG_MAX_DLEVEL >= $log_level */
END_OF_BODY

  return ($header, $body);
}

sub generate_code_capwap
{
  my $macro_name = shift;
  my $log_level = shift;
  my $args        = shift;

  my $macro_params_list = "arg1, arg2, arg3, arg4, arg5";
  my $pass_params_list  = "arg1, arg2, arg3, arg4, arg5";
  my $func_params_list  = "uint8 arg1, void *arg2, size_t arg3, uint8 arg4, int arg5";

  my $header = <<END_OF_HEADER;
#if (IWLWAV_RTLOG_MAX_DLEVEL < $log_level)
#define $macro_name($macro_params_list)
#else

__MTLK_FLOG void
__$macro_name\_$OriginID(const char *fname, uint8 gid, uint8 fid, uint16 lid, $func_params_list);

#define $macro_name($macro_params_list) \\
  __$macro_name\_$OriginID(LOG_CONSOLE_TEXT_INFO, LOG_LOCAL_GID, LOG_LOCAL_FID, __LINE__, \\
               $pass_params_list)
#endif /* IWLWAV_RTLOG_MAX_DLEVEL < $log_level */
END_OF_HEADER

  my $body = <<END_OF_BODY;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= $log_level)
__MTLK_FLOG void
__$macro_name\_$OriginID(const char *fname, uint8 gid, uint8 fid, uint16 lid, $func_params_list)
{
#if (IWLWAV_RTLOG_FLAGS & RTLF_REMOTE_ENABLED)
  int flags__ = mtlk_log_get_flags($log_level, $OriginID, gid);
  if ((flags__ & LOG_TARGET_CAPWAP) != 0)
  {
    mtlk_ndev_send_capwap_packet($pass_params_list);
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

  dbg_print "The format string is            ====$string====\n";

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
  dbg_print "The packed format string is     ====$packed_format====\n";

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
  $GroupSeen{$group_name}++;

  if ($auto_generate_gid_list)
  {
    push(@GIDNamesList, $group_name) if (1 == $GroupSeen{$group_name});
  }
  elsif (not exists $GroupNameToID{$group_name})
  {
    print STDERR "Group $group_name is not defined in the file $GIDListFile\n";
    exit 1;
  }

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

my $dbg_dashes = "===========================";

sub dbg_print_line
{
  my $line = shift;
  my $lnum = shift;
  dbg_print "\n$dbg_dashes$dbg_dashes\n";
  dbg_print "The line $lnum is\n";
  dbg_print "$dbg_dashes\n";
  dbg_print "$line\n";
  dbg_print "$dbg_dashes\n";
}

sub process_dump_command
{
  my $line = shift;
  my $line_number = shift;
  my $macro = shift;
  my $log_level = shift;
  my $msg_string = shift;

  $line =~ s/.*\n  (.* $macro)/$1/xs; # remove text before the macro

  dbg_print_line($line, $line_number);
  dbg_print "log level                       ==".$log_level  ."==\n";
  dbg_print "msg string                      ==".$msg_string ."==\n";
  dbg_print "$dbg_dashes\n";

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

  # push a format string for printing out the $msg_string (instead of the $msg_string itself)
  push @StringsList, [ ($line_number, $curr_group_name, $curr_file_id, "(%s:%d): %s") ];
}

sub process_print_command
{
  my $line = shift;
  my $line_number = shift;
  my $macro = shift;
  my $log_level = shift;
  my $in_macro_suffix = shift;
  my $text_before_format_string = shift;
  my $format_string = shift;
  my $text_after_format_string = shift;

  $line =~ s/.*\n  (.* $macro)/$1/xs; # remove text before the macro line

  # Input full macro name with suffix
  my $in_macro_name = $macro . "_" . $in_macro_suffix;

  # Remove log level from ELOG/WLOG/RLOG
  $macro =~ s/[0-9]// if $macro =~ /^[WER]LOG.*/;

  $log_level = "IWLWAV_RTLOG_ERROR_DLEVEL" if $macro =~ /^ELOG.*/;
  $log_level = "IWLWAV_RTLOG_WARNING_DLEVEL" if $macro =~ /^WLOG.*/;
  $log_level = "IWLWAV_RTLOG_RELEASE_DLEVEL" if $macro =~ /^RLOG.*/;

  if (not defined $log_level)
  {
    #If log level not specified then this is not a printout we recognize
    #do nothing and keep the code as is.
    return $macro.$text_before_format_string.$format_string.$text_after_format_string;
  }

  dbg_print_line($line, $line_number);
  dbg_print "macro is                        ==".$macro."==\n";
  dbg_print "log level is                    ==".$log_level."==\n";
  dbg_print "macro suffix is                 ==".$in_macro_suffix."==\n";
  dbg_print "text before format string is    ==".$text_before_format_string."==\n";
  dbg_print "format string is                ==".$format_string."==\n";
  dbg_print "text after format string is     ==".$text_after_format_string."==\n";
  dbg_print "$dbg_dashes\n";

  # Generate full macro name with suffix and compare with input
  my $macro_suffix = get_macro_suffix_string($format_string);
  my $macro_name = $macro . "_" . $macro_suffix;
  if ("$in_macro_name" ne "$macro_name")
  {
    log_error($line_number, "Incorrect macro name '$in_macro_name', expected '$macro_name'\n$line");
  }

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

  push @StringsList, [ ($line_number, $curr_group_name, $curr_file_id, "(%s:%d): $SCDFormatString") ];

  #Generate the macro and put it to code database
  (my $header, my $body) = generate_code($macro, $log_level, $macro_suffix);
  if( -1 == index($HeaderData, " $macro_name(") )
  {
    $HeaderData .= $header."\n\n";
    $SourceData .= $body."\n\n";
    $UpdateFlag = 1;
  }

  return $macro_name.$text_before_format_string.$format_string.$text_after_format_string;
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
    $UpdateFlag = 1;
  }

  return $macro."(".$src_task_id.", ".$dst_task_id.", ".$message_type.", ".$rest;
}

sub process_capwap_command
{
  my $line_number = shift;
  my $macro       = shift;
  my $log_level   = shift;
  my $args        = shift;

  if (not defined $log_level)
  {
    #If log level not specified then this is not a printout we recognize
    #do nothing and keep the code as is.
      return $macro."(".$args.");"
  }

  if (not defined $curr_group_name)
  {
    print STDERR "No group name definition before $macro at line $line_number\n";
    exit 1;
  }

  if (not defined $curr_file_id)
  {
    print STDERR "No file ID definition before $macro at line $line_number\n";
    exit 1;
  }

  if($line_number > $MAX_LID_VALUE)
  {
    print STDERR "Line ID is too big ($line_number > $MAX_LID_VALUE)\n";
    exit 1;
  }

  #Generate the macro and put it to code database
  (my $header, my $body) = generate_code_capwap($macro, $log_level, $args);
  if( -1 == index($HeaderData, " $macro(") )
  {
    $HeaderData .= $header."\n\n";
    $SourceData .= $body."\n\n";
    $UpdateFlag = 1;
  }

  return $macro."(".$args.");"
}

open my $fhLock, '>', "$GIDListDir/.logprep.lock~" or die "error opening $GIDListDir/.logprep.lock~ for reading: $!";
flock $fhLock, LOCK_EX;

#Read GID list and build number to name conversion hash
if(-e $GIDListFile)
{
  open my $fh, '<', $GIDListFile or die "error opening $GIDListFile for reading: $!";
  local $INPUT_RECORD_SEPARATOR="\n";
  while (<$fh>)
  {
      die "Incorrect GID list entry $_" if(not s/^\#define\s+(\w+)\s+(\w+).*$/
        { push(@GIDNamesList, $1);
          $GroupIDToName{$2} = $1;
          $GroupNameToID{$1} = $2;
        }/eg);
  }
  @GIDListOrig = @GIDNamesList;
}

#Read headers database
if(-e $MacroHeaderFile)
{
  open my $fh, '<', $MacroHeaderFile or die "error opening $MacroHeaderFile for reading: $!";
  $HeaderData = do { local $/; <$fh> };
}

#Read source database
if(-e $MacroSourceFile)
{
  open my $fh, '<', $MacroSourceFile or die "error opening $MacroSourceFile for reading: $!";
  $SourceData = do { local $/; <$fh> };
}

# Read GIDs, strings and messages from SCD
if(-e $SCDFile)
{
  my $fh = open_input_file($SCDFile);
  while (<$fh>)
  {
    $line = $_;
    if ($line =~ /^G/) { # GID
      s/^G\s+(\w+)\s+(\w+)\s+(\w+)\s+(.*)$
       /$GroupSeen{$3}++/egx;
    } elsif ($line =~ /^S/) { # string
      s/^S\s+(\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(.*)$
       /push(@StringsList, [ ($4, $GroupIDToName{$2}, $3, $5) ])/egx
    } elsif ($line =~ /^T/) { # message
      s/^T\s+(\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(.*)$
       /push(@MessageList, [ ($4, $GroupIDToName{$2}, $3, $5) ])/egx
    }
  }
  close $fh;
}

$INPUT_RECORD_SEPARATOR=";";

my $MacroNameRe       = '((?<!\w)[IWER]LOG([0-9]){0,1})_([A-Z]+)'; # with required suffix
my $DumpMacroNameRe   = '((?<!\w)(mtlk_dump))';
my $SignalMacroNameRe = '((?<!\w)SLOG([0-9]){0,1})';
my $CapwapMacroNameRe = '((?<!\w)CAPWAP([0-9]){0,1})';
my $CurrentLineNumber = 1;

if (defined $InputFileName)
{
  $fhInput = open_input_file($InputFileName)
}
else
{
  $InputFileName = "<STDIN>";
  $fhInput = STDIN;
}

my $prev_line = "";
my $orig_line = "";

while (<$fhInput>)
{
    my $line = $prev_line . $_;
    $orig_line = $line;

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
    elsif($line =~ /.*$SignalMacroNameRe(.|\n)*/)
    {
      if($multiline_macro_binding_first_line)
      {
          $line =~ s/$SignalMacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
    }
    elsif($line =~ /.*$CapwapMacroNameRe(.|\n)*/)
    {
      if($multiline_macro_binding_first_line)
      {
          $line =~ s/$CapwapMacroNameRe(.|\n)*//mg;
      }
      $LinesBeforePrintout = $line =~ s/\n/\n/mg;
      $line = $orig_line;
    }

    #Find group IDS
    $line =~ s/(LOG_LOCAL_GID\s+)(\w+)/$1.process_group_definition($2)/xemg;
    $line =~ s/(LOG_LOCAL_FID\s+)(\w+)/$1.process_file_definition($2)/xemg;

    #Process printouts
    my $line_num = $CurrentLineNumber + $LinesBeforePrintout;
    if($line =~ /.*$MacroNameRe(.|\n)*/) {
        # Checking required format string i.e. '"'
        unless ($line =~ /$MacroNameRe[^(]*[(][^,]*["]/)
        {
          log_error($line_num, "Missing format string\n$&");
        }

        # A line may not be fully read if it contains a separator inside the format string.
        # In this case, the following line will be added to it.
        $prev_line = $line;

        $line =~ s/
          $MacroNameRe                                    #Macro name
          ((?: [^"]|\n)*")                                #Text between macro name and beginning of the format string
          ((?: .|\n)*?)                                   #Format string
          ((?<!\\)"\s*[\,\)](?: .|\n)*)                   #Text after format string
          /process_print_command($orig_line, $line_num, $1, $2, $3, $4, $5, $6);
           $prev_line = "";  # print complete
          /xemg;
    }
    elsif($line =~ /.*$SignalMacroNameRe(.|\n)*/) {
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
          /process_signal_command($line_num, $1, $2, $3, $4, $5, $6)/xemg;
    }
    elsif($line =~ /.*$CapwapMacroNameRe(.|\n)*/) {
        $line =~ s/
          $CapwapMacroNameRe  #Macro name
          (?:\s*[\(]\s*)      #Text between macro name and first parameter
          (.*)                #Args
          (?:\s*[)]\s*[;]\s*) #rest
          /process_capwap_command($line_num, $1, $2, $3)/xemg;
    }
    # Process mtlk_dump printout
    elsif($line =~ /
                $DumpMacroNameRe        #Macro name
                \s*\(\s*                #'('
                (\d+)                   #Log level
                [^"]+                   #Text before message string
                \"([^"]*)\"             #Message string
                \s*\)                   #Text after string and ')'
                /xmg)
    {
        $LinesBeforePrintout = $line =~ tr/\n//;
        process_dump_command($line, $line_num, $1, $2, $3);
    }

    # Don't update line number if the processing was not completed
    if ("$prev_line" eq "")
    {
      $CurrentLineNumber += $orig_line =~ s/\n/\n/mg;
    }
}

#close $fhInput;

if ("$prev_line" ne "")
{
  log_fatal($CurrentLineNumber, "Incomplete macro line\n$orig_line");
}

exit(2) if ($ErrorFlag);

open my $fhSCD, '>', $SCDFile or die "error opening $SCDFile for writing: $!";
print $fhSCD "O $OriginID $OriginName\n";
print $fhSCD "D $OriginID $ELFFile\n";

#Flush GID list into both C header and SCD file
my $i=1;
my @GIDListNew = ();

foreach $GID (@GIDNamesList)
{
  if ($GroupSeen{$GID})
  {
    push(@GIDListNew, $GID);
    if ($auto_generate_gid_list)
    {
      $GroupNameToID{$GID} = $i++;
    } else {
      $i = $GroupNameToID{$GID};
    }
    if($i > $MAX_GID_VALUE)
    {
      print STDERR "Group ID is too big for $GID ($i > $MAX_GID_VALUE)\n";
      exit 1;
    }

    print $fhSCD "G $OriginID ".$GroupNameToID{$GID}." $GID\n";
  }
}

#If new GIDs were added we have to rebuild the GID list file
#It is only for the auto_generate_gid_list mode
if($auto_generate_gid_list and ($#GIDListNew != $#GIDListOrig))
{
  open my $fh, '>', $GIDListFile or die "error opening $GIDListFile for writing: $!";

  foreach $GID (@GIDListNew)
  {
    print $fh "\#define $GID\t".$GroupNameToID{$GID}."\n";
  }

  close $fh;
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

if ($UpdateFlag) {
#Flush headers database
open $fh, '>', "$MacroHeaderFile.tmp" or die "error opening $MacroHeaderFile for writing: $!";
print $fh $HeaderData;
close $fh;

#Flush source database
open $fh, '>', "$MacroSourceFile.tmp" or die "error opening $MacroSourceFile for writing: $!";
print $fh $SourceData;
close $fh;

move("$MacroHeaderFile.tmp", $MacroHeaderFile);
move("$MacroSourceFile.tmp", $MacroSourceFile);
}

flock $fhLock, LOCK_UN;
close $fhLock;

exit 0;
