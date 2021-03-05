This program (call it `sh.ssh` or `bash.ssh`) is intended to set as login shell for users
who should be limited what they can run over ssh.

`sh.ssh` looks up which commands (shell command lines) are allowed by the invoking 
user's group membership. It looks up `/etc/ssh/AllowGroupCommands` file.


Using `sh.ssh` as login shell enables sysadmins to:

- allow `~/.ssh/rc` system wide
  - but don't allow it here anyone only for certain groups
- users still can write their files including `~/.ssh/rc`
- allow both password and pubkey auth
  - because running `~/.ssh/rc` can only by disable globally or per-pubkey
- while not worrying about arbitrary commands called by `~/.ssh/rc`
  - because it is not allowed here only for certain groups


However being a login shell does not only play role at login, other programs like
to invoke it when they run any shell command line, so `sh.ssh` have check whether it is
invoked as part of a login process or just casually. So it has a "controlled" mode 
and a "permissive" mode. The logic is the following:

- is `SHELL_DIRECTLY_BY_SSHD` env var set? -> controlled mode
  - it's a good idea to set it by `/etc/pam.d/sshd`.
    eg: `session required pam_env.so envfile=/etc/ssh/env user_readenv=0`
    and put `SHELL_DIRECTLY_BY_SSHD=1` in `/etc/ssh/env`.
- parent process owner different than us or is it root? -> controlled mode
- parent process execuable ends in `/sshd` or `/dropbear`? -> controlled mode
- otherwise -> permissive mode
  - it's highly recommended to set `SHELL_DIRECTLY_BY_SSHD` environment 
    because process ownership and process command name checks are less reliable 
    and is not robostly tested on different systems.
