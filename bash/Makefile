
INSTALL_WRAPPER =

define mkdir-recipe
	[ -d $@ ] || $(INSTALL_WRAPPER) mkdir -p $@
	@echo remove $@ >> uninstall.sh
endef


.PHONY: default
default:
	$(error try `make INSTALL_WRAPPER=sudo install-bash-command-not-found-handle install-bash-utils´)


.PHONY: install-bash-command-not-found-handle
install-bash-command-not-found-handle: /etc/profile.d/bash.d/command-not-found-handle.sh


/etc/profile.d/bash.d/command-not-found-handle.sh: command-not-found-handle.sh | /etc/profile.d/bash.d
	$(INSTALL_WRAPPER) install --compare $< $@
	@echo remove $@ >> uninstall.sh

/etc/profile.d/bash.d:
	$(mkdir-recipe)


LIBPREFIX = /usr/lib/hband-tools/bash
LIBS = utils-base utils-experimental utils-pipe utils-semver utils-template utils-try ansi-codes
INSTALL_LIBS_TARGETS = $(addprefix $(LIBPREFIX)/,$(LIBS))
MANSECTION = 1
MANPREFIX = /usr/share/man/man$(MANSECTION)
MANCOMPRESSOR = gzip
MANCOMPRESSORSFX = .gz
GEN_MANPAGES = $(foreach name,$(LIBS),gen/bash-$(name).$(MANSECTION)$(MANCOMPRESSORSFX))
INSTALL_LIBS_MANPAGE_TARGETS = $(foreach name,$(LIBS),$(MANPREFIX)/bash-$(name).$(MANSECTION)$(MANCOMPRESSORSFX))

.PHONY: install-bash-utils install-bash-util-libs install-bash-util-manpages
install-bash-utils: install-bash-util-libs install-bash-util-manpages
install-bash-util-libs: $(INSTALL_LIBS_TARGETS)
install-bash-util-manpages: $(INSTALL_LIBS_MANPAGE_TARGETS)

$(INSTALL_LIBS_TARGETS): $(LIBPREFIX)/%: % | $(LIBPREFIX)
	$(INSTALL_WRAPPER) install $< $@
	@echo remove $@ >> uninstall.sh

$(LIBPREFIX):
	$(mkdir-recipe)

$(GEN_MANPAGES): gen/bash-%.$(MANSECTION)$(MANCOMPRESSORSFX): %.pod | gen/
	pod2man --name=bash-$(basename $(notdir $<)) --section $(MANSECTION) --utf8 $< | $(MANCOMPRESSOR) > $@~ && mv $@~ $@
gen/:
	mkdir gen

$(INSTALL_LIBS_MANPAGE_TARGETS): $(MANPREFIX)/%: gen/%
	$(INSTALL_WRAPPER) install -p -m 0644 $< $@
