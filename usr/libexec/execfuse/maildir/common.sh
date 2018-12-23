
virt2phy_filepath()
{
if [[ ! $1 =~ /.getmail/ && $1 =~ ^(.*/)([^/]+)\[(.+?)\]\.eml$ ]]
then
	echo "${BASH_REMATCH[1]}${BASH_REMATCH[3]}"
else
	echo "$1"
fi
}
