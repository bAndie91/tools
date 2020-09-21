
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
if [[ ! $1 =~ /.getmail/ && $1 =~ ^(.*/)([^/]+)\[(.+?)\]\.eml$ ]]
then
	echo "${BASH_REMATCH[1]}${BASH_REMATCH[3]}"
else
	echo "$1"
fi
}
