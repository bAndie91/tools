#!/bin/bash

command_not_found_handle()
{
	local badcmd=$1
	#shift
	#declare -a args=("$@")
	
	echo "$badcmd: command not found. similar commands: " >&2
	
	local distance_threshold
	if [ ${#badcmd} -le 4 ]
	then
		distance_threshold=1
	else
		distance_threshold=2
	fi
	local startswith_threshold=3
	local endswith_threshold=0
	local min_suggestions=20
	
	# 1. list every commands in all the PATH directories and bash functions
	#    and aliases.
	# 2. measure Levenshtein between the typed word and each commands.
	# 3. find exact matches of typed word in each commands and score them
	#    according to how close is it to the start or end of the command name.
	# 4. calc the lowest score (the lower the better), ie. overall score.
	# 5. sort the commands first by the lowest score, then length.
	# 6. pass through commands, scores of which are below threshold.
	# 7. worse-score commands are passed through too, in groups, to show at
	#    least a given number of suggestions.
	#    they are groupd by the overall score.
	
	{
		find ${PATH//:/ } -maxdepth 1 -type f -executable -printf '%f\n' 2>/dev/null
		declare -p -F | cut -d' ' -f3-
		alias -p | cut -d' ' -f2- | cut -d= -f1
	}\
	| levenshtein-distance "$badcmd" \
	| env badcmd=$badcmd perl -ne '
		BEGIN {
			use List::Util qw/min/;
			$\ = "\n";
			$, = "\t";
			$worst_score = 999;
		}
		chomp;
		s/^(\d+)\s//;
		$distance = $1;
		$len = length;
		$spos = index $_,  $ENV{badcmd}; $spos = $worst_score if $spos == -1;
		$rpos = rindex $_, $ENV{badcmd};
			if($rpos == -1) { $epos = $worst_score; } else { $epos = $len - ($rpos + length($ENV{badcmd})); }
		print min($spos, $epos, $distance), $spos, $epos, $distance, $len, $_;' \
	| sort -k1n -k5n \
	| env min_suggestions=$min_suggestions startswith_threshold=$startswith_threshold endswith_threshold=$endswith_threshold distance_threshold=$distance_threshold\
		perl -ne '
		$, = "\t";
		s/(\S+)\s(\S+)\s(\S+)\s(\S+)\s(\S+)\s//;
		($score, $spos, $epos, $distance, $len) = ($1, $2, $3, $4, $5);
		if($spos > $ENV{startswith_threshold} and $epos > $ENV{endswith_threshold} and $distance > $ENV{distance_threshold})
		{
			if($printed >= $ENV{min_suggestions} and $score ne $last_score) { exit; }
		}
		print "$_";
		$printed++;
		$last_score = $score;' \
	| column \
	  >&2
	
	return 127
}
