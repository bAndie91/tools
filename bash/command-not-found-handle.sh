#!/bin/bash

command_not_found_handle()
{
	local command=$1
	#shift
	#declare -a args=("$@")
	
	echo "bash: $command: command not found" >&2
	echo -n "did you mean... " >&2
	
	local threshold
	if [ ${#command} -le 4 ]
	then
		threshold=1
	else
		threshold=2
	fi
	
	{
		find ${PATH//:/ } -maxdepth 1 -type f -executable -printf '%f\n' 2>/dev/null
		declare -p -F | cut -d' ' -f3-
		alias -p | cut -d' ' -f2- | cut -d= -f1
	}\
	| levenshtein-distance "$command" \
	| while read -r metric word
	do
		if [ $metric -le $threshold ]
		then
			echo "$metric	$word"
		fi
	done \
	| sort -n \
	| sed -e 's/^\S\+\s\+//' \
	| tr "\n" " " \
	  >&2
	echo >&2
	
	return 127
}
