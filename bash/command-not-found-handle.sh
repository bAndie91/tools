#!/bin/bash

command_not_found_handle()
{
	local command=$1
	#shift
	#declare -a args=("$@")
	
	echo -n "$command: command not found. other similar commands: " >&2
	
	local threshold
	if [ ${#command} -le 3 ]
	then
		threshold=1
	else
		threshold=2
	fi
	local startswith_threshold=2
	local endswith_threshold=0
	
	{
		find ${PATH//:/ } -maxdepth 1 -type f -executable -printf '%f\n' 2>/dev/null
		declare -p -F | cut -d' ' -f3-
		alias -p | cut -d' ' -f2- | cut -d= -f1
	}\
	| levenshtein-distance "$command" \
	| env command=$command perl -ne '
		chomp;
		$\ = "\n";
		$, = "\t";
		s/^(\d+)\s//;
		$distance = $1;
		$pos = index $_, $ENV{command};
		$rpos = rindex $_, $ENV{command};
		print $pos, $rpos, $distance, length($_), $_;' \
	| env startswith_threshold=$startswith_threshold endswith_threshold=$endswith_threshold distance_threshold=$distance_threshold \
		perl -ne '
		/(\S+)\s(\S+)\s(\S+)/;
		if($1 > $ENV{startswith_threshold} and $2 > $ENV{endswith_threshold} and $3 > $ENV{distance_threshold}) {}
		else { print; }
		' \
	| awk '{ if($1 == -1){$1=999} if($2 == -1){$2=999} if($3 == -1){$3=999} print }' \
#	| sort -k1n -k2n \
#	| cut -d'\t' -f5- \
#	| tr "\n" " " \
	  >&2
	echo >&2
	
	return 127
}
