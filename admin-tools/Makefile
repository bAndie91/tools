
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


install-all: /etc/profile.d/locale-validator.sh install-dhcpdiscover install-hostapd-tools
.PHONY: install-all

uninstall-all: uninstall-locale-validator uninstall-dhcpdiscover
.PHONY: uninstall-all

install-hostapd-tools: /usr/sbin/wps-pbc-approve /usr/sbin/wifi-new-psk
.PHONY: install-hostapd-tools

/usr/sbin/wps-pbc-approve /usr/sbin/wifi-new-psk: /usr/sbin/%: %
	install $< $@
