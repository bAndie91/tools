
INSTALL_WRAPPER =

define mkdir-recipe
	[ -d $@ ] || $(INSTALL_WRAPPER) mkdir -p $@
	@echo remove $@ >> uninstall.sh
endef


.PHONY: default
default:
	$(error try `make INSTALL_WRAPPER=sudo install-bash-command-not-found-handle install-libs´)


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

.PHONY: install-libs
install-libs: $(INSTALL_LIBS_TARGETS)

$(INSTALL_LIBS_TARGETS): $(LIBPREFIX)/%: % | $(LIBPREFIX)
	$(INSTALL_WRAPPER) install $< $@
	@echo remove $@ >> uninstall.sh

$(LIBPREFIX):
	$(mkdir-recipe)
