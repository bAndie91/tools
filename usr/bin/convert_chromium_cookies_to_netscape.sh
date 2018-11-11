#!/bin/bash

set -e
set -u

if [ -z "${1:-}" ]
then
	sql_where=1
else
	hostset=''
	for host in "$@"
	do
		hostset="$hostset${hostset:+,}'.$host','$host'"
	done
	sql_where="host_key IN ($hostset)"
fi

vivaldiCookieFile=~/.config/vivaldi/Default/Cookies

LD_PRELOAD=/usr/lib/yazzy-preload/ignore_read_lock.so \
sqlite3 -noheader "$vivaldiCookieFile" .dump 2>/dev/null |\
{
sed -e '/^CREATE INDEX/d'
echo "SELECT host_key, path, name, secure,
	strftime('%s', datetime(expires_utc / 1000000 + (strftime('%s', '1601-01-01')), 'unixepoch')),
	hex(encrypted_value)
	FROM cookies WHERE $sql_where;"
}|\
sqlite3 -list -noheader :memory: 2>/dev/null |\
CHROMIUM_DECRYPT_PASS=peanuts chromium_cookie_decrypt.py 5 |\
{
while read -r host_key path name secure expireTimestamp value
do
	host_key=${host_key#.}
	[ secure = 0 ] && issecure=FALSE || issecure=TRUE
	
	echo "$host_key	TRUE	$path	$issecure	$expireTimestamp	$name	$value"
done
}
