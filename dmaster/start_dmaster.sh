#!/bin/sh

set -e

if [ -e /var/run/dmaster.sock ]
then
	# verify that nothing is listening on it
	! lsof -nPw /var/run/dmaster.sock
	rm /var/run/dmaster.sock
fi

exec /usr/tool/set-sys-path \
	saveout --out /var/log/dmaster/%F.log --flush-sec=1 \
		pipecmd \
			redirexec --stderr-fd=stdout dmaster \
			-- \
			ts "%F %H:%M:%.S %z"
