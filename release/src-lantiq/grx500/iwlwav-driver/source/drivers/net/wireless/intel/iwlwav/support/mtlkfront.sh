#!/bin/sh

#Basic Script configuration
mtlkfront_do_output_processing=n
print_when_checking=n
print_when_preprocessing=y
trace_commands=n
do_logger_preprocessing=y

is_source_file()
{
  test "x`echo $1 | grep '\.\(c\|cpp\)$'`" != x
}

logger_preprocessing_enabled()
{
  test "x$do_logger_preprocessing" = "xy"
}

check_mtlk_inc_source_file()
{
  awk "/^[[:blank:]]*\#include[[:blank:]]+[\"\<]mtlkinc\.h[\"\>]/ { exit 0; } \
       /^[[:blank:]]*\#include/ { print NR; exit 1; }" < $1 &&
  return 0
  
  return 1
}

check_mtlk_inc_header_file()
{
  awk "/^[[:blank:]]*\#include[[:blank:]]+[\"\<]mtlkinc\.h[\"\>]/ { print NR; exit 1; }"  < $1 &&
  return 0
  
  return 1
}

check_mtlk_inc()
{
  if is_source_file $1
  then
    LINE_NO=`check_mtlk_inc_source_file $1`
  else
    LINE_NO=`check_mtlk_inc_header_file $1`
  fi

  if [ "$LINE_NO" != "" ] ;
  then
    echo $1\($LINE_NO\) : error: wrong mtlkinc.h inclusion
    return 1
  fi

  return 0
}

#Output-postprocessing part
#Performs output preprocessing and symlinks parsing to hide rebasing
if test x"$mtlkfront_do_output_processing" = x"y"; then
FILTER_CMD=do_filtering
else
FILTER_CMD=cat
fi

do_filtering () {

perl -e '
    use strict;

    my $builddir=path_to_regexp($ARGV[0]);
    my $srcdir=$ARGV[1];
    
    while (<STDIN>)
    {
      my $line=$_;
      my @file_names = ();
      
      #Get all substrings that look like file names from the string
      if(@file_names = ($line =~ /[\w\d\.\/\-]+\/+/g))
      {
        foreach my $file_name (@file_names)
        {
           my $full_name=trim(`([ -e $file_name ] && readlink -f $file_name) || echo $file_name`);
           my $file_name_regexp = path_to_regexp($file_name);
           $line =~ s/$file_name_regexp/$full_name\//;
        }
      }
      
      $line =~ s/$builddir/$srcdir/g;
      #Remove multiple consequent slashes
      $line =~ s/([a-zA-Z0-9_\.\-])\/+([a-zA-Z0-9_\.\-])/\1\/\2/g;
      print "$line";
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
      $path =~ s/([\:\\\/\.\,\-])/\\$1/g;
      return $path;
    }' "`readlink -f $abs_builddir`/" "`readlink -f $abs_top_srcdir`/"
}

run_filtered () {

{ $* 2>$abs_builddir/.$$.stderr; echo $?>$abs_builddir/.$$.pipe.result; } | $FILTER_CMD
$FILTER_CMD < $abs_builddir/.$$.stderr 1>&2

RESULT_VAL=`cat $abs_builddir/.$$.pipe.result`

rm -f $abs_builddir/.$$.pipe.result $abs_builddir/.$$.stderr
return $RESULT_VAL

}

run_stderr_filtered () {

  { $* 2>$abs_builddir/.$$.stderr; echo $?>$abs_builddir/.$$.pipe.result; }
  $FILTER_CMD < $abs_builddir/.$$.stderr 1>&2

  RESULT_VAL=`cat $abs_builddir/.$$.pipe.result`

  rm -f $abs_builddir/.$$.pipe.result $abs_builddir/.$$.stderr
  return $RESULT_VAL

}

#Logger-related part
#Performs static code checks and logprep preprocessing
if test "x$logger_origin_name" = x; then 
logger_origin_name=UNKNOWN_ORIGIN;
fi

if test "x$logger_binary_name" = x; then 
logger_binary_name=UNKNOWN_ELF;
fi

if test "x$logger_origin_id" = x; then 
logger_origin_id=255;
fi

# We use heuristics telling that file name
# is the last parameter of the compile command
FILE_NAME=`awk '{print $NF;exit;}'<<END
$*
END
`

echo "  MF      $FILE_NAME"

#If file is .C of .CPP and compilation do preprocessing with logprep
if logger_preprocessing_enabled && \
   is_source_file $FILE_NAME && \
   test "x`echo $* | grep -v -w '[-]E'`" != x; then

  # Build dependencies list
  MKDEP_CMD=`echo $* |                                                                           \
             sed -e's|-M[TF][[:space:]][[:space:]]*[^[:space:]][^[:space:]]*||g'                 \
                 -e's|\(-Wp,\)*-M[DP][[:space:]][[:space:]]*||g'                                 \
                 -e's|\(-Wp,\)*-M[DP]\,[^[:space:]]*||g'                                         \
                 -e"s|-o[[:space:]][[:space:]]*[^[:space:]][^[:space:]]*| -D__MTLK_DEPENDENCY_GENERATION_PATH -MM -MT $FILE_NAME|g"`

  # Create logmacros.h and loggroups.h because source code depends on them
  for file in $abs_builddir/logmacros.h $abs_top_srcdir/loggroups.h ; do
    test -e $file || touch -m -t 198001010000.00 $file
  done

  TOP_SRCDIR=`readlink -f $abs_top_srcdir`
  RAW_DEPS="`run_stderr_filtered $MKDEP_CMD`" || exit 10
  DEPS=`echo $RAW_DEPS                               | \
        awk '{ for (i = 1; i <= NF; i++) print $i }' | \
        grep -v ':$'                                 | \
        grep -v '\\.mod\\.c$'                        | \
        sed 's/\\\\//'                               | \
        grep -v 'logmacros\\.[hc]'                   | \
        grep -v 'loggroups\\.h'                      | \
        grep -v '/shared_mbss_mac/'                  | \
        xargs -n 1 -r readlink -f                    | \
        grep $TOP_SRCDIR`

  # DEPS already contains name of file being compiled, so we just need 
  # to run logprep for each file in list
  for file in $DEPS; do
    TIMESTAMP_FILE=$abs_builddir/`echo $file | sed -e"s|$TOP_SRCDIR|./.logprep_ts/|g"`.logprep_ts
    if test -e $file; then
	    if test ! -e $TIMESTAMP_FILE -o $file -nt $TIMESTAMP_FILE; then
	      (test x$print_when_checking = xy) && ( (test -e $TIMESTAMP_FILE && echo "    CK [M] $file") || echo "    CK    $file" )
	      check_mtlk_inc $file || exit $?
	      (test x$print_when_preprocessing = xy) && ( (test -e $TIMESTAMP_FILE && echo "    LP [M] $file") || echo "    LP    $file" )
	      perl $abs_logprep_srcdir/logprep.pl gcc \
	                                          $logger_origin_id $logger_origin_name \
	                                          $abs_top_srcdir/loggroups.h $abs_builddir/logmacros.h \
	                                          $abs_builddir/logmacros.c $abs_builddir/$logger_origin_name.scd $logger_binary_name \
	                                          $file || { echo Logprep error while processing file $file && exit 1; }

	      mkdir -p `dirname $TIMESTAMP_FILE` && touch $TIMESTAMP_FILE
	    fi
    fi
  done

  #Do not rebuild all sources because of loggger database changes
  touch -m -t 198001010000.00 $abs_builddir/logmacros.h $abs_top_srcdir/loggroups.h

fi;

(test x$trace_commands = xy) && (echo "MTLKFRONT invoked for: $*" 1>&2)

#Here we put logger origin ID into the marker supplied by Makefile.am.common
PREPROCESSED_COMMAND=`echo $* | sed -e"s|LOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT|LOG_LOCAL_OID=$logger_origin_id|g"`

(test x$trace_commands = xy) && (echo "MTLKFRONT will invoke: $PREPROCESSED_COMMAND" 1>&2)

run_filtered $PREPROCESSED_COMMAND
