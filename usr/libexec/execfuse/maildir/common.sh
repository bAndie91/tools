
EPERM=1
ENOENT=2
EIO=5
EACCESS=13
EEXIST=17
EINVAL=22
ENOSYS=38
EOPNOTSUPP=95

virt2phy_filepath()
{
local virtpath=$1
local basedir=$2

if [[ $1 =~ /.getmail/ ]]
then
	echo "$1"
else
	if [[ $1 =~ ^(.*/)([^/]+)\{inode=([0-9]+?)\}\.eml$ ]]
	then
		local pathprefix=${BASH_REMATCH[1]}
		local inum=${BASH_REMATCH[3]}
		fname=`find "$basedir$pathprefix" -maxdepth 1 -inum $inum -printf "%f"`
		echo "$pathprefix$fname"
	elif [[ $1 =~ ^(.*/)([^/]+)\[(.+?)\]\.eml$ ]]
	then
		echo "${BASH_REMATCH[1]}${BASH_REMATCH[3]}"
	else
		echo "$1"
	fi
fi
}
