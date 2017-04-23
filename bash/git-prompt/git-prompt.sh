#!/bin/bash

__git_prompt_print()
{
local n=0
local color
local line
local RESET
local gitdir prefix isthereworktree=
local hash
local branch= branches= tags= descbranch= desctag= pointer=
local ahead= behind= divergence=
local upstream= fallback= pushpull=

local X Y
local MX= AX= DX= RX= CX= UX= QX= EX=
local MY= AY= DY= RY= CY= UY= QY= EY=
local AA= DD= AU= DU= UA= UD= UU= U=
local unstag staged
local specdir= speclist=() spec=

local add del rest
local adds1= dels1= adds2= dels2=
local delta1 delta2

local sign_branch=❦
local sign_tags=➼
local sign_desc=☛
local sign_ahead=▲
local sign_behind=▼
local sign_unstag=✎
local sign_staged=✈
#local sign_stash=⌂ ❂ ⚑ ⚒ ◳ ◪ ◕ ◍ ⌘ ∗
local sign_stash=●
local sign_pushpull=★
local sign_origin=☆
local sign_clean=✔

trueish()
{
	[ "${1,,}" = true -o "${1,,}" = yes -o "${1,,}" = y ] || [ "$1" -gt 0 ] 2>/dev/null
}

get_tracking_branch()
{
	# Options:
	#  -H  fall back to "origin/HEAD" if it is tracked but current branch is not remotely tracked
	# Arguments:
	#  - local branch name; mandatory
	#  - variable name to store remote tracking ref, or "origin/$1" if not found;
	#    it is being echoed if this argument is omitted
	#  - variable name to store "1" if branch is not remotely tracked, or "" otherwise
	local okHEAD=
	if [ "$1" = -H ]
	then
		okHEAD=1
		shift
	fi
	local branch=$1
	local savetovar=$2
	local fellback=$3
	local remote=`git config --get-all "branch.$branch.remote" | head -n1`
	local rbranch
	if [ -n "$remote" ]
	then
		rbranch=`git config --get-all "branch.$branch.merge" | head -n1`
		# Strip "refs/heads/"
		rbranch=${rbranch:11}
		[ -n "$fellback" ] && declare -g $fellback=
	fi
	if [ -z "$remote" -o -z "$rbranch" ]
	then
		remote=origin
		if [ $okHEAD ] && [ -n "$(git branch --remotes --list $remote/HEAD)" ]
		then
			rbranch=HEAD
		else
			rbranch=$branch
		fi
		[ -n "$fellback" ] && declare -g $fellback=1
	fi
	if [ -n "$savetovar" ]
	then
		declare -g $savetovar="$remote/$rbranch"
	else
		echo "$remote/$rbranch"
	fi
}

gitdir=`git rev-parse --git-dir 2>/dev/null` || return
hash=`git show -s --format=%H 2>/dev/null`

for color in BLACK RED GREEN YELLOW BLUE MAGENTA CYAN WHITE
do
	local $color B$color ${color}BG
	eval "$color='[0;3${n}m'"
	eval "B$color='[1;3${n}m'"
	eval "${color}BG='[4${n}m'"
	let n++
done
RESET='[0m'



if [ -z "$hash" ]
then
	speclist+=(INIT)
fi
if [ "$(git rev-parse --is-bare-repository)" = true ]
then
	specdir=BARE
fi
if [ "$(git rev-parse --is-inside-git-dir)" = true ]
then
	specdir=GITDIR
fi
if [ -d "$GIT_WORK_TREE" ] || [ "$(git rev-parse --is-inside-work-tree)" = true ]
then
	isthereworktree=1
fi


IFS_=$IFS
IFS=$'\n'
	branch=`git symbolic-ref --quiet --short HEAD`
	if [ -n "$branch" ]
	then
		pushpull=$sign_pushpull
		get_tracking_branch "$branch" upstream fallback
		[ "$fallback" ] && pushpull=$sign_origin
		
		line=`git rev-list --count --left-right "$branch...$upstream" 2>/dev/null`
		# Empty $line means remote branch is not tracked.
		# "0\t0" means they are in sync.
		if [ "${line%	*}" = 0 -a "${line#*	}" = 0 ]
		then
			pushpull=
		fi
	else
		desctag=`git describe --contains --tags HEAD 2>/dev/null`
		descbranch=`git describe --contains --all HEAD 2>/dev/null`
		descbranch=${descbranch#remotes/}
	fi
	
	if [ -n "$hash" ]
	then
		for line in `git tag --list --points-at "$hash"`
		do
			tags=$tags${tags:+$CYAN,}$BCYAN$line
		done
		
		prefix="$hash refs/heads/"
		for line in `git show-ref | grep "^$prefix"`
		do
			line=${line:${#prefix}}
			[ "$line" != "$branch" ] && branches=$branches${branches:+$MAGENTA,}$BMAGENTA$line
		done
	fi
	
	for line in `test $isthereworktree && git status --porcelain ${GIT_PROMPT_SHOW_IGNORED:+--ignored}`
	do
		line=${line//\?/N}
		line=${line//\!/E}
		X=${line:0:1}
		Y=${line:1:1}
		case "$X$Y"
		in
			AA|DD)
				let $X$Y++
				;;
			[AD]U|U[ADU])
				let U++
				;;
			*)
				[ "$X" != ' ' ] && let ${X}X++
				[ "$Y" != ' ' ] && let ${Y}Y++
				;;
		esac
	done
	
	line=`git rev-list --count --left-right "$(git show-ref --quiet --verify refs/remotes/origin/HEAD && echo origin/HEAD || get_tracking_branch master)...HEAD" 2>/dev/null`
	behind=${line%	*}
	ahead=${line#*	}
	
	if trueish "${GIT_PROMPT_COUNT_LINES-true}"
	then
		for line in `test $isthereworktree && git diff --numstat --find-copies=100`
		do
			line=${line%	*}
			line=${line//-/0}
			let adds1+=${line%	*}
			let dels1+=${line#*	}
		done
		
		for line in `git diff --numstat --staged --find-copies=100`
		do
			line=${line%	*}
			line=${line//-/0}
			let adds2+=${line%	*}
			let dels2+=${line#*	}
		done
	fi
	
	for var in ahead behind adds1 dels1 adds2 dels2
	do
		[ "${!var}" = 0 ] && eval $var=
	done
IFS=$IFS_
stash=$(git stash list -s --format=%H 2>/dev/null | wc -l)


if [ -d "$gitdir/rebase-merge" ]
then
	branch=`cat "$gitdir/rebase-merge/head-name"`
	if [ -f "$gitdir/rebase-merge/interactive" ]
	then
		speclist+=(REBASE-I)
	else
		speclist+=(REBASE-M)
	fi
fi
if [ -d "$gitdir/rebase-apply" ]
then
	if [ -f "$gitdir/rebase-apply/rebasing" ]
	then
		speclist+=(REBASE)
	elif [ -f "$gitdir/rebase-apply/applying" ]
	then
		speclist+=(AM)
	else
		speclist+=(AM/REBASE)
	fi
fi
[ -f "$gitdir/MERGE_HEAD" ] && speclist+=(MERGE)
[ -f "$gitdir/CHERRY_PICK_HEAD" ] && speclist+=(CHERRYPICK)
[ -f "$gitdir/BISECT_LOG" ] && speclist+=(BISECT)


ahash=$MAGENTA${hash:0:7}${hash:+ }
specdir=${specdir:+$BLACK$YELLOWBG$specdir$RESET }
for line in "${speclist[@]}"
do
	spec="$spec${spec:+ }$BWHITE$REDBG$line$RESET"
done
if [ -z "$branch" ]
then
	spec="$spec${spec:+ }$BWHITE${MAGENTABG}HEAD$RESET"
fi
if [ -n "$branch$tags" ]
then
	local branchesglue=
	if [ -n "$branches" ]
	then
		[ -n "$branch" ] && branchesglue="$MAGENTA," || branchesglue=" $BRED$sign_desc "
	fi
	branches=${branch:+ $MAGENTA$sign_branch $BMAGENTA$branch}${pushpull:+$BYELLOW$pushpull}$branchesglue$branches
	tags=${tags:+ $CYAN$sign_tags $tags}
	pointer=$branches$tags
else
	if [ -n "$desctag" ]
	then
		pointer=" $BRED$sign_desc $BCYAN$desctag"
	elif [ -n "$descbranch" ]
	then
		pointer=" $BRED$sign_desc $BMAGENTA$descbranch"
	fi
fi
ahead=${ahead:+$BBLUE$sign_ahead$RESET$ahead}
behind=${behind:+$BBLACK$sign_behind$RESET$behind}
divergence=$ahead$behind

if [ $stash = 0 ]
then
	stash=
elif [ $stash = 1 ]
then
	stash="$BBLUE$sign_stash"
else
	stash="$BBLUE$sign_stash$MAGENTA$stash"
fi

unstag=${NY:+ ${BCYAN}N$BWHITE$NY}${AY:+ ${BGREEN}A$BWHITE$AY}${DY:+ ${BRED}D$BWHITE$DY}${MY:+ ${BYELLOW}M$BWHITE$MY}${TY:+ ${BBLUE}T$RESET$TY}${CY:+ ${BGREEN}C$RESET$CY}${RY:+ ${BYELLOW}R$BWHITE$RY}${AA:+ ${BBLUE}A$BWHITE$AA}${DD:+ ${BBLUE}D$BWHITE$DD}${U:+ ${BBLUE}U$BWHITE$U}
staged=${AX:+ ${GREEN}A$RESET$AX}${DX:+ ${RED}D$RESET$DX}${MX:+ ${YELLOW}M$RESET$MX}${TX:+ ${BLUE}T$RESET$TX}${CX:+ ${GREEN}C$RESET$CX}${RX:+ ${YELLOW}R$RESET$RX}
unstag=${unstag:+$YELLOW$sign_unstag$unstag}
staged=${staged:+$BWHITE$sign_staged$staged}

[ -n "$adds1$dels1" ] && delta1="${adds1:+$BGREEN+$adds1}${dels1:+$BRED-$dels1}$RESET" || delta1=
[ -n "$adds2$dels2" ] && delta2="${adds2:+$GREEN+$adds2}${dels2:+$RED-$dels2}$RESET" || delta2=
delta1=${delta1:+$BBLACK[$delta1$BBLACK]}
delta2=${delta2:+$BBLACK[$delta2$BBLACK]}

if [ -z "$unstag$staged" -a -n "$hash" ]
then
	pointer="$pointer  $GREEN$(git show -s --format=%cr)"
	[ "$isthereworktree" = 1 -a -z "$pushpull" ] && pointer="$pointer${pointer:+ }$BGREEN$sign_clean"
fi


__git_prompt="$ahash$specdir$spec$pointer${divergence:+  }$divergence${EY:+  ${BRED}!$BWHITE$EY}${unstag:+  }$unstag${delta1:+ }$delta1${stash:+  }$stash${staged:+  }$staged${delta2:+ }$delta2"
echo -n "$__git_prompt$RESET
"
}
