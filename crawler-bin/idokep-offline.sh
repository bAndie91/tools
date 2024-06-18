#!/bin/bash

set -e

httrack -%v2 --continue --include-query-string=0 --purge-old=0 --keep-alive --can-go-down --can-go-up --stay-on-same-dir \
	--depth=1 -%P --near --index=0 --path . \
	-A10000000 -%c100 --update \
	https://www.idokep.hu/idojaras/Mak%C3%B3 https://www.idokep.hu/elorejelzes/Mak%C3%B3 \
	'-*' '+*.idokep.hu/*' '-www.idokep.hu/keptar/users/*' '-www.idokep.hu/keptar/income/vago/*'

perl -pe 's{^<html.*}{<html>}' -i index.html

cd www.idokep.hu

cat css/*.css | perl -ne 'for $m (m{url\("?(?!"?data:)(.+?)[?#")]}g){ print "$m\n" }' |\
sd '^\.\./+' '' |\
sd '^/+' '' |\
while read path
do
	[ ! -s "$path" ] && echo "$path"
done |\
sort -u |\
rcmod any=0 wget --base=https://www.idokep.hu/ -i - --no-host-directories --force-directories


export banner_text_start='<!-- banner_text start -->'
export banner_text_end='<!-- banner_text end -->'

for htmlpath in idojaras/Makó.html elorejelzes/Makó.html
do
	mtime_ts=`stat -c %Y "$htmlpath"`
	mtime_dt=`date -d@$mtime_ts`
	export banner_text="<div class=\"row\"><div class=\"col\" style=\"font-size: 20px; color: black;\">Frissítve ekkor: $mtime_dt</div></div>"
	perl -pe 's{(id="menubarDesktop".*?)($ENV{banner_text_start}(.+)$ENV{banner_text_end}|)$}{$1$ENV{banner_text_start}$ENV{banner_text}$ENV{banner_text_end}}' -i "$htmlpath"
done
