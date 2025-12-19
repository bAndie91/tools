# NAME

dmaster - Manage continuously running processes

## SYNOPSIS

dmaster

## DESCRIPTION

Daemon master.
Start programs defined in **daemontab** file.
Restart them when ended.

Re-read the **daemontab** file on **HUP** signal,
stops processes which are no longer in it (or _ID_ changed) with SIGTERM signal,
and starts new ones.
Those daemons whose _COMMAND_ _ARGS_ changed, are not restarted automatically.

Automatically respawn a daemon when it exits.
If a daemon exits too often, suppress it a little while.
Default parameters: at most 5 spawns in 2 sec, hold up for 10 sec.

## FILES

- `/etc/daemontab`

    Format is _ID_ _COMMAND_ _ARGS_, delimited by any amount of space/tab, one line per each.
    _ID_ is an arbitrary word choosed by the user, to identify daemon commands across reloads.
    Command arguments may enclosed in double quotes (`"`), but not part of the argument. See:

        d1 my-daemon --param="foo bar"

    is invalid (parsed as 2 parameters: `--param="foo` and `bar"` with dangling quotes).

        d1 my-daemon "--param=foo bar"

    is what you want.

    It's not recommended to write complex scripts directly in daemontab.
    Put them in `/etc/daemons` directory instead.

- `/var/run/dmaster.sock`

    dmaster(1) listens on a Unix Domain socket (see unix(7)).
    When a client connects, internal state is dumped on the socket
    in **Single Value per Line with Hierarchical Keys** format.

    Internal state includes all started daemon's **PIDFD**,
    which may be copied by an other program by pidfd\_getfd(2),
    in order to reliably send signals to it.
    After the internal state dump is closed by a line containing a lone `END`,
    it reads one line on the management socket.
    Currently you can not control dmaster(1) this way (use signals for this, see ["SIGNALS"](#signals)),
    but while it waits for that one line, does not do anything,
    so it's time for the management client side to call pidfd\_getfd(2)
    to reliably copy the PIDFD representing a given daemon process.

    **Note**, this also means that daemon management is paused until
    normal operation is resumed by sending a newline on the management socket.
    Due to this potential DoS vector, management socket is chmod(2) to 0660.

- `/var/run/shm/dmaster.state`

    Dump internal state to it.
    See USR1 signal below.

## SIGNALS

- USR1

    Write internal state into `/var/run/shm/dmaster.state` file.

- TERM, INT

    Terminate managed daemons, always with one **TERM** signal each,
    wait for all of them to exit, then exit dmaster(1) itself.
    If a daemon is a process group leader (become on its own or started via setpgrp(1)), sends the signal to the whole process group at once.

- HUP

    Re-read **daemontab**, stop daemons which are disappeared and start new ones.
    Leave others running.

## ENVIRONMENT

- XDG\_CONFIG\_HOME

    Where to find `daemontab`.
    Default is `/etc`.

- XDG\_CACHE\_HOME

    Where to create `dmaster.sock`
    Default is `/var/run`.

- DAEMONS\_RESPAWN\_BURST\_PERIOD

    Measure this much time (seconds) to detect respawn-burst.

- DAEMONS\_RESPAWN\_BURST\_LIMIT

    If a daemon respawns this many times within DAEMONS\_RESPAWN\_BURST\_PERIOD,
    consider it a respawn-burst, and hold it back a while.

- DAEMONS\_RESPAWN\_BURST\_HOLD

    How much time (seconds) to wait before starting a daemon
    after a respawn-burst.

## EXAMPLE INSTALLATION

Put this in your inittab(5):

    dm:2345:respawn:loggerexec daemon dmaster /sbin/dmaster

This one removes the socket file in case of it's left there:

    dm:2345:respawn:multicmd -d -- rm /var/run/dmaster.sock -- loggerexec daemon dmaster dmaster

May use the default (although a bit confusing) semicolon multicmd(1) delimiter if your init(1) supports the "at" mark (`@`) syntax:

    dm:2345:respawn:@multicmd rm /var/run/dmaster.sock ; loggerexec daemon dmaster dmaster

Don't forget to place this inittab(5) entry above any entry with a **wait** action.

## SEE ALSO

supervisor(1), supervise(8), daemon(1), runit(1), [sinit](https://core.suckless.org/sinit/), setpgrp(1)

# NAME

dmaster-signal - Send a signal to a daemon managed by dmaster

## SYNOPSIS

dmaster-signal _DAEMON-ID_ _SIGNAL_

## DESCRIPTION

dmaster-signal(1) utilizes PIDFD to reliably send signal to the daemon
managed by dmaster(1) and specified by _DAEMON-ID_.

_SIGNAL_ is either numeric or symbolic signal name.

## SEE ALSO

dmaster(1), pidfd\_send\_signal(2), signal(7)
