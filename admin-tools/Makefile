
default:
	false
.PHONY: default

/etc/profile.d/locale-validator.sh: locale-validator.sh
	install $< $@
	@echo remove $@ >> uninstall.sh


install-dmaster: /sbin/dmaster
	@echo "Don't forget to install /sbin/dmaster as a system service (eg. put in /etc/inittab)"
.PHONY: install-dmaster

/sbin/dmaster: dmaster
	install $< $@

uninstall-dmaster:
	[ ! -e /sbin/dmaster ] || rm /sbin/dmaster
.PHONY: uninstall-dmaster


install-all: install-dmaster /etc/profile.d/locale-validator.sh
.PHONY: install-all
