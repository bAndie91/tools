# tools

Various bash/perl/python/etc scripts, native cli programms and libs for convenient work on Linux

- see [descriptions](descriptions.md)

## install and uninstall

`make install` or `make install-all` works in most of the directories to install a given tool
and set of tools. It records what files are installed in system directories in `uninstall.sh`
in the cwd, so you may `sh uninstall.sh` to remove installed files. It's loosely maintained,
so there may be duplicated `remove file.xyz` commands in it, but there are all installed files
occurring in it at least once. It does not remove directories recursively, so the user decides
what to do with directories installed by `make install` but with files in it which are not.
You may also run `uninstall.sh` several times if the `remove xyz-directory` is before
`remove xyz-directory/file.xyz`.
