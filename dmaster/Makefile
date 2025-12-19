
MANFILE_COMPRESSOR = xz
MANFILE_EXT = xz


default:
	$(error hint: make install)
.PHONY: default


.PHONY: install
install: /sbin/dmaster /sbin/dmaster-signal /sbin/dmaster-dump /sbin/dmaster-reload \
 /usr/share/man/man8/dmaster.8.$(MANFILE_EXT) /usr/share/man/man8/dmaster-signal.8.$(MANFILE_EXT) /usr/share/man/man5/daemontab.5.$(MANFILE_EXT)
	$(warning Don't forget to install /sbin/dmaster as a respawn process, eg. put in /etc/inittab, see "man 8 dmaster" for examples.)

/sbin/dmaster /sbin/dmaster-signal /sbin/dmaster-dump /sbin/dmaster-reload: /sbin/%: %
	install $< $@

define install_manpage
	pod2man --name="$(3)" --section $(2) --utf8 "$(1)" | $(MANFILE_COMPRESSOR) > "$(4).tmp" &&\
	mv -f "$(4).tmp" "$(4)"
endef

/usr/share/man/man8/dmaster.8.$(MANFILE_EXT) /usr/share/man/man8/dmaster-signal.8.$(MANFILE_EXT): /usr/share/man/man8/%.8.$(MANFILE_EXT): %
	$(call install_manpage,$<,8,$<,$@)

/usr/share/man/man5/daemontab.5.$(MANFILE_EXT): dmaster
	$(call install_manpage,$<,5,daemontab,$@)


dmaster.troff dmaster-signal.troff: %.troff: %
	pod2man --name="$(basename $<)" --section 8 --utf8 "$<" > "$@~" && mv "$@~" "$@"



define remove
@[ ! -e $(1) ] || rm -v $(1)
endef

.PHONY: uninstall-dmaster
uninstall-dmaster:
	$(call remove,/sbin/dmaster)
	$(call remove,/sbin/dmaster-signal)
	$(call remove,/sbin/dmaster-dump)
	$(call remove,/usr/share/man/man8/dmaster.8.$(MANFILE_EXT))
	$(call remove,/usr/share/man/man8/dmaster-signal.8.$(MANFILE_EXT))
	$(call remove,/usr/share/man/man5/daemontab.5.$(MANFILE_EXT))
