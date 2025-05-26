# tools

Various bash/perl/python/etc scripts, native cli programms and libs for convenient work on Linux

- [cli scripts for end-users](user-tools/descriptions.txt)
- [cli tools for system administation](admin-tools/descriptions.txt)
- program suites
  - [queue management](queue-mgmt/README.md)
  - [tabular data management](tabdata/README.md)
- [low-level programs and cli tools, compiled](compiled-tools/descriptions.txt)
- [tools intended to run with superuser privileges](root-tools/descriptions.txt)
  - [mount helper scripts for various (fuse) filesystems](mount/)
- [tools with graphical iterface for end-users](xgui-tools/descriptions.txt)
- [tools to interact with other graphical programs and Xorg](xwin-tools/descriptions.txt)
- many other...

See also [descriptions](descriptions.md).

## install and uninstall

`make install` or `make install-all` works in most of the directories to install a given tool
and set of tools. It records what files are installed into system directories in `uninstall.sh`
file in the cwd, so you may run `sh uninstall.sh` to remove installed files. It's loosely maintained,
so there may be duplicated `remove file.xyz` commands in it, but there are all installed files
occurring in it at least once. It does not remove directories recursively, so the user decides
what to do with directories installed by `make install` and having files in it which are not
installed by `make`.
You may also run `uninstall.sh` several times if `remove xyz-directory`-like lines are before
`remove xyz-directory/file.xyz`.

# Project issues, bugs, feature requests, ideas

1. clone the repo
2. use [git-bug](https://github.com/git-bug/git-bug) to open a new ticket in this repo
3. find one or more person in the commit history to make contact with, then either

   a. send your newly created `git-bug` ticket (or patch if you already propose a code change) via email, or

   b. send the URL of your git clone to the contributor(s), via email or other channel, requesting them to pull (`git-bug` issues and/or branches as well) from you.
