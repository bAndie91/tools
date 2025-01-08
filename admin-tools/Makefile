
default:
	false
.PHONY: default


define remove
@[ ! -e $(1) ] || rm -v $(1)
endef


/etc/profile.d/locale-validator.sh: locale-validator.sh
	install $< $@
	@echo remove $@ >> uninstall.sh

uninstall-locale-validator:
	$(call remove,/etc/profile.d/locale-validator.sh)
.PHONY: uninstall-locale-validator



install-dmaster: /sbin/dmaster /sbin/dmaster-signal /sbin/dmaster-dump /usr/share/man/man8/dmaster.8.xz /usr/share/man/man8/dmaster-signal.8.xz /usr/share/man/man5/daemontab.5.xz
	@echo "Don't forget to install /sbin/dmaster as a system service (eg. put in /etc/inittab)"
.PHONY: install-dmaster

/sbin/dmaster /sbin/dmaster-signal /sbin/dmaster-dump: /sbin/%: %
	install $< $@

/usr/share/man/man8/dmaster.8.xz /usr/share/man/man8/dmaster-signal.8.xz: /usr/share/man/man8/%.8.xz: %
	pod2man --name="$<" --section 8 --utf8 "$<" | xz > "$@.tmp" &&\
	mv -f "$@.tmp" "$@"

/usr/share/man/man5/daemontab.5.xz: dmaster
	pod2man --name=daemontab --section 5 --utf8 "$<" | xz > "$@.tmp" &&\
	mv -f "$@.tmp" "$@"


uninstall-dmaster:
	$(call remove,/sbin/dmaster)
	$(call remove,/sbin/dmaster-signal)
	$(call remove,/sbin/dmaster-dump)
	$(call remove,/usr/share/man/man8/dmaster.8.xz)
	$(call remove,/usr/share/man/man8/dmaster-signal.8.xz)
	$(call remove,/usr/share/man/man5/daemontab.5.xz)
.PHONY: uninstall-dmaster



install-dhcpdiscover: depcheck-dhcpdiscover /sbin/dhcpdiscover
.PHONY: install-dhcpdiscover
/sbin/dhcpdiscover: dhcpdiscover
	install $< $@
uninstall-dhcpdiscover:
	$(call remove,/sbin/dhcpdiscover)
.PHONY: uninstall-dhcpdiscover

.PHONY: depcheck-dhcpdiscover
depcheck-dhcpdiscover:
	python -c 'import scapy'


install-all: install-dmaster /etc/profile.d/locale-validator.sh install-dhcpdiscover /usr/sbin/wps-pbc-approve
.PHONY: install-all

uninstall-all: uninstall-dmaster uninstall-locale-validator uninstall-dhcpdiscover
.PHONY: uninstall-all

/usr/sbin/wps-pbc-approve: wps-pbc-approve
	install $< $@
