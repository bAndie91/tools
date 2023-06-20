
install-basedir-lib:
	mkdir -p /usr/lib/tool

install-bc-functions: install-basedir-lib
	install --compare functions.bc /usr/lib/tool/
	install --compare bc-functions.sh /etc/profile.d/

install-all: install-bc-functions
