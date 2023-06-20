
for var in `locale-validator`
do
	eval "x=\$$var"
	if [ -n "$x" ]
	then
		echo "locale setting '$var=$x' is unavailable. unsetting." >&2
		unset $var
	fi
	unset x
done
unset var
