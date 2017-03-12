
# Colorful git prompt for bash with symbols

Source `git-prompt.sh` into your bash session.
Prepend the following string to your PS1: `$(__git_prompt_print)`

# Examples

You are standing on the master branch, which was committed 13 hours ago 
and there were no changes so far.
![master-clean](demo/master-clean.png)

Same as above, but there is 1 file ignored by git. 
You can disable showing ignored files by setting `GIT_PROMPT_SHOW_IGNORED=` bash variable (empty string), 
enable by `GIT_PROMPT_SHOW_IGNORED=1`.
![master-clean-ignored](demo/master-clean-ignored.png)

A star next to the branch name indicates the branch is not in synchron with its upstream. 
Either push or poll is needed or even the remote branch is not exists.
![master-unpushed](demo/master-unpushed.png)

You are on an unsynchronized branch, which is ahead by 1 and behind by 5 commits from the master.
![divergent-unpushed](demo/divergent-unpushed.png)

You are on an unsynchronized branch, which is ahead by 1 and behind by 14 commits from the master.
You have 1 untracked file (or directory) in the working tree.
![unclean-untracked](demo/unclean-untracked.png)

You are on the master branch, but dev branch is also stays here.
The current commit has a tag.
![branches-tag](demo/branches-tag.png)

You are on dev branch which is not pushed to the upstream.
![branches-unpushed-tag](demo/branches-unpushed-tag.png)

There are changes in stage (plan icon), 1 file being added, 1 deleted, 1 edited, and 1 renamed in the next commit. 
You can see how many lines are added and removed.
Only lines in text files are counted, not binary blocks.
You can disable the costly line counting operation by setting bash variable `GIT_PROMPT_COUNT_LINES=` (empty string), default is enabled.
![branches-unpushed-tag-staged](demo/branches-unpushed-tag-staged.png)

The working tree is edited (pencil icon).
You can see the same status letters as in stage.
![branches-unpushed-tag-unclean-staged](demo/branches-unpushed-tag-unclean-staged.png)

Full circle indicates stashed files.
![branches-unpushed-stash-staged](demo/branches-unpushed-stash-staged.png)

You have unstaged, staged, and stashed changes.
![branches-unpushed-unclean-stash-staged](demo/branches-unpushed-unclean-stash-staged.png)

You have 2 stash entries in a clean working tree.
![branches-unpushed-stash](demo/branches-unpushed-stash.png)

You are in a detached HEAD state.
Current commint is behind the master branch by 3 commits.
![detached-bybranch](demo/detached-bybranch.png)

Detached HEAD state.
Current commit is expressed by the relation to the next tag.
This is the default behavior if there is a tag, otherwise branch names are used.
![detached-bytag](demo/detached-bytag.png)

You are in BISECT mode.
There is 1 merge conflict denoted by `U1`.
![bisect-unclean-staged](demo/bisect-unclean-staged.png)

Current working directory is under `.git`.
You are also in MERGE mode.
![gitdir-merge-divergent](demo/gitdir-merge-divergent.png)
