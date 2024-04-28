
default:
	false
.PHONY: default


define remove
@[ ! -e $(1) ] || rm -v $(1)
endef


/etc/profile.d/locale-validator.sh: locale-validator.sh
	install $< $@
	@echo remove $@ >> uninstall.sh


install-dmaster: /sbin/dmaster /usr/share/man/man8/dmaster.8.xz
	@echo "Don't forget to install /sbin/dmaster as a system service (eg. put in /etc/inittab)"
.PHONY: install-dmaster

/sbin/dmaster: dmaster
	install $< $@

/usr/share/man/man8/dmaster.8.xz: dmaster
	pod2man --name="$<" --section 8 --utf8 "$<" | xz > "$@.tmp" &&\
	mv -f "$@.tmp" "$@"

uninstall-dmaster:
	$(call remove,/sbin/dmaster)
	$(call remove,/usr/share/man/man8/dmaster.1.xz)
.PHONY: uninstall-dmaster


install-all: install-dmaster /etc/profile.d/locale-validator.sh
.PHONY: install-all
