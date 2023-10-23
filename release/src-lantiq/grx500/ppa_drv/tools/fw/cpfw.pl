#!/usr/bin/perl

use warnings;
use strict;

use File::Copy;
use File::Basename;

#Attention: This script must work at its own folder

my $fw_repo_dir;
my $dst_dir = `pwd`;
chomp($dst_dir);
$dst_dir .= "/../../src/platform/";

#Get the ppe repo path from ppe configuration file
my %config = do "./ppe_cfg";
$fw_repo_dir = $config{"ppe_repo_path"};

if (!defined($fw_repo_dir)){
    die "Please configure the fw_repo_dir in the script before you run it. \n";
}

if(@ARGV < 1){
   die "$0 PPE_TAG <Check_Tag_Enable: 0|1>\n";
}

my $ppe_tag = shift @ARGV;
my $mercurial_tag;
my $ppe_repo;
my $check_tag = 1;

if(@ARGV > 0){ #Second parameter is check tag enable or disable, 1- enable, 0 - disable By default, is enable
	$check_tag = shift @ARGV;
}

#check if PPE repo is there, if not, clone one, if found a folder with same name but not ppe repo, warn and exit
&check_ppe_repo($config{"ppe_repo_path"},$config{"ppe_repo_parent_dir"},$config{"ppe_repo_url"});

#parse ppe tag to get tag name and subrepo name
if($ppe_tag =~ /(\w+.*)@(\w+.*)@(\w+.*)/){
    $mercurial_tag = $1;
    $ppe_repo = $3;
}else{
    die "Error: PPE FW tag is error formatted: $ppe_tag \n";
}

#print "$ppe_tag, $mercurial_tag, $ppe_repo \n";
#change to ppe subrepo dir and update to the given tag
$fw_repo_dir .= $ppe_repo;
chdir($fw_repo_dir) or die "Cannot change to PPE repo dir! : $fw_repo_dir\n";
system("hg pull");
die "Cannot pull the repo \n" if (!$? == 0);
system("hg update -C $mercurial_tag");
die "Cannot pull to the tag: $mercurial_tag \n" if(!$? == 0);

#walk through the configure file for PPE firmware packages and get the src and dst file
$fw_repo_dir .= $config{"ppe_release_dir"};

foreach my $pkg (keys %{$config{$ppe_repo}})
{
	foreach my $platform (keys %{$config{$ppe_repo}{$pkg}}){
		my $ppe_s_file;
		my $ppe_d_file;
		($ppe_s_file, $ppe_d_file) = @{$config{$ppe_repo}{$pkg}{$platform}};
		$ppe_s_file = $fw_repo_dir . $ppe_s_file;
		$ppe_d_file = $dst_dir . $ppe_d_file;
		
		if($check_tag){
			my $tag_ok = &check_file_tag($ppe_tag, $ppe_s_file);
			if($tag_ok == 0){
				next;
			}
		}
		if(-f $ppe_s_file && -f $ppe_d_file){
			print "copy ". basename($ppe_s_file) . "   \t\t to      " . basename($ppe_d_file) . " \n";
			copy($ppe_s_file, $ppe_d_file);
		}
	}
}

#copy FW released .h file to driver directory
my $ppe_h_file = "fw_h_file";
$fw_repo_dir = $config{"ppe_repo_path"};
foreach my $fw_h_file (keys %{$config{$ppe_h_file}})
{
	my $ppe_s_file;
	my $ppe_d_file;
	
	($ppe_s_file, $ppe_d_file) = @{$config{$ppe_h_file}{$fw_h_file}};
	$ppe_s_file = $fw_repo_dir . $ppe_s_file;
	$ppe_d_file = $dst_dir . $ppe_d_file;

	if(-f $ppe_s_file && -f $ppe_d_file){
		print "copy " . basename($ppe_s_file) . "\t\t\t\tto    " . basename($ppe_d_file) . "\n";
		copy($ppe_s_file, $ppe_d_file);	
	}	
}

#function verify file tag is exactly as the given tag
sub check_file_tag{
    my $tag = shift @_;
	my $sfile = shift @_;
	my $tag_found = 0;
	
	open PPE_FW , "<$sfile" or  die "Cannot open ppe fw file :$sfile  ($!)\n";
    
    while(<PPE_FW>){
	  if(/$tag/){
		   $tag_found = 1;
		   last;
	  }
    }
    close PPE_FW;
	
	if(!$tag_found){
		warn("$sfile don't have tag: $tag!!!\n");
	}
	
	return $tag_found;
}

#function verify the ppe_repo and pull it if there's no repo there
sub check_ppe_repo{
	my $ppe_repo_dir = shift @_;  #from config
	my $ppe_repo_parent_dir = shift @_; #from config
	my $ppe_repo_url = shift @_; #from config
	my $ppe_hgrc = $ppe_repo_dir . ".hg/hgrc";
	my $is_ppe_repo = 0;
	
	if( -d $ppe_repo_dir ){
		if( ! -f $ppe_hgrc ){
		    print "Folder $ppe_repo_dir exist, but not hg repo !!!\n";
			return -1;
		}else{
			open PPE_HGRC, "<$ppe_hgrc"  or die "Cannot open ppe hgrc file: $ppe_hgrc ($!)\n";
			while(<PPE_HGRC>){
				if(/$ppe_repo_url/){
					$is_ppe_repo = 1;
					last;
				}
			}
			close PPE_HGRC;
			if(!$is_ppe_repo){
				print "Folder $ppe_repo_dir exit, but not ppe repo\n";
				return -1;
			}
		}
	}else{
		chdir($ppe_repo_parent_dir) or die "Cannot change to dir :$ppe_repo_parent_dir\n";
		print "Do hg clone $ppe_repo_url \n";
		system("hg clone $ppe_repo_url");
	}
}




