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
local pushpull=

local X Y
local MX= AX= DX= RX= CX= UX= QX=
local MY= AY= DY= RY= CY= UY= QY=
local unstag staged
local specdir= spec=

local add del rest
local adds1= dels1= adds2= dels2=
local delta1 delta2

local sign_branches=❦
local sign_tags=➼
local sign_desc=☛
local sign_ahead=▲
local sign_behind=▼
local sign_unstag=✎
local sign_staged=✈
#local sign_stash=⌂
#local sign_stash=❂
local sign_stash=●
local sign_pushpull=★


gitdir=`git rev-parse --git-dir 2>/dev/null` || return
hash=`git show -s --format=%H`

for color in BLACK RED GREEN YELLOW BLUE MAGENTA CYAN WHITE
do
	local $color B$color ${color}BG
	eval "$color='[0;3${n}m'"
	eval "B$color='[1;3${n}m'"
	eval "${color}BG='[4${n}m'"
	let n++
done
RESET='[0m'

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
		line=`git rev-list --count --left-right "$branch...origin/$branch" 2>/dev/null`
		# Empty $line means branch is not remotely tracked.
		if [ -z "$line" ] || [ ${line%	*} -gt 0 -o ${line#*	} -gt 0 ]
		then
			pushpull=yes
		fi
	else
		desctag=`git describe --contains --tags HEAD 2>/dev/null`
		descbranch=`git describe --contains --all HEAD 2>/dev/null`
		descbranch=${descbranch#remotes/}
	fi
	
	for line in `git tag --list --points-at "$hash"`
	do
		tags=$tags${tags:+$CYAN,}$BCYAN$line
	done
	
	prefix="$gitdir"/refs/heads/
	for line in `grep "$hash" -x -l -r "$prefix"`
	do
		line=${line:${#prefix}}
		[ "$line" != "$branch" ] &&	branches=$branches${branches:+$MAGENTA,}$BMAGENTA$line
	done
	
	for line in `test $isthereworktree && git status --porcelain`
	do
		line=${line//\?/Q}
		X=${line:0:1}
		Y=${line:1:1}
		[ "$X" != ' ' ] && let ${X}X++
		[ "$Y" != ' ' ] && let ${Y}Y++
	done
	
	line=`git rev-list --count --left-right master...HEAD`
	behind=${line%	*}
	ahead=${line#*	}
	
	if [ "${GIT_PS1_COUNT_LINES-1}" ]
	then
		for line in `test $isthereworktree && git diff --numstat`
		do
			line=${line%	*}
			let adds1+=${line%	*}
			let dels1+=${line#*	}
		done
		
		for line in `git diff --numstat --staged`
		do
			line=${line%	*}
			let adds2+=${line%	*}
			let dels2+=${line#*	}
		done
		
		for var in ahead behind adds1 dels1 adds2 dels2
		do
			[ "${!var}" = 0 ] && eval $var=
		done
	fi
IFS=$IFS_
stash=$(git stash list -s --format=%H 2>/dev/null | wc -l)


if [ -d "$gitdir/rebase-merge" ]
then
	branch=`cat "$gitdir/rebase-merge/head-name"`
	spec=REBASE-M
	if [ -f "$gitdir/rebase-merge/interactive" ]
	then
		spec=REBASE-I
	fi
else
	if [ -d "$gitdir/rebase-apply" ]
	then
		if [ -f "$gitdir/rebase-apply/rebasing" ]
		then
			spec=REBASE
		elif [ -f "$gitdir/rebase-apply/applying" ]
		then
			spec=AM
		else
			spec=AM/REBASE
		fi
	elif [ -f "$gitdir/MERGE_HEAD" ]
	then
		spec=MERGE
	elif [ -f "$gitdir/CHERRY_PICK_HEAD" ]
	then
		spec=CHERRYPICK
	elif [ -f "$gitdir/BISECT_LOG" ]
	then
		spec=BISECT
	fi
fi


hash=$MAGENTA${hash:0:7}
specdir=${specdir:+ $BLACK$YELLOWBG$specdir$RESET}
spec=${spec:+ $BWHITE$REDBG$spec$RESET}
if [ -z "$branch" ]
then
	spec="$spec $BWHITE${MAGENTABG}HEAD$RESET"
fi
if [ -n "$branch$tags" ]
then
	local branchesglue=
	if [ -n "$branches" ]
	then
		[ -n "$branch" ] && branchesglue=" $sign_desc " || branchesglue=,
		branchesglue=$MAGENTA$branchesglue
	fi
	branches=${branch:+ $MAGENTA$sign_branches $BMAGENTA$branch}${pushpull:+$BYELLOW$sign_pushpull}$branchesglue$branches
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

unstag=${AY:+ ${BGREEN}A$BWHITE$AY}${DY:+ ${BRED}D$BWHITE$DY}${MY:+ ${BYELLOW}M$BWHITE$MY}${TY:+ ${BBLUE}T$RESET$TY}${CY:+ ${BGREEN}C$RESET$CY}${RY:+ ${BYELLOW}R$BWHITE$RY}${UY:+ ${BBLUE}U$BWHITE$UY}${QY:+ ${BRED}?$BWHITE$QY}
staged=${AX:+ ${GREEN}A$RESET$AX}${DX:+ ${RED}D$RESET$DX}${MX:+ ${YELLOW}M$RESET$MX}${TX:+ ${BLUE}T$RESET$TX}${CX:+ ${GREEN}C$RESET$CX}${RX:+ ${YELLOW}R$RESET$RX}${UX:+ ${BLUE}U$RESET$UX}
unstag=${unstag:+$YELLOW$sign_unstag$unstag}
staged=${staged:+$BWHITE$sign_staged$staged}

[ -n "$adds1$dels1" ] && delta1="${adds1:+$BGREEN+$adds1}${dels1:+$BRED-$dels1}$RESET" || delta1=
[ -n "$adds2$dels2" ] && delta2="${adds2:+$GREEN+$adds2}${dels2:+$RED-$dels2}$RESET" || delta2=
delta1=${delta1:+$BBLACK[$delta1$BBLACK]}
delta2=${delta2:+$BBLACK[$delta2$BBLACK]}

if [ -z "$unstag$staged" ]
then
	pointer="$pointer${pointer:+ }$GREEN"`git show -s --format=%cr`
fi


__git_prompt="$hash$specdir$spec$pointer${divergence:+  }$divergence${unstag:+  }$unstag${delta1:+ }$delta1${stash:+  }$stash${staged:+  }$staged${delta2:+ }$delta2"
echo -n "$__git_prompt$RESET
"
}
