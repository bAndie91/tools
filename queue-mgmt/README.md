# Qrun

- no daemon
- add, remove, list jobs by dedicated commands: `qadd`, `qrm`, `qls`
- user decides when and where to run the next job by `qrun`
  - may put `qrun` in scheduler (cron) or at the end of each submitted jobs;
  making the queue "automatically running" this way
- run jobs in series
- default queue control dir is per-user
- record command, environment, PID, exit status, etc. in separate files
- pending jobs are files in the queue control dir
- leave log management to each job
- queue IDs are sequential numbers

# Similar tools/projects

- [at](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/at.html)
  - very mature tool
  - present everywhere
  - run jobs at a given time
  - run as a daemon (atd)
  - queue control dir is system-wide, multiuser
  - queue IDs are sequential numbers
- [nq](https://github.com/leahneukirchen/nq)
  - no daemon
  - start jobs right away
  - run jobs in series
  - default queue dir is the CWD
  - record command, output, and exit status in a single file
  - list jobs by `ls -F ,*`
  - pending jobs are processes waiting for a lock
  - queue IDs are timestamps+PID
- [task spooler](https://vicerveza.homeunix.net/~viric/soft/ts/)
  - per-user daemon, start on-demand
- [pueue](https://github.com/Nukesor/pueue)
