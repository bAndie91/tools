
install-basedir-lib:
	mkdir -p /usr/lib/tools

install-bc-functions: install-basedir-lib
	install --compare functions.bc /usr/lib/tools/
	install --compare bc-functions.sh /etc/profile.d/

install-all: install-bc-functions
