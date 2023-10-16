#!/bin/bash

command_not_found_handle()
{
	local command=$1
	#shift
	#declare -a args=("$@")
	
	echo "$command: command not found. similar commands: " >&2
	
	local distance_threshold
	if [ ${#command} -le 3 ]
	then
		distance_threshold=1
	else
		distance_threshold=2
	fi
	local startswith_threshold=3
	local endswith_threshold=0
	
	{
		find ${PATH//:/ } -maxdepth 1 -type f -executable -printf '%f\n' 2>/dev/null
		declare -p -F | cut -d' ' -f3-
		alias -p | cut -d' ' -f2- | cut -d= -f1
	}\
	| levenshtein-distance "$command" \
	| env command=$command perl -ne '
		use List::Util qw/min/;
		chomp;
		$\ = "\n";
		$, = "\t";
		s/^(\d+)\s//;
		$distance = $1;
		$lpos = index $_,  $ENV{command}; $lpos = 999 if $lpos == -1;
		$rpos = rindex $_, $ENV{command}; $rpos = 999 if $rpos == -1;
		print $lpos, $rpos, $distance, length($_), $_;' \
	| env startswith_threshold=$startswith_threshold endswith_threshold=$endswith_threshold distance_threshold=$distance_threshold \
		perl -ne '
		/(\S+)\s(\S+)\s(\S+)/;
		if($1 > $ENV{startswith_threshold} and $2 > $ENV{endswith_threshold} and $3 > $ENV{distance_threshold}) {}
		else { print; }
		' \
	| perl -ne '
		use List::Util qw/min/;
		$, = "\t";
		s/(\S+)\s(\S+)\s(\S+)\s//;
		@scores = ($1, $2, $3);
		print min(@scores), $_;' \
	| sort -k1n -k2n \
	| cut -d$'\t' -f3- \
	| column \
	  >&2

# TODO dont show too few/many suggestions
	
	return 127
}
